#ifndef MIO_H
#define MIO_H

#include <stdlib.h>
#include <stdio.h>
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

   public:

      mIO(std::string &f)
      {
	 FILE *ff;
	 writable=false;
	 ff=fopen((char*)f.c_str(),"rb");
	 if (ff==NULL)
	    writable=true;
	 fclose(ff);
	 fname = (char*)malloc(f.size());
	 memcpy(fname,(char*)f.c_str(),f.size());
      }

      mIO(const char *f)
      {
	 FILE *ff;
	 writable=false;
	 ff=fopen(f,"rb");
	 if (ff==NULL)
	    writable=true;
	 fclose(ff);
	 fname = (char*)malloc(strlen(f));
	 memcpy(fname,f,strlen(f));
      }

      ~mIO(void)
      {
	 free(fname);
      }

      template <class T> void read(const char *varname,T *var)
      {
	 read(varname,var,0);
      }

      template <class T> void read(const char *varname,T *var,int id)
      {
	 FILE *f;
	 int ic, n, len, count;
	 size_t bytes;
	 char type, tag[TAG_LEN];

	 f=fopen(fname,"rb");
	 ic = fread(&len, sizeof(int), 1, f);
	 
	 count=0;
	 while (!feof(f))
	 {
	    memset(tag,0,TAG_LEN);

	    ic += fread(tag, sizeof(char), len, f);
	    ic += fread(&type, sizeof(char), 1, f);
	    ic += fread(&n, sizeof(int), 1, f);

	    bytes = get_bytes(type);

	    if (strcmp(varname, tag)==0)
	    {
	       if (count==id)
	       {
		  ic += fread(var, bytes, n, f);
	   	  if (ic!=(3+len+n))
	      	  {
	   	     printf("Reading error \n");
	   	     exit(2);
	      	  }
	       }
	       else
		  fseek(f,n*bytes,SEEK_CUR);
	       count++;
	    }
	    else
	       fseek(f,n*bytes,SEEK_CUR);

	    ic = fread(&len, sizeof(int), 1, f);
	 }

	 if (count==0)
	    printf("Error: field %s not found\n",varname);

	 if (id>count-1)
	    printf("Error: %d instance of field %s not found [max = %d] \n",id,varname,count);

	 fclose(f);
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
	
	 ic  = fwrite(&len, sizeof(int), 1, f);
      	 ic += fwrite(varname, sizeof(char), len, f);
	 ic += fwrite(&type, sizeof(char), 1, f);
	 ic += fwrite(&n, sizeof(int), 1, f);

	 bytes = get_bytes(type);
	 ic += fwrite(var, bytes, n, f);

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
	 char line[1024];

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
	 char line[1024];

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
	 int ic, n, count;
	 size_t bytes, len;
	 char type, tag[TAG_LEN];
	 char line[1024];

	 f=fopen(fname,"rb");
	 ic = fread(&len, sizeof(int), 1, f);
	 
	 while (!feof(f))
	 {
	    memset(tag,0,TAG_LEN);

	    ic += fread(tag, sizeof(char), len, f);
	    ic += fread(&type, sizeof(char), 1, f);
	    ic += fread(&n, sizeof(int), 1, f);

	    bytes = get_bytes(type);

	    if (strcmp(varname, tag)==0)
	    {
	       if (count==id)
	       {
		  sprintf(line,"# %s \n",tag);
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
	       } 
	       else
		  fseek(f,n*bytes,SEEK_CUR);
	       count++;
	    }
	    else
	       fseek(f,n*bytes,SEEK_CUR);
	 
	    ic = fread(&len, sizeof(int), 1, f);
	 }

	 fclose(f);
      }
};

#endif
