# Posit-LLVM

A pass for rewriting arithmetic operations between doubles either as operations between floats or 32-bit posits.
Arithmetic operations are rewritten inline using LLVM.
It's for LLVM 3.8.

Build:

    $ cd cs6120-project3
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..
    $ make install

Run:
 
    To compile and run the C program `"src".c` with doubles replaced with floats for arithmetic operations, run:

    $ make float_run SRC="src"

    To compile and run `"src".c` with arithmetic operations on doubles replaced with the associated 32-bit posit arithmetic operations, run:
    
    $ make posit_run SRC="src"

Output LLVM:

    To write the LLVM code resulting from the posit LLVM pass, run:

    $ make llvm_posit SRC=`src`

    Similarly, to write the float LLVM pass, run:

    $ make llvm_float SRC=`src`

Run:
    
    
    $ clang -Xclang -load -Xclang build/posit/libPositPass.* foo.c
