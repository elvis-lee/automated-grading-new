#! /usr/bin/python

from waveform_helper import WaveFormFileHelper

tasks = [
        {'length': 5000, 'sequence': [(0, 3, 7)]},
        {'length': 5000, 'sequence': [(0, 31, 31)]},
        {'length': 7500, 'sequence': [(0, 2, 9), (5000, 10, 10)]},
        {'length': 7500, 'sequence': [(0, 1, 1), (2500, 4, 4), (5000, 9, 9)]},
]

tasks.append({
    'length': 10000,
    'sequence': [(x * 500, x, (x + 10) % 32) for x in range(20)]
})
tasks.append({
    'length': 20000,
    'sequence': [(x * 500, (x + 17) % 32, (47 - x) % 32) for x in range(40)]
})


for idx, task in enumerate(tasks):
    filename = 'output/%d.in.txt' % (idx + 1)
    wfh = WaveFormFileHelper(filename, 'w')
    with wfh:
        wfh.write('P', len(task['sequence']), 0)
        wfh.write('L', task['length'], 0)
        for event in task['sequence']:
            data = (event[1] | (event[2] << 5))
            wfh.write('D', event[0], data)
    
