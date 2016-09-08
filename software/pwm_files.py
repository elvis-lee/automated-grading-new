#! /usr/bin/python

from waveform_helper import WaveFormFileHelper

tasks = [
        {'length': 25000, 'sequence': [(0, 3, 40)]},
        {'length': 25000, 'sequence': [(0, 31, 100)]},
        {'length': 50000, 'sequence': [(0, 2, 20), (25000, 10, 80)]},
        {'length': 60000, 'sequence': [(0, 5, 10), (20000, 10, 40), (40000, 15, 90)]},
]

tasks.append({
    'length': 20000 * 20,
    'sequence': [(x * 20000, x, (8 * x + 10) % 102) for x in range(20)]
})
tasks.append({
    'length': 17500 * 40,
    'sequence': [(x * 17500, (x + 17) % 32, (1000 - 4 * x) % 102) for x in range(40)]
})


for idx, task in enumerate(tasks):
    filename = 'input_cases/%d.in.txt' % (idx + 1)
    wfh = WaveFormFileHelper(filename, 'w')
    with wfh:
        wfh.write('P', len(task['sequence']), 0)
        wfh.write('L', task['length'], 0)
        for event in task['sequence']:
            bv = (event[1] | (event[2] << 5))
            wfh.write('D', event[0]     , bv            )
            wfh.write('D', event[0] + 5 , bv | (1 << 12))
            wfh.write('D', event[0] + 50, bv            )
    
