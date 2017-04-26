/*
Course: CSC 360 Operating Systems
Student: Shreyas Devalapurkar
Student Number: V00827994
Instructor: Dr. Kui Wu
Assignment 2: Multi-Flow Scheduler
Due Date: November, 4th, 2016
*/

//Define required libraries
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

//Define a linked list structure for the flows with required attributes
typedef struct flow
{
  int number;
  int priority;
  int numbuffer;
  bool isTransmitting;
  float arrivalTime;
  float transmissionTime;
  struct flow *next;
} flowStruct;

// Function defintions at the top of the program in order to avoid incompatible type warnings
void removeFlowQueue(flowStruct *flow);
void addFlowToArray(flowStruct *addedFlow);
void addFlowToQueue(flowStruct *addedFlow);
void executeFlowThread(int numFlows);
void *flowThreadFunction();
void getFlowQueue(flowStruct *flow);
void setTempValues(flowStruct *addedFlow, flowStruct *tempFlow);
void setpresentValueValues(flowStruct *presentValue, flowStruct *tempFlow);
void swapQueue(flowStruct *flow);

// Global variable definitions
flowStruct *flowArray = NULL;
flowStruct *flowQueue = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//Helper function to copy some values from addedFlow into tempFlow
void setTempValues(flowStruct *addedFlow, flowStruct *tempFlow)
{
  tempFlow->number = addedFlow->number;
  tempFlow->arrivalTime = addedFlow->arrivalTime;
  tempFlow->transmissionTime = addedFlow->transmissionTime;
  tempFlow->priority = addedFlow->priority;
  tempFlow->numbuffer = addedFlow->numbuffer;
}

//Helper function to copy some values from tempFlow into presentValue
void setpresentValueValues(flowStruct *presentValue, flowStruct *tempFlow)
{
  presentValue->number = tempFlow->number;
  presentValue->priority = tempFlow->priority;
  presentValue->arrivalTime = tempFlow->arrivalTime;
  presentValue->transmissionTime = tempFlow->transmissionTime;
  presentValue->numbuffer = tempFlow->numbuffer;
}

