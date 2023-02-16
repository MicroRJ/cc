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
	int f=fib(a);
	assert(f==17711);
	return f;
}
```


### State of the major files:
- lexer/parser 'ccread.c' is straightforward and simple so you can't mess that up too much,
  unless you're me, so there's some work to be done ...
- checker 'ccseek.c' aka the thing that's supposed to tell you what type or declaration an expression is referring to and
  some other things, a) it doesn't know what it's doing and b) it's missing a bunch of requirements to be an actual checker ...
  This is mostly due to the endless amount of time spent lollygagging in the endless process of
  finding a "substantial" word with the same number of letters as other words so they line up perfectly and
  look pretty to then name a worthless file ...
- generator 'ccemit.c' is somewhat of a philosophical mess, but then again, isn't everything I do a mess ...
- virtual-machine 'ccexec.c' is in the same state as me when I wake up after 3 hours of sleep a sharp pain in the neck
  and a sore throat ...

#### Todo list:
- Lexer: Hello?? did you forget comments?
- Better logging and error reporting, this includes a proper 'ccprintf' ...
- Real tree checker system
- Real type system, not just integers and array modifiers
- Faster parser, remove the enormous amount of calls for each expression
- Support for strings and pointers
- Support for pp-directives, '#include', '#if' ...
- Start thinking about using an SSA model.
- Look into faster VM implementations, and possibly generate some machine code.
- Support for initializer lists
- Support for several built-ins especially 'sizeof'
