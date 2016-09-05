template_file = 'grading_template.py'
out_dir = 'grading_scripts/'

with open(template_file, 'r') as f:
    input_lines = f.readlines()

for i in range(6):
    input_lines[5] = 'TASK_ID = %d\n' % i
    out_file_name = '%s/script_%d.py' % (out_dir, i)
    with open(out_file_name, 'w') as fo:
        for line in input_lines:
            fo.write(line)
