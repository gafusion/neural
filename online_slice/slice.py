import sys, glob, re, os
os.environ.setdefault('NEURAL', '/Users/meneghini/Coding/neural')
sys.path.insert(0, os.environ['NEURAL'])
from brainfuse import *
import numpy
from numpy import ma
from bokeh.io import curdoc
from bokeh.layouts import row, column, widgetbox
from bokeh.models import ColumnDataSource, LabelSet, Label
from bokeh.models.widgets import Slider, TextInput, CheckboxButtonGroup, Dropdown
from bokeh.plotting import figure

os.chdir(str(os.path.abspath(os.path.dirname(unicode(__file__, sys.getfilesystemencoding())))))

N = 100
colorblind_line_cycle = ['#377eb8', '#ff7f00', '#4daf4a', '#f781bf', '#a65628', '#984ea3', '#999999', '#e41a1c',
                         '#dede00'] * 20

# load model details
execfile('model.py') in locals(), globals()

# load NNs
nets = {}
for k, file in enumerate(glob.glob(path)):
    nets[k] = net = brainfuse(file)
outputNames = net.outputNames
inputNames = net.inputNames
scale_mean_in = net.scale_mean_in
scale_deviation_in = net.scale_deviation_in

cases['average']={}
for k, s in enumerate(inputNames):
    cases['average'][s]=scale_mean_in[k]

# Set up inputs
cases_dropdown = Dropdown(label="Cases", menu=[(case,case) for case in cases.keys()])

n = 2.
slider = {}
for k, s in enumerate(inputNames):
    if positive_inputs:
        positive_inputs_value = (scale_mean_in[k] + n * scale_deviation_in[k])/100.
    else:
        positive_inputs_value = numpy.nan
    slider[s] = dict(value=cases.values()[0].get(s,scale_mean_in[k]),
                     start=numpy.nanmax([positive_inputs_value, scale_mean_in[k] - n * scale_deviation_in[k]]),
                     end=scale_mean_in[k] + n * scale_deviation_in[k], step=scale_deviation_in[k] / 100.)
    slider[s]['slider'] = Slider(title=mapper.get(s,s), **slider[s])
hold_button = CheckboxButtonGroup(labels=["hold"], active=[])

# Set up outputs
out_buttons = CheckboxButtonGroup(labels=[mapper.get(s,s) for s in outputNames], active=[0])

# Set up base plot
plot = figure(plot_height=800, plot_width=800, title=title,
              tools="crosshair,pan,reset,save,box_zoom")

# vertical line
vsource = ColumnDataSource(data=dict(x=[0, 0], y=[-1, 1]))
plot.line('x', 'y', source=vsource, line_width=2, line_alpha=0.6, color='#000000')

# line patch and label
x = numpy.linspace(0, 1, N)
outputs = {}
for k, s in enumerate(outputNames):
    outputs[s] = dict(dsource=ColumnDataSource(data=dict(x=x, y=x)),
                      psource=ColumnDataSource(data=dict(x=numpy.append(x, x), y=numpy.append(x, x))),
                      lsource=ColumnDataSource(data=dict(x=[0], y=[0], text=[s])))
    outputs[s]['line'] = plot.line('x', 'y', name=s, source=outputs[s]['dsource'], line_width=3, line_alpha=0.6,
                                   visible=False, color=colorblind_line_cycle[k])  # , legend=s)
    outputs[s]['patch'] = plot.patch('x', 'y', source=outputs[s]['psource'], fill_alpha=0.2, line_width=0, line_alpha=0,
                                     visible=False, color=colorblind_line_cycle[k])
    outputs[s]['label'] = Label(x=0, y=0, text=mapper.get(s,s), x_offset=0, y_offset=0, render_mode='canvas', visible=False,
                                text_color=colorblind_line_cycle[k])
    plot.add_layout(outputs[s]['label'])

last = {}
last['slider'] = None
last['slider_old'] = None
last['x'] = 0
last['y'] = 0
last['e'] = 0
last['vmin'] = numpy.nan
last['vmax'] = numpy.nan
last_slider = [None]

