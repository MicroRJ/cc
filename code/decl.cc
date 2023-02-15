
int assert(int is_true)
{
  if(is_true == 0)
  {
    ccerror();
  }

  return 0;
}


int main(int aa)
{
  int a,b,c,d,e;
  a=0,b=1,c=2,d=3,e=4;

  assert(a==0);
  assert(b==1);
  assert(c==2);
  assert(d==3);
  assert(e==4);


  return 1;
}