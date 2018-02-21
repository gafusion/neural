import numpy as np
from brainfuse_tf import tf_connect
import time

model='/tmp/meneghini/OMFIT/OMFIT_2018-02-16_23_11_30_896962/project/objects/dir_2018-02-20__13:43/10__348584/mb_128'
#data['path']='/tmp/meneghini/OMFIT/OMFIT_2018-02-16_23_11_30_896962/project/objects/dir_2018-02-20__15:56/08__643709/mb_1024'
input = np.atleast_2d(
    [[1.11893e+00, 3.94863e-02, 1.13228e-03, 3.92026e-02, -6.70454e-01, 1.54321e+00, -5.67412e-03, 2.37426e+00,
     1.33564e+01, 6.60647e-01, 6.84373e-02, 1.36100e+00, 9.31754e+00, 1.62651e+00, 4.84792e+00, 2.27050e-01,
     1.86034e-02, 3.66451e-02, -7.96746e-05, 7.96746e-05, 3.40337e-03, 2.47660e+00]]*200)

t0=time.time()
with tf_connect(path=model) as tf:
    tf.run(input=input)
print(time.time()-t0)