#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
	int number1 = 6;
	int number2 = 12;
	int result;
	//printf("hey\n");
  	long res = syscall(334,number1,number2,&result); //Pass in the system call number, two numbers to add and a pointer to where we want the result stored  
  	printf("System call returned %ld.\n", res);
  	printf("Adding %d and %d\n",number1,number2);
	printf("%d + %d = %d\n",number1,number2,result);
	return 0;
}
		