import SocketServer
import socket
import threading
import time
import os
import numpy as np
from numpy import *
import struct

serve_port=8887

#=======================
# helper functions
#=======================
def send_data(sock, path, data):
    payload='{path}::{shape}::[{data}]'.format(path=path,
                                                shape=data.shape,
                                                data=','.join(map(lambda x:'%f'%x,data.flatten())))
    return send_msg(sock, payload)

def send_msg(sock, msg):
    # Prefix each message with a 4-byte length (network byte order)
    msg = struct.pack('>I', len(msg)) + msg
    sock.sendall(msg)

def recv_data(sock):
    data=recv_msg(sock)
    path,shape,data=data.split('::')
    return path, np.reshape(eval(data),eval(shape))

def recv_msg(sock):
    # Read message length and unpack it into an integer
    raw_msglen = recvall(sock, 4)
    if not raw_msglen:
        return None
    msglen = struct.unpack('>I', raw_msglen)[0]
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
class tf_connect(object):
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

    def run(self, input):

        send_data(self.sock, self.path, input)
        path,output=recv_data(self.sock)

        print "Sent:     {}".format(input)
        print "Received:     {}".format(output)

#=======================
# server
#=======================
if __name__ == "__main__":
    import tensorflow as tf

    class model(tf.Session):
        def __init__(self, target='', graph=None, config=None, path=None):
            tf.Session.__init__(self, target=target, graph=graph, config=config)
            self.__enter__()
            graph=tf.Graph()
            tf.saved_model.loader.load(self,[tf.saved_model.tag_constants.SERVING],path)
            self.y = tf.get_collection('nn_model')

        def runme(self,input):
            return self.run(self.y, feed_dict={'x:0': input,'keep_prob:0':1.0})[0]

    models={}
    def activate(path, input):
        if path not in models:
            models[path]=model(path=path)
        return models[path].runme(input)

    class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

        def handle(self):
            print("{} wrote:".format(self.client_address[0]))
            path,input=recv_data(self.request)
            print path
            print input
            output=activate(path=path,input=input)
            send_data(self.request,path,output)

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
