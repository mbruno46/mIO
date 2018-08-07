import struct

class mIO:
    def __init__(self,name,endian='<'):
        self.fname = name
        self.endian = endian

    def __reader(self,type_r,n,f):
        if type_r is 'c':
            var = str(f.read(n))
        elif type_r is 'i':
            var = struct.unpack(self.endian+'i'*n,f.read(4*n))
        elif type_r is 'd':
            var = struct.unpack(self.endian+'d'*n,f.read(8*n))
        return var

    def __seeker(self,type_r,n,f):
        if type_r is 'c':
            f.seek(n,1)
        elif type_r is 'i':
            f.seek(4*n,1)
        elif type_r is 'd':
            f.seek(8*n,1)

    def read(self,tag,idx=None):
        res = []
        count = 0
        with open(self.fname,'rb') as f:
            while True:
                bb = f.read(4)
                if not bb:
                    break
                else:
                    [n] = struct.unpack(self.endian+'i',bb)
                tag_r = struct.unpack(self.endian+'c'*n,f.read(n))
                (type_r,) = struct.unpack(self.endian+'c',f.read(1))
                [n] = struct.unpack(self.endian+'i',f.read(4))
                
                if tag==''.join(tag_r):
                    if idx==None:
                        var = self.__reader(type_r,n,f)
                        res.append( var )
                    else:
                        if count==idx:
                            res = self.__reader(type_r,n,f)
                            break
                    count += 1
                else:
                    self.__seeker(type_r,n,f)
        return res

    def load(self):
        res = {}
        ff = self.fields()
        for fff in ff:
            res[fff] = []

        with open(self.fname,'rb') as f:
            while True:
                bb = f.read(4)
                if not bb:
                    break
                else:
                    [n] = struct.unpack(self.endian+'i',bb)
                tag_r = struct.unpack(self.endian+'c'*n,f.read(n))
                (type_r,) = struct.unpack(self.endian+'c',f.read(1))
                [n] = struct.unpack(self.endian+'i',f.read(4))
                
                var = self.__reader(type_r,n,f)
                res[''.join(tag_r)].append( var )
        return res
    
    def __str__(self):
        res = ''
        with open(self.fname,'rb') as f:
            while True:
                bb = f.read(4)
                if not bb:
                    break
                else:
                    [n] = struct.unpack(self.endian+'i',bb)
                tag_r = struct.unpack(self.endian+'c'*n,f.read(n))
                (type_r,) = struct.unpack(self.endian+'c',f.read(1))
                [n] = struct.unpack(self.endian+'i',f.read(4))
                res += '\n' + ''.join(tag_r) + ' = '
                if type_r is 'c':
                    res += ' ' + str(f.read(n))
                else:
                    if type_r is 'i':
                        var = struct.unpack(self.endian+'i'*n,f.read(4*n))
                    elif type_r is 'd':
                        var = struct.unpack(self.endian+'d'*n,f.read(8*n))
                    for v in var:
                        res += ' ' + str(v) 
        return res

    def fields(self):
        res = []
        with open(self.fname,'rb') as f:
            while True:
                bb = f.read(4)
                if not bb:
                    break
                else:
                    [n] = struct.unpack(self.endian+'i',bb)
                tag_r = struct.unpack(self.endian+'c'*n,f.read(n))
                (type_r,) = struct.unpack(self.endian+'c',f.read(1))
                [n] = struct.unpack(self.endian+'i',f.read(4))
                res.append(''.join(tag_r))
                self.__seeker(type_r,n,f)
        return res
    
    def howmany(self,tag):
        count = 0
        with open(self.fname,'rb') as f:
            while True:
                bb = f.read(4)
                if not bb:
                    break
                else:
                    [n] = struct.unpack(self.endian+'i',bb)
                tag_r = struct.unpack(self.endian+'c'*n,f.read(n))
                (type_r,) = struct.unpack(self.endian+'c',f.read(1))
                [n] = struct.unpack(self.endian+'i',f.read(4))
                self.__seeker(type_r,n,f)

                if tag==''.join(tag_r):
                    count+=1
        return count
