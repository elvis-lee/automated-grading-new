from waveform_helper import WaveFormFileHelper

# task1: length=20000, P=3, R=7
LENGTH = 20000
P = 3
R = 7


period = 40 * (P + 1)
on_duration = int(period * R / 31)
off_duration = int(period - on_duration)
on_off_durations = [off_duration, on_duration]

filename = 'sample_output1'
wfh = WaveFormFileHelper(filename, 'w')
with wfh:
    if R == 0:
        wfh.write('D', 0, 0)
    elif R == 31:
        wfh.write('D', 0, 1)
    else:
        t = 0
        v = 1
        while t < LENGTH:
            wfh.write('D', t, v)
            t += on_off_durations[v]
            v = 1 - v
