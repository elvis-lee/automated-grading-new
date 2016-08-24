import struct

class WaveFormFileHelper:
    def __init__(self, filename, mode):
        self.filename = filename
        if mode[-1] != 'b':
            mode += 'b'
        self.mode = mode

    def __enter__(self):
        self.f = open(self.filename, self.mode)

    def __exit__(self, type, value, traceback):
        self.f.close()

    def write(self, cmd, timestamp, data):
        self.f.write(struct.pack('=ccIHc', b'S', cmd.encode('ascii'), timestamp, data, b'E'))

    def read(self):
        msg = self.f.read(9)
        (_, cmd, timestamp, data, _) = struct.unpack('=ccIHc', msg)
        return (cmd, timestamp, data)
