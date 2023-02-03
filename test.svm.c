int a;
int b;
int s;

int fib(int s)
{ if(s < 2)
	{
		return s;
	} else
	{
		int l=fib(s-1);
		int r=fib(s-2);
		return l+r;
	}
}

int main()
{ if(b==0) b=1;

	while(b<5) b=b+1;

	fib(45);
}