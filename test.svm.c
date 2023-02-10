int fib(int x)
{
	int l,r;

	if(x>2)
	{
		l=fib(x-2);
		r=fib(x-1);

		x=l+r;
	}

	return x;
}

int main(int a)
{
	int g;

	if(a>1)
	{
		g=fib(a);
	}
}
