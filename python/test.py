from mIO import mIO

f = mIO("../test.dat")
n = f.read("vector-length")
vec = f.read("myvector")
print 'reading ', n[0], vec[0]

text = f.read("mytext");
print 'reading ', text

print f.read("not-this-field")

print 'and printing '

print f

print f.fields()
