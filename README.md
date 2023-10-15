# EAC-y 

**Edit and Continue** debugging on UNIX-like systems. 

### Creator/Maintainer: Matthew Smith

## Introduction

Debugging is  a  vital  part  of  the  software  development
process.  And yet, the most well-known debugger on Linux and
its cousins, the GNU Debugger  (GDB),  does  *not*  supports
*edit and continue* (EAC) compilation.  By EAC, we mean  the
ability to pause a program's execution to  edit  its  source
code and rebuild, then continuing execution of  the  program
with that new code. 

The  reason  for  this  feature's  absence   is   its   high
non-triviality of  implementation.   In  the  Windows  world,
Microsoft's Visual Studio has some  implementation  of  EAC.
This  features  is  very  arguably  useful  for   e.g   game
developers who need to squash bugs  which  are  hiding  deep
into runtime. 

EAC-y is  a  very  simple-minded  attempt  to  emulate  this
functionality through the automated loading and unloading of
shared object files. This trick is not new, however existing
implementations are far more complex. EAC-y is my attempt at 
a very lightweight solution. For the sake of comparison we
have for elements, 

    eacy.h
    eacy.cpp
    load.h
    load.cpp
    eacy.py
    eacy.vim

a total of 573  significant  lines  of  code  (SLOC)  as  of
writing. In contrast, a project such as
[hscpp](https://github.com/jheruty/hscpp) contains over  9000
SLOC, while its more popular inspiration  [Runtime  Compiled
C++](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus)
is comparable at its core. 

As of writing, I have not delved too deeply into  either  of
the two  projects  mentioned  above,  so  I  cannot  provide
further insight than this. 

## Building and Installing

1. Build the `eacy` source parser: 

        make all

2. Install everything to your system: 

        sudo make install

3. (optional) Add the following to your `.vimrc`,

        call plug##begin()
        " other plugins ...
        Plug 'DeltaBoyBZ/eacy'
        call plug##end()
source your `.vimrc` and execute `:PlugInstall`.  This isn't
strictly necessary since the next step  will  configure  Vim
anyway. 

4. Configure GDB and Vim to use EAC-y with: 

        ./configure.sh

## Usage (brief)

### Source Code 

At its core, EAC-y can be used with just the header `eacy.h`. We include it in a
source file,

    #include<eacy/eacy.h>

We have to define the shared library to which an EAC
function belongs e.g.

    #define EAC_LIB "./libfoo.so"

We then use macros to mark a function as being *EAC* e.g.

    int foo(int x, int y)
    {EAC_FUNC(foo, int, EAC_WRAP(x, y))
        /*
        regular statements
        */
    } EAC_EXPORT(foo, int, EAC_WRAP(int x, int y), EAC_WRAP(x, y))

EAC-y comes with a parser `eacy` which can process source files and add the
necessary macros. Functions and methods are marked with the `EAC` token and any
encapsulating class is marked with `EAC_CLASS`. 

### Targets and GDB 

The user  should  define  several  targets  in  their  build
system.  There is of course a main  executable  target,  but
there should also  be  a  set  of  shared  library  targets.
For example: 

    libfoo.so: foo.cpp
        g++ -g -shared -fPIC foo.cpp -o libfoo.so

    foo: foo.cpp libfoo.so
        g++ -g -DEAC_DEBUG foo.cpp -o foo

Observe the `-DEAC_DEBUG`, which is necessary for  the  core
executable using EAC-y.  During a GDB  session,  the  shared
library targets are rebuilt and, upon  reaching  a  function
breakpoint, are manually reload at the  GDB  prompt.   EAC-y
defines  three  special  GDB  commands  for  this  workflow:

    eacbreak
    eactbreak
    eacreload

### Vim 

EAC-y was designed with a Vim-based  workflow  in-mind.   It
hence comes with a very small Vim  plugin  which  interfaces
with the `eacy` parser.  There are two important mappings in
particular.

- `<LocalLeader>ep` - run `eacy` on the current buffer and replace
    with the output. This can fail, so be sure that you can
    *undo*. 

- `<LocalLeader>er` - runs `make` for the target named by `EAC_LIB` at 
  the source code position marked by the cursor.

The recommended  workflow  involves  using  the  `Termdebug`
plugin for Vim.  This  is  made  available  by  placing  the
following in your `.vimrc`,

    packadd termdebug

then invoked by executing `:Termdebug`. 

### TODO: Deeper Guide 

### TODO: Video

## Design Limitations

As  of  writing,  the  main  limitation  would  seem  to  be
EAC-y's failure to handle virtual methods. It is likely that
they will have to be handled by a markedly different  system
than regular methods.

It is inconvenient, that a user program must be linked  with
`libeacyload.so`, however this seems at present  to  be  the
most elegant approach.  It allows us to  globally  flag  EAC
components for reloading.

This is an extremely young project.  There is hence much  to
be desired with regard to the workflow  of  EAC-y.   A  more
complete lamentation  can  be  found   in   the
[Design](design.md) document.

## Licensing

All elements of EAC-y are provided under an [MIT License](LICENSE.txt). 

