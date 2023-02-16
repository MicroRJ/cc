int assert(int is_true)
{
  if(is_true == 0)
  {
    ccerror();
  }
  return 0;
}

int retrsame(int x)
{
	return x;
}

int main(int a)
{

	int i=0;
	while(i<256)
	{
		assert(retrsame(i)==i);
		i=i+1;
	}

	return i;
}