def update_data(attrname, old, new, nets, svar):
    if svar == '__buttons__':
        last['slider'] = last['slider_old']
    elif not len(hold_button.active):
        last['slider'] = svar

    # update sliders
    if svar=='__cases__':
        for s in slider:
            slider[s]['value'] = slider[s]['slider'].value = cases[cases_dropdown.value][s]

    else:
        for s in slider:
            slider[s]['value'] = slider[s]['slider'].value

    if last['slider_old'] != last['slider'] or svar != last['slider'] or svar == '__buttons__':
        last['slider_old'] = last['slider']

        # generate data for NN
        indata = {}
        for s in slider:
            if s == last['slider']:
                indata[s] = numpy.linspace(slider[s]['start'], slider[s]['end'], N)
                plot.xaxis.axis_label = mapper.get(last['slider'],last['slider'])
                hold_button.labels[0] = 'keep plotting as function of %s' % mapper.get(last['slider'],last['slider'])
            else:
                indata[s] = numpy.array([slider[s]['value']] * N)

        # run NN
        out, sut, targets, nets, out_ = activateNets(nets, indata)

        # restore last, then update what needs update
        vmin = numpy.nan
        vmax = numpy.nan
        x = last['x']
        y = last['y']
        e = last['e']
        lowerband = y - e
        upperband = y + e

        # if nothing is active
        if not len(out_buttons.active):
            vmin = numpy.nanmin([force_vmin, vmin, numpy.nanmin(lowerband)])
            vmax = numpy.nanmax([vmax, numpy.nanmax(upperband)])

        # Generate the new curves
        for ka in out_buttons.active:
            s = outputNames[ka]

            last['x'] = x = indata[last['slider']]
            last['y'] = y = out[:, ka]
            last['e'] = e = sut[:, ka]

            lowerband = y - e
            upperband = y + e

            outputs[s]['dsource'].data = dict(x=x, y=y)
            outputs[s]['label'].x = x[0]
            outputs[s]['label'].y = y[0]
            outputs[s]['psource'].data = dict(x=numpy.append(x, x[::-1]), y=numpy.append(lowerband, upperband[::-1]))

            vmin = numpy.nanmin([force_vmin, vmin, numpy.nanmin(lowerband)])
            vmax = numpy.nanmax([vmax, numpy.nanmax(upperband)])
            last['vmin'] = vmin
            last['vmax'] = vmax

            outputs[s]['line'].visible = True
            outputs[s]['patch'].visible = True
            outputs[s]['label'].visible = True

        for ka in range(len(out_buttons.labels)):
            if ka not in out_buttons.active:
                s = outputNames[ka]

                outputs[s]['dsource'].data = dict(x=x, y=y)
                outputs[s]['lsource'].data = dict(x=[x[0]], y=[y[0]])
                outputs[s]['psource'].data = dict(x=numpy.append(x, x[::-1]),
                                                  y=numpy.append(lowerband, upperband[::-1]))

                outputs[s]['line'].visible = False
                outputs[s]['patch'].visible = False
                outputs[s]['label'].visible = False

    vsource.data = dict(x=[slider[last['slider']]['value'], slider[last['slider']]['value']],
                        y=[last['vmin'], last['vmax']])

for s in slider:
    slider[s]['slider'].on_change('value', lambda attrname, old, new, nets=nets, svar=s: update_data(attrname, old, new, nets,  svar))
out_buttons.on_change('active', lambda attrname, old, new, nets=nets, svar='__buttons__': update_data(attrname, old, new, nets, svar))
cases_dropdown.on_change('value', lambda attrname, old, new, nets=nets, svar='__cases__': update_data(attrname, old, new, nets, svar))

# Set up layouts and add to document
if len(cases)>1:
    controls = widgetbox(*([cases_dropdown]+[slider[s]['slider'] for s in sorted(slider.keys(),key=lambda x:mapper.get(x,x).lower())]))
else:
    controls = widgetbox(*[slider[s]['slider'] for s in sorted(slider.keys(),key=lambda x:mapper.get(x,x).lower())])
curdoc().add_root(out_buttons)
curdoc().add_root(row(controls, column(plot, hold_button)))
curdoc().title = title

# refresh before starting
update_data(None, None, None, nets, slider.keys()[0])
