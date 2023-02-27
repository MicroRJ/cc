
int main(int _)
{
  char  c;
  int   i;
  void *p;

  ccassert(sizeof c == 1)
  ccassert(sizeof i == 4)
  ccassert(sizeof p == 8)

  int *u[7];
  int *v[7][7];

  ccassert(sizeof u[0] == 8)
  ccassert(sizeof v[0][0] == 8)

  ccassert(sizeof u == 7 * sizeof u[0])
  ccassert(sizeof v[0] == 7 * sizeof v[0][0])

  int x[7];
  int y[7][7];
  int z[7][7][7];

  ccassert(sizeof x[0] == 4)
  ccassert(sizeof y[0][0] == 4)
  ccassert(sizeof z[0][0][0] == 4)

  ccassert(sizeof x     == 7*4)
  ccassert(sizeof x[0]  == 4)

  ccassert(sizeof y     == 7*7*4)
  ccassert(sizeof y[0]  == 7*4)

  ccassert(sizeof z     == 7*7*7*4)
  ccassert(sizeof z[0]  == 7*7*4)


  return 1;
}

