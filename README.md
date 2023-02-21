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


### State of the major files:
- lexer/parser 'ccread.c' there's some work to be done ...
- checker 'ccseer.c', clueless and it's missing a bunch of requirements to be an actual checker ...
  This is mostly due to the endless amount of time spent lollygagging ...
- generator 'ccemit.c', a philosophical mess ...
- virtual-machine 'ccexec.c', a paradox ...

#### Todo list:
- Overall refactor ...
- Proper entity stage to go from AST to the emitting stage...
- Lexer: Missing comments, integral and floating point suffixes...
- Better logging and error reporting, attaching file location metadata to trees ...
- Faster parser ...
- Support for pp-directives, '#include', '#if' ...
- SSA.
- Faster VM, possibly generate some machine code.
- Support for initializer lists
- Support for several built-ins
