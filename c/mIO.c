#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mIO.h"

#define TAG_LEN 128

static char fname[1024]; 
static size_t writable, bigend;

static struct {
   long int pos;
   int n;
   char type;
} field;


static size_t get_bytes(char t)
{
   if (t=='c')
      return sizeof(char);
   else if (t=='i')
      return sizeof(int);
   else if (t=='d')
      return sizeof(double);
   return 0;
}


static int wrong_type(char t)
{
   if (t=='c')
      return 0;
   else if (t=='i')
      return 0;
   else if (t=='d')
      return 0;
   return 1;
}


static size_t isBigEndian()
{
   int num=1;
   if (*(char*)&num==1)
      return 0;
   else
      return 1;
}


static void bswap(size_t n,size_t b,const void *x,void *y)
{
   unsigned char *xx,*yy;
   int i,j;

   xx=(unsigned char *)x;
   yy=(unsigned char *)y;
   for (i=0;i<n;i++)
      for (j=0;j<b;j++)
         yy[i*b + j]=xx[i*b + b-1-j];
}


static int safe_fwrite(const void *x,size_t s,size_t n,FILE *f)
{
   int ic;
   if (!bigend)
      ic = fwrite(x, s, n, f);
   else
   {
      void *ptr = malloc(s*n);
      bswap(n,s,x,ptr);
      ic = fwrite(ptr, s, n, f);
   }
   return ic;
}


static int safe_fread(void *x,size_t s,size_t n,FILE *f)
{
   int ic;
   if (!bigend)
      ic = fread(x, s, n, f);
   else
   {
      void *ptr = malloc(s*n);
      ic = fread(ptr, s, n, f);
      bswap(n,s,ptr,x);
   }
   return ic;
}


void mIO(const char *f)
{
   FILE *ff;
   writable=0;
   ff=fopen(f,"rb");
   if (ff==NULL)
      writable=1;
   else
      fclose(ff);
   memcpy(fname,f,strlen(f));
   bigend=isBigEndian();
}


static void find(const char *varname,int id)
{
   FILE *f;
   int ic, count;
   size_t bytes;
   char tag[TAG_LEN];
   long int end;
   unsigned int len;

   f=fopen(fname,"rb");
   fseek(f,0,SEEK_END);
   end=ftell(f);
   fseek(f,0,SEEK_SET);

   count=0;
   while (ftell(f)<end)
   {
      memset(tag,0,TAG_LEN);

      ic  = safe_fread(&len, sizeof(int), 1, f);
      ic += safe_fread(tag, sizeof(char), len, f);
      ic += safe_fread(&field.type, sizeof(char), 1, f);
      ic += safe_fread(&field.n, sizeof(int), 1, f);
      if (ic!=(3+len))
      {
         printf("Reading error \n");
         exit(2);
      }

      bytes = get_bytes(field.type);

      if (strcmp(varname, tag)==0)
      {
         if (count==id)
         {
            field.pos = ftell(f);
            count++;
            break;
         }
         else
         {
            fseek(f,field.n*bytes,SEEK_CUR);
            count++;
         }
      }
      else
         fseek(f,field.n*bytes,SEEK_CUR);
   }

   if (count==0)
      printf("Error: field %s not found\n",varname);

   if (id>count-1)
      printf("Error: %d instance of field %s not found [max = %d] \n",
            id,varname,count);

   fclose(f);
}


void mIOread(const char *varname,void *var,int id)
{
   FILE *f;
   size_t bytes;

   find(varname,id);

   f=fopen(fname,"rb");
   fseek(f,field.pos,SEEK_SET);

   bytes = get_bytes(field.type);
   if (safe_fread(var, bytes, field.n, f) != field.n)
   {
      printf("Reading error \n");
      exit(2);
   }
   fclose(f);
}


int mIOsize(const char *varname,int id)
{
   find(varname,id);
   return field.n;
}


void mIOwrite(const char *varname,const char type,void *var,int n)
{
   FILE *f;
   int ic;
   size_t len,bytes;
    
   if (!writable)
   {
      printf("File %s already exists\n",fname);
      exit(1);
   }

   if (wrong_type(type))
   {
      printf("Unexpected type %c \n",type);
      exit(1);
   }

   len = strlen(varname); 

   if (len>TAG_LEN)
   {
      printf("variables with names longer than %d are not supported \n",
            TAG_LEN);
      exit(11);
   }

   f=fopen(fname,"ab");

   ic  = safe_fwrite(&len, sizeof(int), 1, f);
   ic += safe_fwrite(varname, sizeof(char), len, f);
   ic += safe_fwrite(&type, sizeof(char), 1, f);
   ic += safe_fwrite(&n, sizeof(int), 1, f);

   bytes = get_bytes(type);
   ic += safe_fwrite(var, bytes, n, f);

   if (ic!=(3+len+n))
   {
      printf("Write error \n");
      exit(2);
   }

   fclose(f);
}


void mIOprint(void)
{
   FILE *f;
   int ic;
   size_t bytes;
   int len, n;
   char type, tag[TAG_LEN];
   char line[4096];

   f=fopen(fname,"rb");
   ic = fread(&len, sizeof(int), 1, f);

   while (!feof(f))
   {
      memset(tag,0,TAG_LEN);

      ic += fread(tag, sizeof(char), len, f);
      ic += fread(&type, sizeof(char), 1, f);
      ic += fread(&n, sizeof(int), 1, f);

      bytes = get_bytes(type);

      sprintf(line,"%s = ",tag);
      if (type=='c')
      {
         char var[n];
         ic += fread(var, bytes, n, f);
         sprintf(line,"%s %s",line,var);
      }
      else if (type=='i')
      {
         int var;
         for (int i=0;i<n;i++)
         {
            ic += fread(&var, bytes, 1, f);
            sprintf(line,"%s %d",line,var);
         }
      }
      else if (type=='d')
      {
         double var;
         for (int i=0;i<n;i++)
         {
            ic += fread(&var, bytes, 1, f);
            sprintf(line,"%s %f",line,var);
         }
      }
      printf("%s \n",line); 

      if (ic!=(3+len+n))
      {
         printf("Reading error \n");
         exit(2);
      }
	 
      ic = fread(&len, sizeof(int), 1, f);
   }

   fclose(f);
}


