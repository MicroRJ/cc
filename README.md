### CC
Right now 'CC' is simply an attempt at running "C" on a virtual machine and in the process figuring out how to get
a good, basic architecture ready to start adding the features I actually want.
Hopefully if I ever get to it, actually generate some machine code or figure out what to do next ...

Very few things have actually been implemented, not even proper scopes have been implemented yet.
The code that you can run is under 'code/'

#### Todo list:
 - Hello?? did you forget comments?
 - Better logging and error reporting ...
 - Real tree checker system
 - Real type system, not just integers and array modifiers
 - Faster parser, remove the enormous amount of calls for each expression
 - Support for strings and pointers
 - Support for pp-directives, '#include', '#if' ...
 - Start thinking about using a SSA model.
 - Look into faster VM implementations, possibly generate some machine code.
 - Support for initializer lists
 - Support for several built-ins especially 'sizeof'

### Notes:
This project really started out as a single file to generate some boilerplate code, and then it got to this point,
so quality isn't exactly, 'stb' level.