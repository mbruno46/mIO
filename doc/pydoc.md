# mIO documentation - Python

## Reading

The flexibility of python allows the user
to read the fiels contained in a dat file
pretty easily

```python
from mIO import mIO

f = mIO("filename-here")
myvec = f.read("myvec")
```

When the class is initialized little endianess
is automatically assumed. If the dat file has
been written in big endianess the user can
specify it with

```python
f = mIO("filename-here",'>')
```

In general the function read will return a 
list containing every instance of the field
corresponding to the input tag. Each element
of the list will be a tuple whose size
depends on the size of the field

```python
vec = f.read("myvec")
print vec[0] 
print vec[1] 
```

It is possible to write to ouput the entire
content of the dat file by using the built-in
function print

```python
f = mIO("filename-here")
print f
```

It is possible to inspect the content of the dat
file, for example by printing all the tags

```python
print f.fields()
```

Alternatively it is also possible to load
the entire dat file in memory. A dictionary
is created with the tags provided in the file
and a list is attached to each field depending
on how many times the same field appears in the
file

```python
dic = f.load()
print dic["myvec"] # this prints a list of tuples
print dic["myvec"][0] # this print a tuple with the content
```

