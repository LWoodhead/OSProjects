#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>  
#include <string.h>
#include "multilookup.h"
#include "util.h"

//Mutex Objects
sem_t inputFileSem;//Binary semaphore allowing only one thread to select a file to read, gates access to inputFiles and inputFilesIndex
sem_t buffer_full;//Copied from consumer producer problem
sem_t buffer_empty;//Copied from consumer producer problem
sem_t buffer_access;//Binary semphore to read or write from or to buffer
sem_t requester_log_access;//Binary semaphore to write into requester log
sem_t result_log_access;//Binary semaphore to write into the result log
sem_t resolver_wait;//Switches resolver threads to alternate code 

//Global Variables
char inputFiles[50][30];//Array containing the input file names, limit of 50
int inputFilesIndex = 0;//INdex of inputFiles array
int totalInputFiles = 0;//Total number of input files, initilalized in main
char sharedArrayBuffer[500][1025];//Shared buffer between requesters and resolvers
int sharedArrayIndex = 0;//Shared buffer index
char requester_log_file[30];//Filename for requester log file, assigned in main
FILE *result_file_ptr;//Pointer to open result file, assigned in main


void *requester_thread(){
	pid_t tid = pthread_self();//Thread id
	int files_serviced = 0;//Number of files this thread has serviced
	char fileName[30];//Place to store the current file being reads name 
	//Loops until all files have been read
	while(1){

		//Gain a lock on the input file variables, select a new file to read if able otherwise break
		sem_wait(&inputFileSem);
		if(inputFilesIndex < totalInputFiles){
			strcpy(fileName,inputFiles[inputFilesIndex]);
			inputFilesIndex++;
		}else{
			break;
		}
		sem_post(&inputFileSem);
		

		//If the file name is valid read out of it otherwise move on and chose a new file
		FILE *read_ptr = fopen(fileName,"r");
		if(read_ptr != NULL){
			
			//Read all lines out of the locked file
			char address[1025];
		    while(fgets(address, 1025, read_ptr)){
		    	address[strlen(address)-1] = '\0';

		    	//Using 2 semaphores implement a consumer producer problem so only 1 thread can write or read from the shared buffer
		    	sem_wait(&buffer_empty);
		    	sem_wait(&buffer_access);
		    	//Increase index then write into the buffer
				strcpy(sharedArrayBuffer[sharedArrayIndex],address);
				sharedArrayIndex++;
				fprintf(stderr, "new index is %d\n", sharedArrayIndex);
		    	
		    	sem_post(&buffer_access);
		    	sem_post(&buffer_full);
		    	
	    	}
	    	//Close the file
	    	fclose(read_ptr);
		}else{
			fprintf(stderr,"input file %s is invalid\n", fileName);
			fflush(stderr);
		}
		//Increment the number of files this thread has serviced
	    files_serviced++;   
	}
	sem_post(&inputFileSem);

	sem_wait(&requester_log_access);
	FILE *requester_thread_log_ptr = fopen(requester_log_file,"w");
	//Exit if the requester log doesn't open correctly and print an error
	if(requester_thread_log_ptr == NULL){
		fprintf(stderr, "Invalid requester output log\n");
		exit(1);
	}
	//Write to the requester log
	fprintf(requester_thread_log_ptr,"Thread <%d> serviced %d files.", (int)tid,files_serviced);
	fflush(requester_thread_log_ptr);
	fclose(requester_thread_log_ptr);
	sem_post(&requester_log_access);

	//exit the thread
	printf("exiting requester thread,%d",(int)tid);
	pthread_exit(NULL);
}

void *resolver_thread(){
	pid_t tid = pthread_self();
	char address[1025];
	char ip_address[40];
	//Loops until all inputs have been read in and resolved, then hangs on full sem until main function kills these threads
	while(1){

		//Using 2 semaphores implement a consumer producer problem so only 1 thread can write or read from the shared buffer
		
		sem_wait(&buffer_full);
		sem_wait(&buffer_access);
		//Copy item from the shared buffer the decrement the index
		strcpy(address,sharedArrayBuffer[sharedArrayIndex-1]);
		sharedArrayIndex--;
		fprintf(stderr, "new index is %d\n", sharedArrayIndex);
			fprintf(stderr, "current address is %s of size %d vs commitdie of size %d\n", address,(int)strlen(address),(int)strlen("commitdie"));
		//If this is the address string all requesters are done thus we dont need to share access, break out of this loop and use a modified version
		// if(strcmp(address,"commitdie") == 0){
		// 	fprintf(stderr, "breaking\n");
		// 	break;
		// }

		sem_post(&buffer_access);
		sem_post(&buffer_empty);

		dnslookup(address,ip_address,1025);//Perform the dnslookup
		fprintf(stderr,"%s\n",ip_address);
		//Gain a lock on the requester log and write into it, file opened in main
		sem_wait(&result_log_access);
		fprintf(result_file_ptr,"%s,%s\n",address,ip_address);
		sem_post(&result_log_access);

		//Check to see if the ip address was empty
		if(strlen(ip_address) < 2){
			fprintf(stderr, "%s is an invalid hostname\n", address);
			fflush(stderr);
		}
	}
	sem_post(&buffer_access);
	sem_post(&buffer_empty);
	//exit the thread


	printf("exiting resolver thread,%d",(int)tid);
	pthread_exit(NULL);
}

