# llvm-pass-skeleton

A completely useless LLVM pass.
It's for LLVM 3.8.

Build:

    $ cd cs6120-project3
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -Xclang -load -Xclang build/posit/libPositPass.* foo.c
