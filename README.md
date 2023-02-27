### CC
At the moment, 'CC' is an attempt at running "C" *albeit feature-less, resemblant* on a virtual machine, and in the process figure
out how to create a somewhat scalable model to then add the features I want, ultimately, generate some machine code ...

### The Name:
I wanted 'CC' to stand for something like 'c-cures' in the spirit of minimally but meaningfully extending or complementing 'C'
in a few key areas, but due to its current state and the fact that it doesn't even generate actual 'C', 'cannot-c' sounds like a much better name or maybe 'corroded-c' or 'crappy-c' ...

### What works:
Some things? Very few things have been implemented, and not even proper scoping has been added yet, so you can only use a name for a declarator once in the entire file, the code that I've managed to run is under 'code/'.

``` C
...
	char *mem;
  mem=ccmalloc(24);

  mem[0]=97;
  mem[1]=98;
  mem[2]=99;

  ccprintf("Hello, %s %i, %i\n","Sailor!",3,mem[0]);

  void *file;
  file=ccopenfile("code\\hello.txt","w");

  ccpushfile(file,0,3,mem);

  ccbreak();
  ccerror();
  ccassert(1);

  o="abc";
  ccassert(o[0]==97);
  ccassert(o[1]==98);
  ccassert(o[2]==99);
...
```

#### Todo list:
- Improve logging, rich locations...
- Improve the debug system...
- Keep improving the checker stage ...
- More advanced type system ...
- Missing comments, integral and floating point suffixes...
- Faster parser ...
- Support for pp-directives, '#include', '#if' ...
- SSA.
- Faster VM, possibly generate some machine code or target another VM.
- Support for initializer lists
- Support for 'cctypeof'
