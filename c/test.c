#include <stdlib.h>
#include <stdio.h>

#include "mIO.h"

static int n=10;
static int b;

int main()
{
   int i, n2;
   double vec[n];
   double *vec2;
   char fname[64];

   sprintf(fname,"test.dat");

   for (i=0;i<n;i++)
      vec[i] = 3.14 * i;

   b=45;

   mIO(fname);
   mIOwrite("variable1",'i',&b,1);
   mIOwrite("vector-length",'i',&n,1);
   mIOwrite("myvector",'d',vec,n);

   mIOprint();

   mIOread("vector-length",&n2,0);
   vec2 = malloc(sizeof(double)*n2);
   mIOread("myvector",vec2,0);

   printf("vector-length written %d read %d\n",n,n2);
   for (i=0;i<n;i++)
      printf("myvector %d (written-read) %f \n",i,vec[i]-vec2[i]);

   return 0;
}
