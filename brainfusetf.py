import SocketServer
import socket
import threading
import time
import os
import sys
import numpy as np
from numpy import *
import struct

default_serve_port=8887

#=======================
# helper functions
#=======================
def send_data(sock, path, data):
    payload='{path}&{shape}&[{data}]'.format(path=path,
                                                shape=data.shape,
                                                data=','.join(map(lambda x:'%f'%x,data.flatten())))
    return send_msg(sock, payload)

def send_ask_info(sock, path):
    payload='{path}&(?,?)'.format(path=path)
    return send_msg(sock, payload)

def send_info(sock, path, x_names, y_names):
    payload='{path}&{x_names}&{y_names}'.format(path=path,
                                                  x_names=repr(map(str,x_names)),
                                                  y_names=repr(map(str,y_names)))
    return send_msg(sock, payload)

def send_msg(sock, msg):
    # Prefix each message with a 4-byte length (network byte order)
    msg = struct.pack('=I', len(msg)) + msg
    sock.sendall(msg)

def parse_data(data):
    try:
        path,shape,data=data.split('&')
    except Exception:
        print(data)
        raise
    return path, np.reshape(eval(data),eval(shape))

def parse_info(data):
    path,x_names,y_names=data.split('&')
    return path, eval(x_names), eval(y_names)

def recv_msg(sock):
    # Read message length and unpack it into an integer
    raw_msglen = recvall(sock, 4)
    if not raw_msglen:
        return None
    msglen = struct.unpack('=I', raw_msglen)[0]
    # Read the message data
    return recvall(sock, msglen)

def recvall(sock, n):
    # Helper function to recv n bytes or return None if EOF is hit
    data = b''
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data += packet
    return data

#=======================
# client
#=======================
class btf_connect(object):
    def __init__(self, path, host=os.environ.get('BTF_HOST','localhost'), port=os.environ.get('BTF_PORT',default_serve_port)):
        self.host = host
        self.port = port
        self.path = path
        self.connection = None
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __enter__(self):
        self.sock.connect((self.host, self.port))
        return self

    def __exit__(self, exec_type, exec_value, exec_tb):
        self.sock.close()

    def info(self):
        send_ask_info(self.sock, self.path)
        path,self.x_names,self.y_names=parse_info(recv_msg(self.sock))
        return self.x_names,self.y_names

    def run(self, input):
        if isinstance(input,dict):
            if not hasattr(self,'x_names') or not hasattr(self,'y_names'):
                with btf_connect(host=self.host,port=self.port,path=self.path) as btf:
                    self.x_names,self.y_names=btf.info()
            print(self.x_names)
            print(self.y_names)
            input=np.array([input[name] for name in self.x_names]).T
        send_data(self.sock, self.path, input)
        path,output=parse_data(recv_msg(self.sock))
        if isinstance(input,dict):
            output={name:output[:,k] for k,name in enumerate(self.y_names)}
        return output

def activateNets(nets, dB):
    '''
    :param nets: dictionary with OMFITbrainfuse objects (or path where to load NNs from)

    :param dB: dictionary with entries to run on

    :return: tuple with (out,sut,targets,nets,out_)
    '''
    if isinstance(nets,basestring):
        nets={k:OMFITpath(file) for k,file in enumerate(glob.glob(nets))}
    elif not isinstance(nets,(list,tuple)):
        nets={0:nets}
    net=nets.values()[0]

    with btf_connect(path=net.filename) as btf:
        inputNames,outputNames=btf.info()
    targets=array([dB[k] for k in outputNames]).T

    out_=empty((len(atleast_1d(dB.values()[0])),len(outputNames),len(nets)))
    for k,n in enumerate(nets):
        with btf_connect(path=net.filename) as btf:
            out_[:,:,k]=btf.run(dB)
    out=mean(out_,-1)
    sut=std(out_,-1)
    return out,sut,targets,nets,out_

#=======================
# server
#=======================
if __name__ == "__main__":
    import tensorflow as tf
    from tensorflow.python.platform import gfile

    serve_port=default_serve_port
    if len(sys.argv)>1:
        serve_port=int(sys.argv[1])

    models={}
    def activate(path, input):
        '''
        high level function that handles models buffering
        :param path: path of the model
        :param input: input array (or None to get xnames,ynames)
        :return: output array or xnames,ynames
        '''
        if not path.startswith(os.sep):
            path=os.path.split(__file__)[0]+os.sep+path
        if path not in models:
            models[path]=model(path=path)
        elif os.path.getmtime(path)>models[path].load_time:
            models[path].close()
            models[path]=model(path=path)
        if input is None:
            return models[path].x_names,models[path].y_names
        else:
            return models[path].activate(input)

    class model(tf.Session):
        def __init__(self, target='', graph=None, config=None, path=None):
            tf.Session.__init__(self, target=target, graph=graph, config=config)
            self.__enter__()

            if not path.startswith(os.sep):
                path=os.path.split(__file__)[0]+os.sep+path
            self.path=path
            print('Loading %s'%self.path)
            self.load_time=os.path.getmtime(self.path)
            with gfile.FastGFile(self.path, 'rb') as f:
                graph_def = tf.GraphDef()
                graph_def.ParseFromString(f.read())

            self.y, self.x_names, self.y_names = tf.import_graph_def(graph_def, return_elements=['unnormalized_nn/y:0',
                                                                                                 'x_names:0',
                                                                                                 'y_names:0'], name='')
            self.x_names=self.x_names.eval()
            self.y_names=self.y_names.eval()

        def activate(self,input):
            '''
            :param input: input array
            :return: output array
            '''
            print('Serving %s'%self.path)
            return self.run(self.y, feed_dict={'x:0': input})

    class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

        def handle(self):
            msg=recv_msg(self.request)
            if msg is None:
                print("{}: {}".format(self.client_address[0],'-- no message --'))
                return
            print("{}: message length {}".format(self.client_address[0],len(msg)))
            query=msg.split('&')[1]
            #respond to info request
            if query=='(?,?)':
                print('INFO-MODE')
                path=msg.split('&')[0]
                x_names,y_names=activate(path=path,input=None)
                send_info(self.request,path,x_names,y_names)
            #respond to data request
            else:
                print('DATA-MODE: serve starts')
                while True:
                    try:
                        if msg is not None:
                            path,input=parse_data(msg)
                            output=activate(path=path,input=input)
                        send_data(self.request,path,output)
                        msg=recv_msg(self.request)
                    except Exception as _excp:
                        if 'Broken pipe' in repr(_excp) or 'Connection reset by peer' in repr(_excp) or 'Protocol wrong type for socket' in repr(_excp):
                            print('DATA-MODE: serve ends')
                            break
                        else:
                            raise

    class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
        pass

    server = ThreadedTCPServer(('0.0.0.0', serve_port), ThreadedTCPRequestHandler)
    print('-- BASH shell --')
    print("export BTF_HOST=%s"%socket.gethostname())
    print("export BTF_PORT=%d"%serve_port)
    print('-- TCSH shell --')
    print("setenv BTF_HOST %s"%socket.gethostname())
    print("setenv BTF_PORT %d"%serve_port)
    try:
        server.serve_forever()
    except (KeyboardInterrupt, SystemExit):
        pass
    finally:
        server.shutdown()
        server.server_close()
