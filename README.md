# mIO

A simple IO file format compatible with C++, Python based on tag recognition.

It allows the user to store data in binary format by attaching a short text (tag)
that supposedly identifies the data and simplifies the reading.

It is primarily a c++/python library self-contained in a single file. 
The documentation can be found here: [c++](./doc/cdoc.md) and [python](./doc/pydoc.md). 

Note that the c++ folder contains some useful additional tools to 
quickly inspect and read data files generated with mIO.
It requires to be compiled

```bash
cd c++
./compile.sh
```

----
Below is a quick example in c++

```c++
#include <stdlib.h>

#include "mIO.h"

int main()
{
   int n,b,i;
   double *vec;
   char fname[64];

   sprintf(fname,"test.dat");

   n=10;
   b=45;

   vec=new double[n];
   for (i=0;i<n;i++)
      vec[i] = 3.14 * i;

   mIO m(fname);
   m.write("variable1",&b,1);
   m.write("vector-length",&n,1);
   m.write("myvector",vec,n);


   int n2;
   double *vec2;
   mIO m2(fname);
   m2.read("vector-length",&n2);
   vec2 = new double[n2];
   m2.read("myvector",vec2);

   m2.print();

   return 0;
}
```
