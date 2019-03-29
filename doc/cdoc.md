# mIO documentation - C

The c implementation requires the 
compilation of the library before 
using it, unless the user copies
*mIO.h* and *mIO.c* in the same
directory of the main program.
The script *compile.sh* automatically
creates a static library that 
can be easily integrated in a program
by adding the following flags to a 
*gcc* compiler for example

```bash
MIO_PATH=/path/to/mIO
gcc main.c -o main.x -I$MIO_PATH -L$MIO_PATH -lmIO
```

and by including the header in the 
main program
```c
#include "mIO.h"
```

The module is initialized by
passing a char array to the 
function *mIO*

```c
char *fname="filename-here";
mIO(fname);
```

## Writing

Once the module is initialized the user
can write a char, int or double variable
by simply typing

```c
char vc;
int vi;
double vd;

mIOwrite("mychar",'c',&vc,1);
mIOwrite("myint",'i',&vi,1);
mIOwrite("mydouble",'d',&vd,1);
```

List of arguments of *mIOwrite*:

1. a char array (or string) with  the 
name or tag of the field

2. a single char chosen among the 3 
given above, to specify the type of
the field

3. a pointer to the field

4. the number of elements of the field

The same tag can be used multiple times.
Currently the c implementation does not 
support the writing of **simultaneous**
files in the same program, like the c++
version [c++](./doc/cppdoc.md).

## Reading

Once the module is initialized the user
can read any variable or array
by passing the appropriate tag

```c
double vd;
double vec[10];

mIOread("mydouble",'d',&vd,1);
mIOread("mydoublevec",'d',vec,10);
```

Compared to the [c++](./cppdoc.md) 
compilation, 
the user must specify both type and 
number of elements to be read in.
If the size is not known a priori
the user can get it by typing

```c
mIOsize("mydoublevec",'d');
```



