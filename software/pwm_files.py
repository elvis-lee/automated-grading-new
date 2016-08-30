#! /usr/bin/python

from waveform_helper import WaveFormFileHelper

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


for idx, task in enumerate(tasks):
    filename = 'output/%d.in.txt' % (idx + 1)
    wfh = WaveFormFileHelper(filename, 'w')
    with wfh:
        wfh.write('P', len(task['sequence']), 0)
        wfh.write('L', task['length'], 0)
        for event in task['sequence']:
            data = (event[1] | (event[2] << 5))
            wfh.write('D', event[0], data)
    
