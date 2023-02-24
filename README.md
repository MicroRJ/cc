### CC
At the moment, 'CC' is an attempt at running "C" *albeit feature-less, resemblant* on a virtual machine, and in the process figure
out how to create a somewhat scalable model to then add the features I want, ultimately, generate some machine code ...

### The Name:
I wanted 'CC' to stand for something like 'c-cures' in the spirit of minimally but meaningfully extending or complementing 'C'
in a few key areas, but due to its current state and the fact that it doesn't even generate actual 'C', 'cannot-c' sounds like a much better name or maybe 'corroded-c' or 'crappy-c' ...

### What works:
Some things? Very few things have been implemented, and not even proper scoping has been added yet, so you can only use a name for a declarator once in the entire file, the code that I've managed to run is under 'code/'.

``` C
int assert(int is_true)
{
  if(is_true==0)
    ccerror();

  return 0;
}
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
  int f=fib(22);
  assert(f==17711);
  return f;
}
```

#### Todo list:
- Improve logging, rich locations...
- Improve the debug system...
- Keep improving the checker stage, scopes?...
- More advanced type system...
- Missing comments, integral and floating point suffixes...
- Faster parser ...
- Support for pp-directives, '#include', '#if' ...
- SSA.
- Faster VM, possibly generate some machine code or target another VM.
- Support for initializer lists
- Support for 'cctypeof'
