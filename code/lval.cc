int fib(int e)
{

  if(e>=2)
  { int l=fib(e-2);
    int r=fib(e-1);
    return l+r;
  }
  return e;
}

int same(int t)
{
  return t;
}

int main(int _)
{


  ccbreak();
  ccerror();
  ccassert(1);

  int i,x,y,z,*p,q;

  char s;


  int a[24];
  int b[24][24];
  int c[24][24][24];

  a[0]=22;
  b[0][0]=22;
  c[0][0][0]=22;

  ccassert(a[0]==22);
  ccassert(b[0][0]==22);
  ccassert(c[0][0][0]==22);

  i=0,x=0,y=0,z=0;
  ccassert(i==0);
  ccassert(x==0);
  ccassert(y==0);
  ccassert(z==0);

  p=&x; ccassert(p==&x);
  p=&y; ccassert(p==&y);

  a[0]=1;
  ccassert(a[0]==1);

  c[0][1][1]=2;

  while(x<24)
  {
    while(y<24)
    {
      while(z<24)
      {

        c[x][y][z]=i;

        q=c[x][y][z];

        ccassert(q==i);

        i=i+1;

        z=z+1;
      }
      y=y+1;
    }
    x=x+1;
  }

  i=fib(22);
  ccassert(i==17711);


  return i;
}