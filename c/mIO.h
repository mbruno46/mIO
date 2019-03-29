#ifndef MIO_H
#define MIO_H

extern void mIO(const char *f);
extern void mIOwrite(const char *varname,const char type,void *var,int n);
extern void mIOprint(void);
extern void mIOread(const char *varname,void *var,int id);
extern int mIOsize(const char *varname,int id);

#endif
