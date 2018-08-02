#include <stdlib.h>
#include <string>

#include "mIO.h"

int main()
{
   int n,b,i;
   double *vec;
   double f1[2], f2[2];
   char fname[64];
   std::string text, fname2;
   
   sprintf(fname,"../test.dat");

   n=10;
   b=45;

   vec=new double[n];
   for (i=0;i<n;i++)
      vec[i] = 3.14 * i;

   f1[0]=-1.2345;
   f1[1]=+3.4567;
   
   f2[0]=-5.6789;
   f2[1]=+10.1112;

   text="here-goes-some-text";

   fname2="../test.dat";
   mIO m(fname2);
   m.write("variable1",&b,1);
   m.write("vector-length",&n,1);
   m.write("mytext",text);
   m.write("myvector",vec,n);
   m.write("myfield",f1,2);
   m.write("myfield",f2,2);
   
   int n2;
   double *vec2;
   mIO m2(fname);
   m2.read("vector-length",&n2);
   vec2 = new double[n2];
   m2.read("myvector",vec2);

   m2.print();

   double r[2];
   m2.read("myfield",r,0);
   printf("first instance of myfield %f %f \n",r[0],r[1]);
   m2.read("myfield",r,1);
   printf("second instance of myfield %f %f \n",r[0],r[1]);
   m2.read("myfield",r,2);

   return 0;
}
