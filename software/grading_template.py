import sys
import numpy

from waveform_helper import WaveFormFileHelper

TASK_ID = 0

TOLERENCE = 1

def grade(time_series, st, et, P, R):
    exp_period_ticks = (P + 1) * 40
    exp_on_ticks = exp_period_ticks * R / 31
    if R == 0 or R == 31:  # corner cases
        st_interest = st + 2 * exp_period_ticks
        et_interest = et - 2 * exp_period_ticks
        on_ratio = float(time_series[st_interest:et_interest]) / (et_interest - st_interest)
        if R == 0:
            return max(0., (on_ratio - 0.001) * -1000.)
        elif R == 31:
            return max(0., (on_ratio - 0.999) * 1000.)
    else:
        st_interest = st + exp_period_ticks
        et_interest = et - exp_period_ticks
        rising_edges = [i+1 for i in range(st_interest, et_interest - 1) if time_series[i] == 0 and time_series[i+1] == 1]
        if len(rising_edges) < 5:
            return 0.
        on_durations = [sum(time_series[rising_edges[i]:rising_edges[i+1]]) for i in range(len(rising_edges) - 1)]
        periods = [v for v in numpy.diff(rising_edges)]
        if rising_edges[0] - st_interest > exp_period_ticks:
            periods.append(rising_edges[0] - st_interst)
        if et_interest - rising_edges[-1] > exp_period_ticks:
            periods.append(et_interest - rising_edges[-1])
        period_penalty = (numpy.mean([max(0, abs(p - exp_period_ticks) - TOLERENCE) for p in periods]) ** 2) * 0.03
        ratio_penalty = (numpy.mean([max(0, abs(d - exp_on_ticks) - TOLERENCE) for d in on_durations]) ** 2) * 0.03
        return 1. - min(0.5, period_penalty) - min(0.5, ratio_penalty)

tasks = [
        {'length': 20000, 'sequence': [(0, 3, 7)]},
        {'length': 20000, 'sequence': [(0, 31, 31)]},
        {'length': 30000, 'sequence': [(0, 2, 9), (15000, 10, 10)]},
        {'length': 60000, 'sequence': [(0, 1, 1), (20000, 4, 4), (40000, 9, 9)]},
]

tasks.append({
    'length': 12500 * 20,
    'sequence': [(x * 12500, x, (x + 10) % 32) for x in range(20)]
})
tasks.append({
    'length': 12500 * 40,
    'sequence': [(x * 12500, (x + 17) % 32, (47 - x) % 32) for x in range(40)]
})

length = tasks[TASK_ID]['length']
sequence = tasks[TASK_ID]['sequence']

input_name = sys.argv[1]
events = []
wfh_r = WaveFormFileHelper(input_name, 'r')
with wfh_r:
    while True:
        pkg = wfh_r.read()
        if not pkg:
            break
        cmd, time, pvals = pkg
        if cmd == 'D':
            events.append((time, pvals & 1))

time_series = numpy.zeros(length, dtype=numpy.int)
events.append((length, 0))
for e_idx in range(len(events) - 1):
    s = max(0, events[e_idx][0])
    e = min(length, events[e_idx+1][0])
    time_series[s:e] = events[e_idx][1]

points = []
for i in range(len(sequence)):
    st = sequence[i][0]
    et = sequence[i+1][1] if i+1 < len(sequence) else length
    points.append(grade(time_series, st, et, sequence[i][1], sequence[i][2]))
final_result = numpy.mean(points)
print(final_result)
