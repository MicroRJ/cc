int fib(int e)
{
  if(e>=2)
  { int l=fib(e-2);
    int r=fib(e-1);
    return l+r;
  }
  return e;
}

int main(int _)
{
	ccprintf("Hello, Sailor!\n");

  int i,x,y,z,*p,q;
  char *o,*u;
  int a[24];
  int b[24][24];
  int c[24][24][24];


  ccbreak();
  ccerror();
  ccassert(1);

	o="abc";
	ccassert(o[0]==97);
	ccassert(o[1]==98);
  ccassert(o[2]==99);

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