import numpy as np
from brainfusetf import btf_connect
import time

model='tglfnn/models/nn_SAT0_mb_1024_abs_reg_common_stair2x2x6.pb'
input = np.atleast_2d(
    [[ 5.45338e-01,  4.17925e-02,  7.21986e-03,  1.24956e-01, -1.37899e-01,  1.58491e+00, -4.20101e-03,  1.55640e+00, 8.36932e+00,  1.02569e+00,  2.05921e+00, -4.45231e-01, 3.00670e+00,  2.06023e+00,  2.38220e+00,  7.66336e-01, 3.20824e-01,  1.14110e+00,  3.21049e-01,  3.36619e-01, 1.87324e+00]]*2)

t0=time.time()
with btf_connect(path=model) as tf:
    print tf.run(input=input)
    print tf.run(input=input*2)
    time.sleep(1)
    print tf.run(input=input*3)
print(time.time()-t0)
