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
	int f=fib(a);
	ccassert(f==17711);
	return f;
}
