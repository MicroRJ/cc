int fib(int x)
{ if(x>=2)
	{ int l=fib(x-2);
		int r=fib(x-1);
		return l+r;
	}
	return x;
}

int main(int a)
{

	int g;
	g=fib(a);

	return g;
}
