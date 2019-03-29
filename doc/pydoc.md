# mIO documentation - Python

## Writing

Not supported. Currently writing in this 
format is supported in C and C++.

## Reading

The flexibility of python allows the user
to read the fields contained in a dat file
pretty easily

```python
from mIO import mIO

f = mIO("filename-here")
myvec = f.read("myvec")
```

When the class is initialized *little endianess*
is automatically assumed. If the dat file has
been written in big endianess the user must
specify it with

```python
f = mIO("filename-here",'>')
```

In general the function read will return a 
list containing every instance of the field
corresponding to the input tag. Each element
of the list will be a **tuple** whose size
depends on the size of the field

```python
vec = f.read("myvec")
print vec[0] # a tuple with the first instance of myvec
print vec[1] # a tuple with the second instance of myvec
```

Instead the function *get* returns always a **list**
with the content field read from the data file.
If we assume that the field *kappa* has a single 
element and it appears once, while the field
*corr* appears 4 times and has 8 elements
then we obtain

```python
k=f.get("kappa") # k is a float
c=f.get("corr") # is a list of lists
print c[0:4][0:8]
```

It is possible to write to std ouput the entire
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
file. Note that in this case all fields of the 
dictionary are lists of tuples.

```python
dic = f.load()
print dic["myvec"] # this prints a list of tuples
print dic["myvec"][0] # this print a tuple with the content
```

