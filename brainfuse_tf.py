import SocketServer
import socket
import threading
import time
import os
import numpy as np
from numpy import *
import struct

serve_port=8882

#=======================
# helper functions
#=======================
def send_data(sock, path, data):
    payload='{path}::{shape}::[{data}]'.format(path=path,
                                                shape=data.shape,
                                                data=','.join(map(lambda x:'%f'%x,data.flatten())))
    return send_msg(sock, payload)

def send_ask_info(sock, path):
    payload='{path}::(?,?)'.format(path=path)
    return send_msg(sock, payload)

def send_info(sock, path, x_names, y_names):
    payload='{path}::{x_names}::{y_names}'.format(path=path,
                                                  x_names=repr(map(str,x_names)),
                                                  y_names=repr(map(str,y_names)))
    return send_msg(sock, payload)

def send_msg(sock, msg):
    # Prefix each message with a 4-byte length (network byte order)
    msg = struct.pack('=I', len(msg)) + msg
    sock.sendall(msg)

def parse_data(data):
    try:
        path,shape,data=data.split('::')
    except Exception:
        print(data)
        raise
    return path, np.reshape(eval(data),eval(shape))

def parse_info(data):
    path,x_names,y_names=data.split('::')
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
    def __init__(self, host='localhost', port=serve_port, path=None):
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

#=======================
# server
#=======================
if __name__ == "__main__":
    import tensorflow as tf
    from tensorflow.python.platform import gfile

    class model(tf.Session):
        def __init__(self, target='', graph=None, config=None, path=None):
            tf.Session.__init__(self, target=target, graph=graph, config=config)
            self.__enter__()
            with gfile.FastGFile(path, 'rb') as f:
                graph_def = tf.GraphDef()
                graph_def.ParseFromString(f.read())
            self.y, self.x_names, self.y_names = tf.import_graph_def(graph_def, return_elements=['unnormalized_nn/y:0',
                                                                                                 'x_names:0',
                                                                                                 'y_names:0'], name='')
            self.x_names=self.x_names.eval()
            self.y_names=self.y_names.eval()

        def activate(self,input):
            return self.run(self.y, feed_dict={'x:0': input})

    models={}
    def activate(path, input):
        if path not in models:
            models[path]=model(path=path)
        if input is None:
            return models[path].x_names,models[path].y_names
        else:
            return models[path].activate(input)

    class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

        def handle(self):
            msg=recv_msg(self.request)
            if msg is None:
                print("{}: {}".format(self.client_address[0],'-- no message --'))
                return
            print("{}: {}".format(self.client_address[0],msg))
            query=msg.split('::')[1]
            #respond to info request
            if query=='(?,?)':
                print('INFO-MODE')
                path=msg.split('::')[0]
                x_names,y_names=activate(path=path,input=None)
                send_info(self.request,path,x_names,y_names)
            #respond to data request
            else:
                print('DATA-MODE: serve starts')
                while True:
                    try:
                        if msg is not None:
                            path,input=parse_data(msg)
                            print input
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
    print("Serving on port %d"%serve_port)

    try:
        server.serve_forever()
    except (KeyboardInterrupt, SystemExit):
        pass
    finally:
        server.shutdown()
        server.server_close()
