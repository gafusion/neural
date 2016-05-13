import numpy
from numpy import *
import os
from fann2 import libfann

__all__ = ['libfann', 'brainfuse', 'activateNets', 'activateNetsFile', 'activateMergeNets']

class brainfuse(libfann.neural_net):
    def __init__(self,filename,**kw):
        libfann.neural_net.__init__(self)
        self.filename=filename
        self.scale_mean_in=None
        self.scale_mean_out=None
        self.scale_deviation_in=None
        self.scale_deviation_out=None
        self.inputNames=[]
        self.outputNames=[]
        self.norm_output=[]
        self.MSE=None
        self.load()

    def denormOutputs(self,inputs,outputs):
        return self.normOutputs(inputs,outputs,denormalize=True)

    def normOutputs(self, inputs, outputs, denormalize=False):
        for ko,itemo in enumerate(self.outputNames):
            norm=1.
            for ki,itemi in enumerate(self.inputNames):
                norm*=(inputs[:,ki]**self.norm_output[ko][ki])
                #print('*normalize %s with %s'%(itemo,itemi))
            if isinstance(norm,ndarray):
                if not denormalize:
                    #print('*apply normalization on %s: %s'%(itemo,repr(self.norm_output[ko])))
                    outputs[:,ko]/=norm
                else:
                    #print('*apply denormalization')
                    outputs[:,ko]*=norm
        return outputs

    def activate(self, dB, verbose=False):
        inputs=dB
        if isinstance(dB,dict):
            inputs=[]
            for k in self.inputNames:
                inputs.append(dB[k].astype(float))
            inputs=array(inputs).T

        targets=[]
        if isinstance(dB,dict):
            for k in self.outputNames:
                try:
                    targets.append(dB[k].astype(float))
                except:
                    if verbose:
                        print(k+' not in dB')
            targets=array(targets).T

        out=[]
        for k,tmp in enumerate(inputs):
            tmp=(tmp-self.scale_mean_in)/self.scale_deviation_in
            tmp=array(self.run(tmp))
            tmp=tmp*self.scale_deviation_out+self.scale_mean_out
            out.append(tmp)
        out=array(out)

        out=self.denormOutputs(inputs,out)

        return out, targets

    def __getstate__(self):
        self.save()
        tmp={'filename':self.filename}
        return tmp

    def __setstate__(self,tmp):
        self.__init__(tmp['filename'])

    def save(self):
        super(libfann.neural_net, self).save(self.filename)

        with open(self.filename,'Ur') as f:
            original=f.readlines()
        tmp=[]
        for line in original:
            if 'neurons' in line and self.scale_mean_in is not None:
                tmp.append('scale_included=1')
                tmp.append('scale_mean_in='+' '.join(['%6.6f'%k for k in self.scale_mean_in]))
                tmp.append('scale_deviation_in='+' '.join(['%6.6f'%k for k in self.scale_deviation_in]))
                tmp.append('scale_new_min_in='+' '.join(['%6.6f'%-1.0]*self.get_num_input()))
                tmp.append('scale_factor_in='+' '.join(['%6.6f'%1.0]*self.get_num_input()))
                tmp.append('scale_mean_out='+' '.join(['%6.6f'%k for k in self.scale_mean_out]))
                tmp.append('scale_deviation_out='+' '.join(['%6.6f'%k for k in self.scale_deviation_out]))
                tmp.append('scale_new_min_out='+' '.join(['%6.6f'%-1.0]*self.get_num_output()))
                tmp.append('scale_factor_out='+' '.join(['%6.6f'%1.0]*self.get_num_output()))
                tmp.append(line.strip())
            elif 'scale_' not in line or self.scale_mean_in is None:
                tmp.append(line.strip())

        if len(self.inputNames):
            tmp.append('input_names='+' '.join([repr(k) for k in self.inputNames]))
        if len(self.outputNames):
            tmp.append('output_names='+' '.join([repr(k) for k in self.outputNames]))
        if len(self.norm_output):
            tmp.append('norm_output='+' '.join([repr(float(k)) for k in array(self.norm_output).flatten()]))
        if self.MSE is not None:
            tmp.append('MSE='+' '+str(self.MSE))

        with open(self.filename,'w') as f:
            f.write('\n'.join(tmp))

        #print('\n'.join(tmp))

    def load(self):
        if not os.stat(self.filename).st_size:
            return self
        super(libfann.neural_net, self).create_from_file(self.filename)
        with open(self.filename,'Ur') as f:
            tmp=f.readlines()
        for line in tmp:
            line=line.rstrip()
            if 'input_names=' in line:
                self.inputNames=eval((line.replace('input_names=','[')+']').replace("' '","','"))
            elif 'output_names=' in line:
                self.outputNames=eval((line.replace('output_names=','[')+']').replace("' '","','"))
            elif 'norm_output=' in line:
               tmp=array(eval((line.replace('norm_output=','[')+']').replace(" ",",")))
               self.norm_output=reshape(tmp,(len(self.outputNames),len(self.inputNames)))
            elif 'MSE=' in line:
                what,value=line.strip().split('=')
                self.MSE=eval(value)
            elif 'scale_mean_in' in line:
                what,value=line.strip().split('=')
                self.scale_mean_in=array(eval('['+','.join(value.split(' '))+']'))
            elif 'scale_mean_out' in line:
                what,value=line.strip().split('=')
                self.scale_mean_out=array(eval('['+','.join(value.split(' '))+']'))
            elif 'scale_deviation_in' in line:
                what,value=line.strip().split('=')
                self.scale_deviation_in=array(eval('['+','.join(value.split(' '))+']'))
            elif 'scale_deviation_out' in line:
                what,value=line.strip().split('=')
                self.scale_deviation_out=array(eval('['+','.join(value.split(' '))+']'))
        return self

