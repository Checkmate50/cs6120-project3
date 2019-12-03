CC=/bin/gcc
all:
	
install:
	cd build && make && cd .. && \
	gcc -c posit/posit.h posit/posit_convert.c posit/posit_ops.c
clean:
	rm *.ll && rm *.o && rm *.out
llvm_posit: ${SRC}.c
	clang -S -emit-llvm -Xclang -disable-O0-optnone ${SRC}.c; \
	opt -load build/posit/libPositPass.* -posit -S ${SRC}.ll
llvm_float: ${SRC}.c
	clang -S -emit-llvm -Xclang -disable-O0-optnone ${SRC}.c; \
	opt -load build/float/libFloatPass.* -float -S ${SRC}.ll
# Really should be make build, but don't feel like getting stupid Makefile stuff to work
posit_run:
	clang -Xclang -load -Xclang build/posit/libPositPass.* -c ${SRC}.c && \
	gcc ${SRC}.o posit_convert.o posit_ops.o && \
	./a.out
float_run:
	clang -Xclang -load -Xclang build/float/libFloatPass.* -c ${SRC}.c && \
	gcc ${SRC}.o && \
	./a.out