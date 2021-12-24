#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#define sizeOfBuffer 5
sem_t sem;
sem_t s;//mutual exclusion
sem_t n;//full
sem_t e;//number of empty spaces in buffer
int messageCounter=0;

int buffer[sizeOfBuffer];

//--------------------------------------------------------------------------------//

void* counterFunction(void*args){
int threadNum=*((int*)args);
while(1){
//do something
printf("counter thread %d waiting to write\n",threadNum);
sem_wait(&sem);
messageCounter++;
printf("counter thread %d now adding to counter, counter value=%d\n",threadNum,messageCounter);
sem_post(&sem);

//sleep for a random time
srand(time(0));
int random = (rand()% (15))+0;//random num from 0 to 14
sleep(random);
printf("counter thread %d recieved a message\n",threadNum);
}
}


//producer
void* monitorFunction(void* args){
int bufferIndex=0;
while (1){
//do something
int semVal;
sem_getvalue(&sem,&semVal);
if(semVal<=0){//there are no ready processes yet
printf("Monitor thread: waiting to read counter\n");
}
sem_wait(&sem);//enter critical section
int temp=messageCounter;
printf("monitor thread: reading a count value of %d\n",temp);
messageCounter=0;
sem_post(&sem);


//produce
int val;
sem_getvalue(&e,&val);
if(val<=0)//there are no empty spaces in buffer
{printf("monitor thread:buffer full!\n");}

sem_wait(&e);//wait for empty space
sem_wait(&s);//enter critical sectiom

//enqueue
buffer[bufferIndex]=temp;
printf("monitor thread: writing to buffer at position %d\n",bufferIndex);

sem_post(&s);//get out of critical section
sem_post(&n);//announce that buffer is not empty,and consumer can start working

bufferIndex++;
if(bufferIndex>=sizeOfBuffer)//reset the buffer index and start again
{bufferIndex=0;}


//sleep for a random time
srand(time(0));
int random = (rand()% (15))+0;//random num from 0 to 14
sleep(random);

}
}

//consumer
void* collectorFunction(void* args){
int bufferIndex=0;
while (1){
//do something
//check if buffer is empty
int val;
sem_getvalue(&n,&val);
if(val<=0)//check if buffer has any items
 {printf("collector thread: nothing is in the buffer!\n");}

sem_wait(&n);//wait for buffer to have an item
sem_wait(&s);//enter critical section


printf("collector thread:reading from the buffer at position %d\n",bufferIndex);

sem_post(&s);//get out of critical section
sem_post(&e);//announce theres an empty space in the buffer for the producer to use

bufferIndex++;
if(bufferIndex>=sizeOfBuffer)//reset buffer index to start  again
 {bufferIndex=0;}

//sleep for a random time
srand(time(0));
int random = (rand()% (15))+0;//random num from 0 to 14
sleep(random);
}
}




int main()
{
//initialize semaphores
sem_init(&sem,0,1);
sem_init(&s,0,1);
sem_init(&n,0,0);
sem_init(&e,0,sizeOfBuffer);


//num of counter threads
int N=5;

   //create threads

   pthread_t counter[N];
   int args[N];
   for(int i=0;i<N;i++){
   args[i]=i;
   pthread_create(&counter[i],NULL,&counterFunction,&args[i]);
   }

   pthread_t monitor;
   pthread_create(&monitor,NULL,&monitorFunction,NULL);

   pthread_t collector;
   pthread_create(&collector,NULL,&collectorFunction,NULL);


   //join threads
   pthread_join(collector,NULL);
   pthread_join(monitor,NULL);
   for(int i=0;i<N;i++){
   pthread_join(counter[i],NULL);
   }
    return 0;
}
