
int main(int _)
{
  char  c;
  int   i;
  void *p;

  ccassert(sizeof c == 1)
  ccassert(sizeof i == 4)
  ccassert(sizeof p == 8)

  return 1;
}

