Luke Woodhead
luwo7426@colorado.edu

Makefile: 
	Turns all of the c code into executable objects to be added to the kernel, both system calls added here to be compilied into the kernel as .o files

syscalls.h: 
	Provides a function prototype with correct arguments to be used with the system call for each system call

syscall_64.tbl:
	Table assigning a system call number to each system call
	333 is the system call number for sys_helloworld
	334 is the system call number for sys_cs3753_add

cs3753_add_test.c:
	My test function to call my system call. Passes in 2 numbers and an address for a result to be stored in then prints the result. Also prints 0 if the system call returns without an error.
	To run use gcc -o test cs3753_add_test.c then ./test
	The int values can be changed inside the program
	Used //https://brennan.io/2016/11/14/kernel-dev-ep3/ for print statement and to learn how to make a syscall

cs3753_add.c:
	My system call. Takes in 2 ints as arguments and an int pointer, adds the ints together and prints the result. Writes the result back into the int pointer in user space using copy_to_user.

/var/log/syslog:
	Contains the kernel output from the system call