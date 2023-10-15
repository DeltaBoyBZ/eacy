# EAC-y Design

## Description

### Core 

An EAC function is a function which  can  be  edited  during
a GDB session with measurable effect on the execution of the
program without the need for restarting.   This  feature  is
implemented by trading the (usually  static)  function  code
for the same code compiled as a separate shared object file,
and dynamically loaded.  Each time  the  function  executes,
this shared object file may have been rebuilt and  reloaded.
Any change affecting the shared object file will  eventually
be incorporated into the static code when that  is  rebuilt.

We this create the illusion that changing  the  static  code
affects execution at runtime.  It is arguably not true  EAC,
but it achieves approximately the  same  effect.   The  code
which loads and runs the dynamic versions of  the  functions
is dropped by  the  preprocessor  whenever  the  `EAC_DEBUG`
marcro is undefined.  There is hence no performance  penalty
for a *release* build. 

The macros which signify that  a  C-style  function  is  EAC
take the form:

    EAC_FUNC(fhandle, ret, arg_names)
    EAC_EXPORT(fhandle, ret, args, arg_names)

For example:

    int foo(int x, int y) {
        EAC_FUNC(foo, int, x, y);
        return x + y;
    } EAC_EXPORT(foo, int, 
        EAC_WRAP(int   x,   int    y),    EAC_WRAP(x,    y))

The detailed relevance of each argument  is  best-understood
by inspecting the (short) source code in [`eacy.h`](eacy.h).
We  however  give  a  rough  overview  via  the  parameters:

- `fname` - the source handle of the function - used to deduce the string form of 
    the function name for dynamic loading. Also used to deduce the full function
    type  for  calling  the  dynamically  loaded   function.

- `ret` - the return type of the function - used to  specify  the  return  value
  saved  when  calling  the  dynamically  loaded   function.

- `args` - the arguments of the  function  as  they  appear  in  its  definition

- `arg_names` - the names of the function arguments (i.e.   without  their  type
  qualifiers) - passed  to  each  variant  of  the  function

Since  `args`  is  often  a   comma-separated   list,   it   is   important   to
use    the    `EAC_WRAP`    macro    to    surrounded    it.

It is important to note that there exists a  separate  macro
`EAC_VOID_FUNC` to deal with functions which return  `void`.
Here,   the   user   should   omit   the   `ret`   argument.

### Parser

Rather than manually  insert  `EAC_FUNC`  invocations,  some
might prefer a parser to insert them automatically.   If  we
mark a funcion in e.g. `foo.c`

    EAC int foo(int x, int y) {
        return x + y;
    }

and then run `eacy foo.c` then  we  should  see  printed  to
standard output:

    int foo(int x, int y) { 
        EAC_FUNC(foo, int, x, y)
        return x + y;
    } EAC_EXPORT(foo, EAC_WRAP(int), 
        EAC_WRAP(int   x,   int    y),    EAC_WRAP(x,    y))

This could optionally replace the original source file,  and
it is as if the user had typed  the  invocation  themselves.

### Other Types of Function

In C++, there are of course many function  types  which  are
not C-style functions.  The prime example is class  methods.
The standard symbol loading function,  `dlsym`,  only  works
with C-style functions.  These functions are constructed  in
C++ via the `extern "C"`  qualifier.   We  may  then  create
a C wrapper for any method e.g.

    class Foo 
    {
    public:
        int bar(int x, int y)
        {
            return x * y;
        }
    };

    extern "C"  int  bar_export(Foo*  foo,  int  x,  int  y)
    {
        return foo->bar(x, y);
    }

The process of creating the `extern` function is  abstracted
away with the  `EAC_EXPORT`  and  `EAC_VOID_EXPORT`  macros.

    EAC_EXPORT(cname, fname, args, arg_names)

The  method  is  marked  as   an   EAC   function   with   an   invocation   of:

    EAC_METHOD(cname, fname, ret, arg_names)

The mode of operation is almost entirely identical  to  that
of `EAC_FUNC`, except the user is required to  name  `cname`
of the host class.  Furthermore,  we  have  a  corresponding
macro `EAC_VOID_METHOD` for void functions. 

Depending on where the method is actually defined, the  user
may also be required  to  use  the  `EAC_EXPORT_DEC`  macro.

In this framework, we can render the  method  `Foo::bar`  as
EAC with:

    EAC_METHOD_EXPORT_DEC(Foo,   bar,   int,    EAC_WRAP(int    x,    int    y))
    class Foo 
    {
    public:
        int bar(int x, int y)
        {
            EAC_METHOD(Foo,  bar,   int,   EAC_WRAP(x,   y))
            return x + y;
        }
    };EAC_VOID_METHOD_EXPORT(Foo, bar, int, 
        EAC_WRAP(,  int  x,  int  y),  EAC_WRAP(,   x,   y))

The  method  then  behaves  like  any  other  EAC  function.

For virtual functions, the current recommended  workflow  is
to  factor  out  the  desired  EAC  components  into   other
functions, and mark *those* functions as EAC. 

### Building Components 

In order to make use of EAC, the core program must:

- Be compiled with definition `-DEAC_DEBUG`
- Be linked with `libeacyload.so`

It is intended that the program will always be run from  the
project root directory.  EAC shared  libraries  live  inside
a subdirectory `.eac`.  When the use builds an EAC component
e.g.   `libfoo.so`,  they  should  ensure  the   output   is
`.eac/libfoo.so`.

During a debugging session, a set of  directories  `.eac.1`,
`.eac.2`, `...` are created  by  debug  scripts.   The  user
should not need to interact with these. 

### The GDB Session 

By default, all EAC functions  make  use  of  components  in
`.eac`.  These are the components as they were upon  program
execution. 

During program runtime, the user may edit the source code in
EAC functions, then rebuild the corresponding component.
Then, at the GDB console:

    <C-c> # or otherwise interrupt execution
    (gdb) eacreload
    (gdb) continue

If done correctly the user should see their code changes
reflected. 

The `eacreload` command increments the *revision counter* by
one, which implicitly flags all components  to  reload  when
they  are  needed.   The  `.eac`  directory  is  copied   to
`.eac.<revision_counter>`  and  it  is  files  in  this  new
directory which are now used. 

## Evaluation

The biggest limitation to EAC-y is the lack of support for
virtual functions. 

### TODO complete explanation as to why

With regard to workflow,  one  question  is:  *Why  so  many
arguments in the macros?* Indeed, In most instances, we will
be   providing   redundant   information   e.g.    the
return   type   of   the   function.    Unfortunately,   the
C   preprocessor   has   a   fair   few   limitations   when
it  comes  to  programmatically  deducing  features  such  as
this.   The  macros  in  their  current  form  are,  as  far
as   I   cam   currently   aware,   minimal.

The arguable  clunkiness  of  the  macro-based  workflow  is
mitigated through the use of the `eacy`  parser.   The  user
need only mark their functions or methods with `EAC` and  the
parser   inserts   the   necessary   macro    code.     Some
might say this is still  awkward,  however  it  is  easy  to
write a plugin for an extensible text  editor  such  as  Vim
or  Emacs  which  should   make   this   process   smoother.

Another   potential   improvement   would    be    in    the
implementation  of  EAC  class  methods.    Entire   classes
*can* be dynamically  loaded  by  implementing  creator  and
destructor  interfaces  as  `extern  "C"`   functions.    To
utilise this in any meaningful  way  however,  EAC  funtions
will have to be  declared  as  virtual,  which  is  a  great
restrition.    We   considered   it   more   straightforward
therefore    to    require    the    `EAC_EXPORT`    macros.

