#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define DEVICE_NAME "/dev/pa2_char_d"
#define MAX_BUFFER 1024



char *buf;

void test_read(){
	printf("Enter the number of bytes you want to read:");
	int buf_size;
	scanf("%d", &buf_size);
	buf = malloc(buf_size);
	int file = open(DEVICE_NAME,O_RDWR);
	read(DEVICE_NAME,buf,buf_size);
	release(DEVICE_NAME);
	//release(DEVICE_NAME);
	//read(file,buf,buf_size);
	//Call to read
	printf("Data read from the device: %s\n",buf);
	free(buf);
}

void test_write(){
	printf("Enter data you want to write to the device:\n");
	char input[MAX_BUFFER];
	scanf("%[^\n]%*c", input);
	buf = malloc(strlen(input));
	memcpy(buf,&input,strlen(input));
	int file = open(DEVICE_NAME,O_RDWR);
	write(DEVICE_NAME,buf,strlen(input));
	release(DEVICE_NAME);
	free(buf);
	//Call to write
}

void test_seek(){
	int offset;
	int whence;
	printf("Enter an offset value:");
	scanf("%d",&offset);
	printf("Enter a value for whence:");
	scanf("%d",&whence);
	int file = open(DEVICE_NAME,O_RDWR);
	llseek(DEVICE_NAME,(loff_t)offset,whence);
	release(DEVICE_NAME);
	//Call to seek
}

int main(void){
	buf = malloc(MAX_BUFFER);
	bool running = true;
	while(running){
		char input[MAX_BUFFER];
		scanf("%s",input);
		if(strlen(input) == 1){
			switch(input[0]){
				case 'r':
					printf("%s\n","reading");
					test_read();
					break;
				case 'w':
					printf("%s\n","writing");
					test_write();
					break;
				case 'e':
					running = false;
					break;
				case 's':
					printf("%s\n","seeking");
					test_seek();
					break;
			} 
		}
	}
} 