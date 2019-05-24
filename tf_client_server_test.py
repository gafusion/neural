import os
import numpy as np
from brainfusetf import btf_connect
import time

#TGLF
model='tglfnn/models/2IONS.pb'
input = np.atleast_2d(
    [2.5292478, 0.002054744851077791, 0.19582141314524046, 0.015206952656657028, 0.10706003099184846, 0.709041910395563, 0.008792046830234525, 0.05097492034911634, 0.08377110300000003, 0.015314464854104462, 1.4051154, -0.051693646899480014, 1.1735722000000002, -0.003961595859164202, 0.5121150391937515, 5.266465158818461, 0.5951102640095797, 0.44138967638425863, 1.8782716794118017, 1.7804539917353261, 0.5108759421285336, 3.2217358623690386, 1.0113503654647604, 0.10764808932018519])
print(input.shape)

t0=time.time()
with btf_connect(path=model) as tf:
    print(tf.info())
print(time.time()-t0)

t0=time.time()
with btf_connect(path=model) as tf:
    print(tf.run(input=input))
print(time.time()-t0)


#EPED
model='eped1nn/models/DIIID.pb'
input = np.atleast_2d(
    [0.476, 3,4,0.54,1.2,1.95,2,3.36,1.695,1.8])
print(input.shape)

t0=time.time()
with btf_connect(path=model) as tf:
    print(tf.info())
print(time.time()-t0)

t0=time.time()
with btf_connect(path=model) as tf:
    print(tf.run(input=input))
print(time.time()-t0)
