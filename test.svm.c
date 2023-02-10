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
	int some[100];
	some[24]=a;

	int h;
	h=some[24];

	int g=h;
	g=fib(g);
	return g;
}
