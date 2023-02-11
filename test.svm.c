int fib(int x)
{ if(x>=2)
	{ int l=fib(x-2);
		int r=fib(x-1);
		return l+r;
	}
	return x;
}

int retsame(int c)
{
	return c;
}

int main(int a)
{
	int some[256];

	int i=0;
	while(i<256)
	{ some[i]=i;
		i=i+1;
	}

	some[24]=retsame(fib(a));
	return some[2];
}
