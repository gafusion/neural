# file processed by 2to3
from __future__ import print_function, absolute_import
from builtins import map, filter, range
import numpy as np
from brainfusetf import btf_connect
import time


def print_nice(model, input_names, input, output_names, output):
    print('=' * 20)
    print(model)
    print('=' * 20)
    print('INPUTS')
    print('-' * 20)
    for k, item in enumerate(input_names):
        print('%s = ' % item.ljust(30),end='')
        for row in range(input.shape[0]):
            print(' % 9.3f' % input[row, k],end='')
        print('')
    print('-' * 20)
    print('OUTPUTS')
    print('-' * 20)
    for k, item in enumerate(output_names):
        print('%s = ' % item.ljust(30),end='')
        for row in range(input.shape[0]):
            print(' % 9.3f' % output[row, k],end='')
        print('')
    print('')


# TGLF-NN example
model = 'tglfnn/models/2IONS.pb'
input = np.atleast_2d([0.772939, 0.0331182, 0.00134146, 0.0336953, 0.266209, -0.273578, 1.62193, -0.00299774, 3.79572, 49.4694, 3.09397, 4.09682, -0.365457, 3.34791, 2.65195, 2.91466, 0.851835, 0.405665, 1.18246, 0.0922339, 0.205078, 1.19789, 0.0917608, 1.99567])

with btf_connect(path=model) as tf:
    input_names, output_names = tf.info()

with btf_connect(path=model) as tf:
    output = tf.run(input=input)

print_nice(model, input_names, input, output_names, output)

# TGLF-NN example
model = 'tglfnn/models/3IONS.pb'
input = np.atleast_2d([0.74519, 0.013754, 0.00768935, 0.00820694, 0.0291257, 0.0653404, -0.10914, 1.27022, -0.00264262, 1.36326, 4.17551, 0.0999972, 0.111262, -0.0440054, 1.50458, 2.47484, 2.36345, 3.07452, 0.642187, 0.11372, 0.740606, 0.0139213, 0.0298837, 0.0908604, 0.0563926, 2.1678])

with btf_connect(path=model) as tf:
    input_names, output_names = tf.info()

with btf_connect(path=model) as tf:
    output = tf.run(input=input)

print_nice(model, input_names, input, output_names, output)

# EPED-NN example
model = 'eped1nn/models/EPED_mb_128_pow_norm_common_30x10.pb'
input = np.atleast_2d([[2.0, 2.0, 5.3, 0.485, 15.0, 1.8, 2.5, 10., 6.2, 1.5],
                       [2.0, 2.0, 5.3, 0.485, 15.0, 1.8, 2.5, 10., 6.2, 2.0],
                       [2.0, 2.0, 5.3, 0.485, 15.0, 1.8, 2.5, 10., 6.2, 2.5],
                       [2.0, 2.0, 5.3, 0.485, 15.0, 1.8, 2.5, 10., 6.2, 3.0]]
                      )
print(type(input))
print(input.shape)

with btf_connect(path=model) as tf:
    input_names, output_names = tf.info()

with btf_connect(path=model) as tf:
    output = tf.run(input=input)

print_nice(model, input_names, input, output_names, output)