// void main(){
// 	printf("happy white person .gif\n");
// }

void main(int argc, char *argv[]){
	printf("Point 1\n");
	//Create local variables and assign global variables from command line input
	int requester_thread_count = atoi(argv[1]);
	int resolver_thread_count = atoi(argv[2]);
	strcpy(requester_log_file,argv[3]);
	char result_log_filename[30];
	strcpy(result_log_filename,argv[4]);
	char inputs_filename[30];
	strcpy(inputs_filename,argv[5]);
	pthread_t requester_thread_tids[requester_thread_count];//Store requester tids
	pthread_t resolver_thread_tids[resolver_thread_count];//Store resolver tids

	printf("Point 2\n");
	printf("%d is the number of resolver threads\n", resolver_thread_count);
	printf("%d is the number of resolver threads\n", requester_thread_count);
	//Initialize mutex onjects
	sem_init(&inputFileSem,0,1);//2nd argument is 0 so that the semaphore is shared between threads and 3rd argument is 1 for a binary semaphore
	sem_init(&buffer_full,0,0);//intialized to 0 since initally the buffer is empty
	sem_init(&buffer_empty,0,500);//Initially no values in the buffer so initialize this as the max of 500
	sem_init(&buffer_access,0,1);//Three more binary semaphores
	sem_init(&requester_log_access,0,1);
	sem_init(&result_log_access,0,1);
	sem_init(&resolver_wait,0,1);

	printf("Point 3\n");
	//Read into the inputFiles array the names of the input files to be read
	FILE *input_files_ptr = fopen(inputs_filename,"r");
	//Exit if the filename is bogus
	if(input_files_ptr == NULL){
		fprintf(stderr,"%s is an invalid file of filenames\n",inputs_filename);
		exit(1);
	}
	char filename[30];
    while(fgets(filename, 30, input_files_ptr)){
    	filename[strlen(filename)-1] = '\0';
    	strcpy(inputFiles[totalInputFiles],filename);
    	totalInputFiles++;//Finds the total number of input files
    }
    fclose(input_files_ptr);

    for(int i=0;i<totalInputFiles;i++){
		printf("%s\n",inputFiles[i]);
	}

    printf("Point 4\n");
    //Open up and check the result thread
    result_file_ptr = fopen(result_log_filename,"w");
    if(result_file_ptr == NULL){	
    	fprintf(stderr, "%s is an invalid results file\n", result_log_filename);
    	exit(1);
    }

	//Create the correct number of requester threads
    for(int i = 0; i < requester_thread_count;i++){
    	pthread_create(&requester_thread_tids[i],NULL,requester_thread,NULL);
    }

    //Create the correct number of resolver threads
    for(int i = 0; i < resolver_thread_count;i++){
    	pthread_create(&resolver_thread_tids[i],NULL,resolver_thread,NULL);
    }

    //Main waits for all the requester threads to finish
    for(int i = 0; i < requester_thread_count;i++){
    	pthread_join(requester_thread_tids[i],NULL);
    }
    printf("Point 5\n");

    /*after all requester threads have rejoined main no new values are added to the shared buffer so gain a lock on the buffer and add
    as the resolver thread counts worth of message to tell the resolver threads to exit, signal buffer_full the number resolver threads
    there are times, this means every resolver thread can move past the sem_wait(&buffer_full) exit the loop and then exit the thread*/
    
    sem_wait(&buffer_empty);
	sem_wait(&buffer_access);
    for(int i = 0; i < resolver_thread_count; i++){
    	char *end_signal = "commitdie";
    	strcpy(sharedArrayBuffer[sharedArrayIndex], end_signal);
    	sharedArrayIndex++;
    }
    sem_post(&buffer_access);
    sem_post(&buffer_full);
    fprintf(stderr, "point 6\n" );

    //Main then waits for all resolver threads to join
    for(int i =0; i < resolver_thread_count; i++){
    	pthread_join(resolver_thread_tids[i],NULL);
    }

    fclose(result_file_ptr);
    //Main prints and returns
    printf("Done :)\n");
}