int fib(int e)
{
  if(e>=2)
  { int l=fib(e-2);
    int r=fib(e-1);
    return l+r;
  }
  return e;
}

int set_to_one(int *j)
{
  j[0]=1;

  return 1;
}

int main(int _)
{
  struct ab_t
  {
  	struct a_t
  	{ int a;
  	} a;

    int b;
  } v;

  int i,x,y,z,*p,q;
  char *o,*u;
  int a[24];
  int b[24][24];
  int c[24][24][24];

  v.a.a=22;
  ccassert(v.a.a==22);

  v.b=22;
  ccassert(v.b==22);

  ccassert(sizeof i == 4);
  ccassert(sizeof p == 8);

  z=777;

  p=&z;
  z=*p;
  ccassert(z==777);

  *p=1;
  ccassert(z==1);


  p=&x; ccassert(p==&x);
  p=&y; ccassert(p==&y);
  p=&z; ccassert(p==&z);

  p[0]=1;
  ccassert(z==1);

  z=*p;
  ccassert(z==1);

  set_to_one(&i);
  ccassert(i==1);

  char *mem;
  mem=ccmalloc(24);

  mem[0]=97;
  mem[1]=98;
  mem[2]=99;

  ccprintf("Hello, %s %i, %i\n","Sailor!",3,mem[0]);

  void *file;
  file=ccopenfile("code\\hello.txt","w");

  ccpushfile(file,0,3,mem);

  ccbreak();
  ccerror();
  ccassert(1);

  o="abc";
  ccassert(o[0]==97);
  ccassert(o[1]==98);
  ccassert(o[2]==99);

  i=0;
  i=7;

  ccassert(i==7);

  i=0;
  x=0;
  while(x<24)
  {
    x=x+1;
    i=i+1;
  }

  i=0;
  x=0;
  while(x<24)
  { y=0;
    while(y<24)
    { z=0;
      while(z<24)
      {
        c[x][y][z]=i;
        q=c[x][y][z];
        ccassert(q==i);

        i=i+1;

        z=z+1;
      }
      ccassert(z==24);
      y=y+1;
    }
    ccassert(y==24);
    x=x+1;
  }
  ccassert(x==24);


  i=fib(22);
  ccassert(i==17711);

  return i;
}