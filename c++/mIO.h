#ifndef MIO_H
#define MIO_H

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <typeinfo>

#define TAG_LEN 128

#define mIO_read(m,X) m.read(#X,X)
#define mIO_write(m,X,n) m.write(#X,X,n)

class mIO
{
   private:
      char *fname;
      bool writable;
      bool bigend;

      struct {
	 long int pos;
	 int n;
	 char type;
      } field;

      size_t get_bytes(char t)
      {
	 if (t=='c')
	    return sizeof(char);
	 else if (t=='i')
	    return sizeof(int);
	 else if (t=='d')
	    return sizeof(double);
	 return 0;
      }

      char get_type(char &v) {return 'c';}
      char get_type(char *v) {return 'c';}
      char get_type(int &v) {return 'i';}
      char get_type(int *v) {return 'i';}
      char get_type(double &v) {return 'd';}
      char get_type(double *v) {return 'd';}

      bool isBigEndian()
      {
	 int num=1;
	 if (*(char*)&num==1)
	    return false;
	 else
	    return true;
      }

      void bswap(size_t n,size_t b,const void *x,void *y)
      {
	 unsigned char *xx,*yy;
	 int i,j;

	 xx=(unsigned char *)x;
	 yy=(unsigned char *)y;
	 for (i=0;i<n;i++)
	    for (j=0;j<b;j++)
	       yy[i*b + j]=xx[i*b + b-1-j];
      }

      int safe_fwrite(const void *x,size_t s,size_t n,FILE *f)
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
      
      int safe_fread(void *x,size_t s,size_t n,FILE *f)
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

   public:

      mIO(const char *f)
      {
	 FILE *ff;
	 writable=false;
	 ff=fopen(f,"rb");
	 if (ff==NULL)
	    writable=true;
	 else
	    fclose(ff);
	 fname = (char*)malloc(strlen(f));
	 memcpy(fname,f,strlen(f));
	 bigend=isBigEndian();
      }
      
      mIO(std::string f)
      {
	 FILE *ff;
	 writable=false;
	 f.push_back('\0');
	 ff=fopen((char*)f.c_str(),"rb");
	 if (ff==NULL)
	    writable=true;
	 else
	    fclose(ff);
	 fname = (char*)malloc(f.size());
	 memcpy(fname,(char*)f.c_str(),f.size());
	 bigend=isBigEndian();
      }
      
      ~mIO(void)
      {
	 free(fname);
      }
      
      void find(const char *varname,int id)
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
		  break;
	       }
	       else
		  fseek(f,field.n*bytes,SEEK_CUR);
	       count++;
	    }
	    else
	       fseek(f,field.n*bytes,SEEK_CUR);
	 }

	 if (count==0)
	    printf("Error: field %s not found\n",varname);

	 if (id>count-1)
	    printf("Error: %d instance of field %s not found [max = %d] \n",id,varname,count);

	 fclose(f);
      }

      void read(const char *varname,void *var)
      {
	 read(varname,var,0,false);
      }

      void read(const char *varname,void *var,int id)
      {
	 read(varname,var,id,false);
      }

      void read(const char *varname,void *var,int id,bool dryrun)
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
      
      int size(const char *varname,int id)
      {
	 find(varname,id);
	 return field.n;
      }

      int size(const char *varname)
      {
	 find(varname,0);
	 return field.n;
      }

      template <class T> void write(const char *varname,T *var,int n)
      {
	 FILE *f;
	 int ic;
	 size_t len,bytes;
	 char type;
	
	 if (!writable)
	 {
	    printf("File %s already exists\n",fname);
	    exit(1);
	 }

	 type = get_type(var);
	 len = strlen(varname); 

	 if (len>TAG_LEN)
	 {
	    printf("variables with names longer than %d are not supported \n",TAG_LEN);
	    exit(11);
	 }

	 f=fopen(fname,"ab");
	
	 ic  = safe_fwrite(&len, sizeof(int), 1, f);
      	 ic += safe_fwrite(varname, sizeof(char), len, f);
	 ic += safe_fwrite(&type, sizeof(char), 1, f);
	 ic += safe_fwrite(&n, sizeof(int), 1, f);

	 bytes = get_bytes(type);
	 ic += safe_fwrite(var, bytes, n, f);

	 //printf("Written variable %s with %d elements of bytes %lu \n",varname,n,bytes);

	 if (ic!=(3+len+n))
	 {
      	    printf("Write error \n");
	    exit(2);
	 }

	 fclose(f);
      }

      template <class T> void write(const char *varname,T var)
      {
	 T tmp=var;
	 write(varname,&tmp,1);
      }

      void write(const char *varname, std::string &var)
      {
	 std::string tmp(var);
	 tmp.push_back('\0');
	 write(varname,(char*)tmp.c_str(),tmp.size());
      }

      void print_tags(void)
      {
	 FILE *f;
	 int ic, n;
	 size_t bytes, len;
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

	    sprintf(line," - %s [%d %c]",tag,n,type);
	    ic += fseek(f, bytes*n, SEEK_CUR);
	    printf("%s \n",line); 

	    if (ic!=(3+len))
	    {
	       printf("Reading error \n");
	       exit(2);
	    }
	 
	    ic = fread(&len, sizeof(int), 1, f);
	 }

	 fclose(f);
      }

      void print(void)
      {
	 FILE *f;
	 int ic;
	 size_t bytes, len, n;
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

      void print(const char *varname,int id)
      {
	 FILE *f;
	 int ic, n;
	 size_t bytes;
	 char line[4096];

	 find(varname,id);

	 f=fopen(fname,"rb");
	 fseek(f,field.pos,SEEK_SET);
	 bytes = get_bytes(field.type);

	 sprintf(line,"# %s \n",varname);
	 ic=0;
	 if (field.type=='c')
	 {
	    char var[field.n];
	    ic = safe_fread(var, bytes, field.n, f);
	    sprintf(line,"%s %s",line,var);
	 }
	 else if (field.type=='i')
	 {
	    int var[field.n];
	    ic = safe_fread(var, bytes, field.n, f);
	    for (int i=0;i<field.n;i++)
	       sprintf(line,"%s %d",line,var[i]);
	 }
	 else if (field.type=='d')
	 {
	    double var[field.n];
	    ic = safe_fread(var, bytes, field.n, f);
	    for (int i=0;i<field.n;i++)
	       sprintf(line,"%s %e",line,var[i]);
	 }
	 printf("%s \n",line); 

	 if (ic!=(field.n))
	 {
	    printf("Reading error \n");
	    exit(2);
	 }
	 fclose(f);
      }
};

#endif
