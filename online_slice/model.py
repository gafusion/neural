import sys,os,re,numpy
from collections import OrderedDict

dir=str(os.path.abspath(os.path.dirname(unicode(__file__, sys.getfilesystemencoding()))))
model=os.path.split(dir)[1]

cases=OrderedDict()

print model

if model == 'eped1nn':
    title =  'EPED1-NN'
    path=os.environ['NEURAL']+'/eped1nn/models/EPED1_H_superH/brainfuse_*.net'
    mapper={'a':'a',
            'bt':'B_t',
            'ip':'I_p',
            'delta':'delta',
            'kappa':'kappa',
            'neped':'n_e,ped',
            'm':'m',
            'r':'R',
            'betan':'beta_n',
            'zeffped':'Z_eff,ped'}

    for sol in range(3):
        solname=['',' (unst)',' (super)'][sol]
        mapper.update({'OUT_wid_E1_%d'%sol:'Pedestal width [%psi]'+solname,
                       'OUT_p_E1_%d'%sol:'Pedestal height [MPa]'+solname,
                       })

    cases['DIII-D']={'ip':1.29774057,
                     'bt':1.69241006,
                     'r' :1.69550002,
                     'a' :0.551712097174,
                     'kappa':1.85502731628,
                     'delta':0.5417377679,
                     'neped':3.61845081369,
                     'betan':1.27835686157,
                     'zeffped':2.07267968067,
                     'm':2,
                     'z':1,
                     'mi':12,
                     'zi':6}

    cases['ITER']={  'ip':15,
                     'bt':5.3,
                     'r':6.2,
                     'a':2,
                     'kappa':1.85,
                     'delta':0.485,
                     'neped':10,
                     'betan':2.0,
                     'zeffped':1.7,
                     'm':2.5,
                     'z':1,
                     'mi':12,
                     'zi':6}

    cases['ITER half field']={  'ip':15/2.,
                                'bt':5.3/2.,
                                'r':6.2,
                                'a':2,
                                'kappa':1.85,
                                'delta':0.485,
                                'neped':10,
                                'betan':2.0,
                                'zeffped':1.7,
                                'm':2.5,
                                'z':1,
                                'mi':12,
                                'zi':6}

    cases['JET']={'ip':2.0,
                  'bt':1.7,
                  'r':2.9,
                  'a':0.92,
                  'kappa':1.7,
                  'delta':0.4,
                  'neped':4.0,
                  'betan':2.0,
                  'zeffped':1.7,
                  'm':2,
                  'z':1,
                  'mi':12,
                  'zi':6}

    positive_inputs=True
    force_vmin=0

elif model == 'tglfnn':
    title =  'TGLF-NN'

    path=os.environ['NEURAL']+'/tglfnn/models/DIIID_ion_stiffness_60_rotation/brainfuse_*.net'

    mapper={'OUT_tur_ENERGY_FLUX_1'      : 'TGLF Q_e',
            'OUT_tur_ENERGY_FLUX_i'      : 'TGLF Q_i',
            'OUT_tur_PARTICLE_FLUX_1'    : 'TGLF Gamma_e',
            'OUT_tur_STRESS_TOR_i'       : 'TGLF Pi_i',

            'OUT_ENERGY_FLUX_1'          : 'Q_e',
            'OUT_ENERGY_FLUX_i'          : 'Q_i',
            'OUT_PARTICLE_FLUX_1'        : 'Gamma_e',
            'OUT_STRESS_TOR_i'           : 'Pi_i',

            'OUT_tot_ENERGY_FLUX_1'      : 'TGLF+NEO Q_e',
            'OUT_tot_ENERGY_FLUX_i'      : 'TGLF+NEO Q_i',
            'OUT_tot_PARTICLE_FLUX_1'    : 'TGLF+NEO Gamma_e',
            'OUT_tot_STRESS_TOR_i'       : 'TGLF+NEO Pi_i',

            'OUT_neo_ENERGY_FLUX_1'      : 'NEO Q_e',
            'OUT_neo_ENERGY_FLUX_i'      : 'NEO Q_i',
            'OUT_neo_PARTICLE_FLUX_1'    : 'NEO Gamma_e',
            'OUT_neo_STRESS_TOR_i'       : 'NEO Pi_i',

            'AS_2':'nD/ne',
            'AS_3':'nC/ne',
            'BETAE':'Beta_e',
            'DELTA_LOC':'delta',
            'DRMAJDX_LOC':'dR/dr',
            'KAPPA_LOC':'kappa',
            'P_PRIME_LOC':'dP/dr',
            'Q_LOC':'q',
            'Q_PRIME_LOC':'dq/dr',
            'RLNS_1':'a/Lne',
            'RLNS_2':'a/LnD',
            'RLNS_3':'a/LnC',
            'RLTS_1':'a/Lte',
            'RLTS_2':'a/LtD',
            'RMAJ_LOC':'R',
            'RMIN_LOC':'r',
            'S_KAPPA_LOC':'dkappa/dr',
            'TAUS_2':'tD/te',
            'VEXB_SHEAR':'dV_eXb/dr',
            'VPAR_1':'V_par',
            'VPAR_SHEAR_1':'dV_par/dr',
            'XNUE':'nu_e',
            'ZEFF':'Z_eff'
            }

    positive_inputs=False
    force_vmin=numpy.nan

elif model.startswith('neojbsnn'):
    title =  'NEOjbs-NN'

    path=os.environ['NEURAL']+'/neojbsnn/models/'+model.split('-')[1]+'/brainfuse_*.net'

    mapper={}

    if model.split('-')[1]!='D_C':
        cases['Carbon']={'log10_mi2/mD':numpy.log10(12/2.),
                         'log10_zi2':numpy.log10(6)}

        cases['Tungsten']={'log10_mi2/mD':numpy.log10(184/2.),
                           'log10_zi2':numpy.log10(74)}

    positive_inputs=False
    force_vmin=numpy.nan

else:
    raise(Exception('model `%s` not recognized'%model))