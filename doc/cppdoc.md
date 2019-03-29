# mIO documentation - C++

The mIO c++ class is entirely 
contained in the header file 
*mIO.h*. Hence the user must simply
include it in the main program
using

```c++
#include "mIO.h"
``` 

The class can be initialized by
passing a string or char array

```c++
std::string fname="filename-here";
mIO f(fname);

char *fname="filename-here";
mIO f(fname);
```

## Writing 

Once the class is initialized the user
can write a char, int or double variable
by simply typing

```c++
char vc;
int vi;
double vd;

f.write("mychar",vc);
f.write("myint",vi);
f.write("mydouble",vd);
```

The key feature of this library is the 
possibility to use a tag to identify the
content the variable in a human readable
way.

The same tag can be used multiple times.

To write an array to disk it is enough
to additionally pass the length of the 
array

```c++
double vec[10];

f.write("mydoublevec",vec,10);
```

## Reading
Once the class is initialized the user
can read any variable or array
by passing the appropriate tag

```c++
double vd;
double vec[10];

f.read("mydouble",&vd);
f.read("mydoublevec",vec);
```

Note the length is not required because it is
stored in the data file. However the array
must be allocated with the right size. If the
size is not known a priori the user can 
get by typing

```c++
double *vec;
int s;

s = f.size("mydoublevec");
vec=new double[s];
f.read("mydoublevec",vec);
```