def activateNets(nets, dB):
    net=nets[nets.keys()[0]]
    out_=empty((len(dB[dB.keys()[0]]),len(net.outputNames),len(nets)))
    for k,n in enumerate(nets):
        out_[:,:,k],targets=nets[n].activate(dB)
    out=mean(out_,-1)
    sut=std(out_,-1)
    return out,sut,targets,nets,out_

def activateNetsFile(nets, inputFile, targetFile=None):
    net=nets[nets.keys()[0]]
    dB={}
    for k in net.inputNames:
        dB[k]=[]
    for line in open(inputFile,'Ur').readlines()[1:]:
        for k,item in enumerate(line.split()):
            dB[net.inputNames[k]].append( float(item))

    if targetFile is not None:
        for k in net.outputNames:
            dB[k]=[]
        for line in open(targetFile,'Ur').readlines()[1:]:
            for k,item in enumerate(line.split()):
                dB[net.outputNames[k]].append( float(item))
    for k in net.inputNames:
        dB[k]=array(dB[k])
    return activateNets(nets,dB)

def activateMergeNets(nets, dB, merge_nets):
        net=nets[nets.keys()[0]]
        out_0=empty((len(dB[dB.keys()[0]]),len(net.outputNames),len(nets)))
        index=net.inputNames.index(merge_nets)
        centers=empty(len(nets))
        merge_norm=empty(len(nets))
        for k,n in enumerate(nets):
            out_0[:,:,k],targets=nets[n].activate(dB)
            centers[k]=nets[n].scale_mean_in[index]
            merge_norm[k]=nets[n].scale_deviation_in[index]

        w=dB[merge_nets][:,newaxis]-centers[newaxis,:]
        w=exp(-(w/merge_norm[newaxis,:])**2)
        w=w/numpy.sum(w,1)[newaxis,:].T

        out_=out_0*w[:,newaxis,:]
        out=numpy.sum(out_,-1)
        sut=sqrt(numpy.sum(w[:,newaxis,:]*(out_0-out[:,:,newaxis])**2,-1))
        return out,sut,targets,nets,out_0