//Function to create the threads (one per flow) and execute them
void executeFlowThread(int numFlows)
{
  pthread_t threadFlow[numFlows];
  int index = 0;
  flowStruct *presentValue;

  for(presentValue = flowArray; presentValue->priority != 0; presentValue = presentValue->next)
  {
    pthread_create(&threadFlow[index], NULL, flowThreadFunction, presentValue);
    index++;
  }

  for(index = 0; index < numFlows; index++)
  {
    pthread_join(threadFlow[index], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}

//Actual thread function to print the state changes of the flows
void *flowThreadFunction(flowStruct* flow)
{
  struct timeval startTime, endArrTime, endStartTime, endFinishTime;
  gettimeofday(&startTime, NULL);

  usleep(flow->arrivalTime * 100000);

  gettimeofday(&endArrTime, NULL);

  printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%2d). \n", flow->number, (float)((endArrTime.tv_sec - startTime.tv_sec) * 1000000 + endArrTime.tv_usec - startTime.tv_usec) / 100000, flow->transmissionTime, flow->priority);

  getFlowQueue(flow);

  gettimeofday(&endStartTime, NULL);

  printf("Flow %2d starts its transmission at time %.2f. \n", flow->number, (float)((endStartTime.tv_sec - startTime.tv_sec) * 100000 + endStartTime.tv_usec - startTime.tv_usec) / 100000);

  usleep(flow->transmissionTime * 1000000);

  gettimeofday(&endFinishTime, NULL);

  printf("Flow %2d finishes its transmission at time %.2f. \n", flow->number, (float)((endFinishTime.tv_sec - startTime.tv_sec) * 100000 + endFinishTime.tv_usec - startTime.tv_usec) / 100000);

  removeFlowQueue(flow);

  pthread_exit(NULL);
}

//Function to add a flow when needed, and also wait on the condition variable while a flow is waiting for a different flow to complete its transmission
void getFlowQueue(flowStruct* flow)
{
  pthread_mutex_lock(&mutex); //Set the mutex lock

  if(flowQueue->isTransmitting == 0 && (flowQueue == NULL || flowQueue->priority == 0)) //If flowQueue is empty
  {
    addFlowToQueue(flow); //Add the flow to the queue

    pthread_mutex_unlock(&mutex); //Unlock the mutex

    return;
  }

  addFlowToQueue(flow);

  printf("Flow %2d waits for the finish of flow %2d. \n", flow->number, flowQueue->number);

  while(flowQueue->number != flow->number)
  {
    pthread_cond_wait(&cond, &mutex);
  }

  flowQueue->isTransmitting = 1;

  pthread_mutex_unlock(&mutex);
}

void removeFlowQueue(flowStruct* flow)
{
  pthread_mutex_lock(&mutex);

  if(flowQueue->isTransmitting == 1)
  {
    flowQueue = flowQueue->next;
  }

  flowQueue->isTransmitting = 1;

  pthread_cond_broadcast(&cond); //Awaken the threads
  pthread_mutex_unlock(&mutex);
}

void addFlowToArray(flowStruct *addedFlow)
{
  flowStruct *presentValue = flowArray;
  flowStruct *previousValue = presentValue;
  flowStruct *tempFlow = (flowStruct*)malloc(sizeof(flowStruct));

  if (tempFlow == NULL)
  {
    perror("Error: Out of memory");
  }

  setTempValues(addedFlow, tempFlow);

  if (presentValue->priority == 0)
  {
    setpresentValueValues(presentValue, tempFlow);
    presentValue->next = malloc(sizeof(flowStruct));

    if (presentValue->next == NULL)
    {
      printf("Error: Out of memory");
    }

    presentValue->next->priority = 0;

    return;
  }

  while (presentValue->priority != 0)
  {
    previousValue = presentValue;
    presentValue = presentValue->next;
  }

  previousValue->next = tempFlow;
  previousValue->next->next = (flowStruct*)malloc(sizeof(flowStruct));

  if (previousValue->next->next == NULL)
  {
    perror("Error: Out of memory");
  }

  previousValue->next->next->priority = 0;
}

void swapQueue(flowStruct *flow)
{
  flowStruct *temp = (flowStruct*)malloc(sizeof(flowStruct));

  if(temp == NULL)
  {
    perror("Error: Out of memory");
  }

  temp = flow;
  flow = flow->next;
  flow->next = temp;
}

void addFlowToQueue(flowStruct *addedFlow)
{
  flowStruct *presentValue = flowQueue;
  flowStruct *previousValue = presentValue;
  flowStruct *tempFlow = (flowStruct*)malloc(sizeof(flowStruct));

  if(tempFlow == NULL)
  {
    perror("Error: Out of memory");
  }

  setTempValues(addedFlow, tempFlow);
  tempFlow->isTransmitting = addedFlow->isTransmitting;

  if(presentValue->priority == 0)
  {
    setpresentValueValues(presentValue, tempFlow);
    presentValue->isTransmitting = 1;

    presentValue->next = (flowStruct*)malloc(sizeof(flowStruct));

    if (presentValue->next == NULL)
    {
      printf("Error: Out of memory");
    }

    presentValue->next->priority = 0;

    return;
  }

  while(presentValue->isTransmitting == 1)
  {
    previousValue = presentValue;
    presentValue = presentValue->next;
  }

  while(presentValue->priority != 0)
  {
    if(presentValue->priority < tempFlow->priority)
    {
      previousValue = presentValue;
      presentValue = presentValue->next;
    }
    else if(presentValue->priority == tempFlow->priority)
    {
      if(presentValue->arrivalTime < tempFlow->arrivalTime)
      {
        previousValue = presentValue;
        presentValue = presentValue->next;
      }
      else if(presentValue->arrivalTime == tempFlow->arrivalTime)
      {
        if(presentValue->transmissionTime < tempFlow->transmissionTime)
        {
          previousValue = presentValue;
          presentValue = presentValue->next;
        }
        else if(presentValue->transmissionTime == tempFlow->transmissionTime)
        {
          if(presentValue->numbuffer < tempFlow->numbuffer)
          {
            previousValue = presentValue;
            presentValue = presentValue->next;
          }
          else
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  if(presentValue->priority == 0)
  {
    setpresentValueValues(presentValue, tempFlow);
    presentValue->isTransmitting = tempFlow->isTransmitting;
    presentValue->next = (flowStruct*)malloc(sizeof(flowStruct));

    if (presentValue->next == NULL)
    {
      printf("Error: Out of memory");
    }

    presentValue->next->priority = 0;
  }
  else if(previousValue == presentValue)
  {
    flowQueue = tempFlow;
    flowQueue->next = presentValue;
  }
  else
  {
    previousValue->next = tempFlow;
    previousValue->next->next = presentValue;
  }
}

int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    printf("Incorrect number of command buffer arguments. Correct usage: ./MFS file.txt where file is the name of the input file\n");
    return -1;
  }

  if(access(argv[1], F_OK) == -1)
  {
    printf("Sorry the file does not exist\n");
    return -1;
  }

  FILE* fp = fopen(argv[1], "r");
  char buffer[60];
  char *token;
  int counter1 = 0;
  int counter2 = 0;
  int numFlows;

  if(fp != NULL)
  {
    flowArray = (flowStruct*)malloc(sizeof(flowStruct));
    flowQueue = (flowStruct*)malloc(sizeof(flowStruct));
    flowStruct *flowThread;

    if(flowArray == NULL || flowQueue == NULL)
    {
        perror("Error: Out of memory");
        return 1;
    }

    flowQueue->priority = 0;
    flowArray->priority = 0;

    while(fgets(buffer, 60, fp) != NULL)
    {
      token = strtok(buffer, ":\n");

      while(token)
      {
        if(counter1 == 0)
        {
          numFlows = atoi(token);
          flowThread = (flowStruct*) malloc(sizeof(flowStruct));

          if(flowThread == NULL)
          {
            perror("Error: Out of memory");
            return 1;
          }

          flowThread->isTransmitting = 0;
        }
        else
        {
          if(counter2 == 0)
          {
            flowThread->number = atoi(token);
          }
          else if(counter2 == 1)
          {
            flowThread->arrivalTime = atof(token) / 10;
          }
          else if(counter2 == 2)
          {
            flowThread->transmissionTime = atof(token) / 10;
          }
          else if(counter2 == 3)
          {
            flowThread->priority = atoi(token);
            flowThread->numbuffer = counter1;

            addFlowToArray(flowThread);
          }
        }

        token = strtok(NULL, ",\n");

        if(counter1 > 0)
        {
          counter2 = (counter2 + 1) % 4;
        }
      }

      counter1++;
    }

    free(flowThread);
  }

  fclose(fp);

  executeFlowThread(numFlows);
}
