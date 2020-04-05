#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <ctype.h>

//This is a struct used to access the semaphores
typedef struct _rwloct_t{
    sem_t lock;
    sem_t writelock;
    int readers;
}rwlock_t;

//So I used this global variable to help me starve the readers, it is assigned origianlly to 0
//But when a writer is writing this value is assigned as 1 to call semwait on the reader fcn using semaphore: lock
int blockval=0;

//FUNCTION: This function is used inside my reader and writer function to initialize my semaphores
void rwlock_init(rwlock_t *rw) { 
   rw->readers = 0;
   sem_init(&rw->lock, 0, 1);//DEBUG: In the future you may want to change this to 0 for reader to wait to let writer
   sem_init(&rw->writelock, 0, 1); 
}
//FUNCTION: This function is used inside my reader function to acquire a reader thread
void rwlock_acquire_readlock(rwlock_t *rw) { 
   if(blockval=1){
       printf("Writer is reading, you must wait");
	    sem_wait(&rw->lock);//if a writer has entered to write this condition will cause a reader to wait sleeping on lock
	}
	else if(blockval=0){
        sem_wait(&rw->writelock);//This will pause a reader if another reader is reading
	}
	sem_post(&rw->writelock); //This will awake a reader or writer right away to make everyong take turns fairly
   
} 
//FUNCTION: This function is used inside my reader function to release a reader thread
void rwlock_release_readlock(rwlock_t *rw) { 
   sem_post(&rw->writelock); //In case a writer is waiting 
} 
//FUNCTION: This function is used inside my writer function to acquire a writer thread
void rwlock_acquire_writelock(rwlock_t *rw) { 
    blockval=1//This assignment will cause any reader after writer to pause until writer has finished writing
    printf("wait for writer");
    sem_wait(&rw->writelock); //Calling semwait for a writer that is behind another writer
    
   
} 
//FUNCTION: This function is used inside my writer function to release a writer thread
void rwlock_release_writelock(rwlock_t *rw) { 
   sem_post(&rw->writelock); //Waking up anyone next in line 
   blockval=0;//This will avoid a reader from being put to sleep because a writer is done writing
   sem_post(&rw->lock); //waking up a sleeping reader 
} 
//Funciton to waste time for the acquire and wait functions to operate while a person is reading or writing data
void reading_writing(){
    float x;
	x=0;
	float t;  
	float i;
	float j;
	t = rand()%10000; 
	for(i = 0; i < t; i++){
		for(j = 0; j< t; j++){ 
		}
			x=i+j;
	}
}
rwlock_t w;//this is a struct type to access the semaphores in the function calls in side reader and writer

//Writer
void* writer(void* arg){
rwlock_init(&w);

rwlock_acquire_writelock(&w);

reading_writing();

rwlock_release_writelock(&w);
//This is revealing order of the reader and writer
printf("Writer Done");
}

void* reader(void* arg){

rwlock_init(&w);

rwlock_acquire_writelock(&w);

reading_writing();

rwlock_release_writelock(&w);
//This is revealing order of the reader and writer
printf("Reader Done");
}



int main(){

//variable to access reader and writer characters in the scenario text file to read
char ch;
//Count for number of readers and writers to find the number of readers and writers 
int i, readct, writect;
readct, writect=0;

//Opening File, fp is my pointer
FILE* fp = fopen("scenarios.txt", "r");

//need to get the count for threads reads and write after you read in r count and w count
if(fp==NULL){
	printf("File is not available \n");
}
else{
	printf("Reading File");
	while((ch=fgetc(fp))!=EOF){
		printf("Character:\n %c", ch);
		if(ch=='w'){
			writect++;
		}
		else if(ch=='r'){
			readct++;
		}
	}
}
//This is just explaing the order of the text files for readers and writers
printf("Write count is: %d\n Read count is: %d\n", writect, readct);

//Creating Threads process
pthread_t read[5], write[5];
//READ THREADS
for(i = 0; i < readct; i++) {
   printf("Creating a read thread");
   pthread_create(&read[i], NULL, reader, NULL);
}
//WRITER THREADS
for(i = 0; i < writect; i++) {
    printf("Creating a write thread");
    pthread_create(&write[i], NULL, writer,NULL);
}
//JOINING THREADS
for(i = 0; i < readct; i++) {
        pthread_join(read[i], NULL);
}
for(i = 0; i < writect; i++) {
        pthread_join(write[i], NULL);
}
//Close my file
fclose(fp);
return 0; 
      
   


}
