
int
xorshift32(int *p)
{
  int q = *p;
  q = q ^ q << 13;
  q = q ^ q >> 17;
  q = q ^ q << 5;
  *p=q;
  return q;
}


int randi(int *randi_s, int randi_l)
{
  int randi_d,randi_v;

  randi_d=xorshift32(randi_s);

  randi_v=randi_d%randi_l;

  return randi_v;
}

int main(int _)
{
  char array[4][24];

  int u,k;
  u=sizeof array / sizeof array[0];
  k=sizeof array[0] / sizeof array[0][0];
  ccassert(u==4);
  ccassert(k==24);

  int x;
  x=38747;

  char *h;
  h="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  for(int j=0; j<u; j=j+1)
  { for(int i=0; i<k-1; i=i+1)
    {
      int v=randi(&x,64);
      array[j][i]=h[v];
    }
    array[j][i]=0;

    char *s;
    s=&array[j];
    ccprintf("array contents[%i]: %s\n",j,s);
  }

}