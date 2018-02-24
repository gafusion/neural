import numpy as np
from brainfuse_tf import btf_connect
import time

model='/tmp/meneghini/OMFIT/OMFIT_2018-02-21_10_23_05_903282/project/objects/dir_2018-02-23__16:51/16__788939/nn_SAT1_mb_128_abs_split_30x4/saved_model.pb'
#data['path']='/tmp/meneghini/OMFIT/OMFIT_2018-02-16_23_11_30_896962/project/objects/dir_2018-02-20__15:56/08__643709/mb_1024'
input = np.atleast_2d(
    [[ 5.45338e-01,  4.17925e-02,  7.21986e-03,  1.24956e-01,
       -1.37899e-01,  1.58491e+00, -4.20101e-03,  1.55640e+00,
        8.36932e+00,  1.02569e+00,  2.05921e+00, -4.45231e-01,
        3.00670e+00,  2.06023e+00,  2.38220e+00,  7.66336e-01,
        3.20824e-01,  1.14110e+00,  3.21049e-01,  3.36619e-01,
        1.87324e+00]]*2)

t0=time.time()
with btf_connect(path=model) as tf:
    print tf.run(input=input)
    print tf.run(input=input*2)
    time.sleep(1)
    print tf.run(input=input*3)
print(time.time()-t0)