import numpy as np
from brainfusetf import btf_connect
import time

def print_nice(model, input_names, input, output_names, output):
    for row in range(input.shape[0]):
        print('=' * 20)
        print(model)
        print('=' * 20)
        print('INPUTS')
        print('-' * 20)
        for k, item in enumerate(input_names):
            print('%s = % g' % (item.ljust(max(map(len, input_names))), input[row, k]))
        print('-' * 20)
        print('OUTPUTS')
        print('-' * 20)
        for k, item in enumerate(output_names):
            print('%s = % g' % (item.ljust(max(map(len, output_names))), output[row, k]))
    print('')

# TGLF-NN example
model = 'tglfnn/models/2IONS.pb'
input = np.atleast_2d([2.5292478, 0.002054744851077791, 0.19582141314524046, 0.015206952656657028, 0.10706003099184846, 0.709041910395563, 0.008792046830234525, 0.05097492034911634, 0.08377110300000003, 0.015314464854104462, 1.4051154, -0.051693646899480014, 1.1735722000000002, -0.003961595859164202, 0.5121150391937515, 5.266465158818461, 0.5951102640095797, 0.44138967638425863, 1.8782716794118017, 1.7804539917353261, 0.5108759421285336, 3.2217358623690386, 1.0113503654647604, 0.10764808932018519])

with btf_connect(path=model) as tf:
    input_names, output_names = tf.info()

with btf_connect(path=model) as tf:
    output = tf.run(input=input)

print_nice(model, input_names, input, output_names, output)

# EPED-NN example
model = 'eped1nn/models/EPED_mb_128_pow_norm_common_30x10.pb'
input = np.atleast_2d([0.5778, 1.8034, 2.0995, 0.2075, 1.1621, 1.8017, 2, 4.0101, 1.6984, 1.4429])

with btf_connect(path=model) as tf:
    input_names, output_names = tf.info()

with btf_connect(path=model) as tf:
    output = tf.run(input=input)

print_nice(model, input_names, input, output_names, output)
