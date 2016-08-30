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
        while True:
            msg = self.f.read(9)
            if not msg or len(msg) < 9:
                return None
            (sflag, cmd, timestamp, data, eflag) = struct.unpack('=ccIHc', msg)
            sflag, cmd, eflag = sflag.decode('ascii'), cmd.decode('ascii'), eflag.decode('ascii')
            if sflag == 'S' and eflag == 'E':
                return (cmd, timestamp, data)
        return None
