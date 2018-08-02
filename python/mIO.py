import struct

class mIO:
	def __init__(self,name):
		self.fname = name

	def read(self,tag):
		with open(self.fname,'rb') as f:
			while True:
				bb = f.read(4)
				if not bb:
					break
				else:
					[n] = struct.unpack('i',bb)
				tag_r = struct.unpack('c'*n,f.read(n))
				(type_r,) = struct.unpack('c',f.read(1))
				[n] = struct.unpack('i',f.read(4))
				if type_r is 'c':
					var = struct.unpack('c'*n,f.read(n))
				elif type_r is 'i':
					var = struct.unpack('i'*n,f.read(4*n))
				elif type_r is 'd':
					var = struct.unpack('d'*n,f.read(8*n))
			
				if tag==''.join(tag_r):
					break
		return var

	def __str__(self):
		res = ''
		with open(self.fname,'rb') as f:
			while True:
				bb = f.read(4)
				if not bb:
					break
				else:
					[n] = struct.unpack('i',bb)
				tag_r = struct.unpack('c'*n,f.read(n))
				(type_r,) = struct.unpack('c',f.read(1))
				[n] = struct.unpack('i',f.read(4))
				if type_r is 'c':
					var = struct.unpack('c'*n,f.read(n))
				elif type_r is 'i':
					var = struct.unpack('i'*n,f.read(4*n))
				elif type_r is 'd':
					var = struct.unpack('d'*n,f.read(8*n))
				res += '\n' + ''.join(tag_r) + ' = '
				for v in var:
					res += ' ' + str(v) 
		return res

