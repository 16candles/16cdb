16cdb
-----

The 16candles debugger

Joe Jevnik
Licensed under the GNU Public License version 2.


Building and Installing 16candles
---------------------------------

To build 16candles, the host machine must have CMake and a C compiler
supporting the gnu11 standard, and must be somewhat POSIX-compliant. Virtually
all Unix-like operating systems, including GNU/Linux, FreeBSD, and Mac OS X are
capable of compiling 16c.

The fastest way to build 16candles is to simply enter these commands from the
project directory:

    $ cmake .
    $ make

It is recommending that you create a build directory. With this in mind, the
commands become:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make .

There are no installation targets for the build system yet. In the meantime,
feel free to move or copy the created executables manually and pretend that a
build script is doing it.


Additional Projects
-------------------

- The VM: [https://github.com/16candles/16c]
- Compiler written in Haskell (supports when/unless):
  [h16cc](https://github.com/llllllllll/16candles_haskell)
- Compiler written in C: [16cc](https://github.com/16candles/16cc)
