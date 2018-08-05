#include <stdlib.h>

#include "mIO.h"

int main(int argc,char *argv[])
{
   char type;
   int n, id;

   if (argc<3)
   {
      printf("[Usage]: ./read-fields file.dat field [skip]\n");
      exit(1);
   }

   mIO f(argv[1]);
   id=0;
   if (argc==4)
      id=atoi(argv[3]);
   f.print(argv[2],id);   
}
