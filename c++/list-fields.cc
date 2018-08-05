#include <stdlib.h>

#include "mIO.h"

int main(int argc,char *argv[])
{
   printf("[Usage]: ./list-fields file.dat\n");
   if (argc!=2)
      exit(1);

   mIO f(argv[1]);
   f.print_tags();

   return 0;
}
