from mIO import mIO

f = mIO("../test.dat")
n = f.read("vector-length")
vec = f.read("myvector")
print 'reading ', n, vec

print 'and printing '

print f
