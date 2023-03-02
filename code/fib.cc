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
  int i=fib(22);
  ccassert(i==17711);
  return i;
}