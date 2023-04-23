#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct 
{
    int threads;
    long limit;
} args;

bool cmdLineRead(int argc, char** argv); 
void *incrementCounter();
void checkNum(int num);
int addFactors(int n);


int counter = 10; // global counter
int pairCounter = 0; // keep track of pairs, doubles as an array Index
int amicablePairArray[250][2]; // store amicable pairs
pthread_mutex_t counterLock; // mutex lock to ensure mutual exclusion for counter
pthread_mutex_t arrayLock; // mutex lock to ensure mutual exclusion for array

int main (int argc, char** argv)
{
    if (cmdLineRead(argc, argv) == false) // run cmdLineRead to ensure no errors
    {
        return EXIT_SUCCESS;
    }
    
    pthread_mutex_init(&counterLock, NULL); // initialize the counterLock
    pthread_mutex_init(&arrayLock, NULL); // initialize the arrayLock
    pthread_t threadID[args.threads]; // create an array of type pthread_t to store threads

    printf("CS 370 - Project #3\n");
    printf("Amicable Numbers Program\n\n");
    printf("Thread Count:   %d \n", args.threads);
    printf("Numbers Limit:  %ld \n\n", args.limit);
    printf("Please wait. Running...\n\n");

    for (int i = 0; i < args.threads; i++) // loop through thread creation
    {
        pthread_create(&threadID[i], NULL, incrementCounter, NULL);
    }

    for (int i = 0; i < args.threads; i++) // join threads once they have completed all numbers
    {
        pthread_join(threadID[i], NULL);
    }

    printf("Amicable Numbers\n");

    for (int i = 0; i < pairCounter; i++) // loop through the global array to output results
    {
        for (int j = 0; j < 2; j++)
        {
            printf("%12u", amicablePairArray[i][j]);
        }
        printf("\n");
    }

    printf("\nCount of amicable number pairs from 1 to %ld is %d\n\n", args.limit, pairCounter);

    return EXIT_SUCCESS; // program exited successfully
}

void *incrementCounter()
{
    while (true) // simply keep looping
    {
        pthread_mutex_lock(&counterLock); // CRITICAL SECTION, incrementing a global counter
        if (counter > args.limit) // if counter has reached the limit, exit
        {
            pthread_mutex_unlock(&counterLock); // unlock counterLock
            return NULL; // exit out of the while loop
        }
        int tempCount = counter++; // set tempCount equal to the next number to check
        pthread_mutex_unlock(&counterLock); // unlock counterLock
        checkNum(tempCount); // call checkNum with the new number
    }
}

void checkNum(int original)
{
    int num1 = addFactors(original); // add all factors of the original number

    // ensure added factors do not exceed the size of the limit and are greater than the original number
    if (num1 > original && num1 <= args.limit)
    {
        int num2 = addFactors(num1); // add all the factors of the previously recieved number

        if (num2 == original) // this means we have found an amicable pair
        {
            pthread_mutex_lock(&arrayLock); // CRITICAL SECTION, altering global array
            amicablePairArray[pairCounter][0] = num2; // inserting into a global array, column 0
            amicablePairArray[pairCounter][1] = num1; // inserting into a global array, column 1
            pairCounter++; // increment the amount of pairs we have found
            pthread_mutex_unlock(&arrayLock); // unlock arrayLock
        }
    }
}

int addFactors(int n)
{
    int sum = 1; // every number will at least add up to one as the sum of their factors
    
    for (int i = 2; i <= (int)sqrt(n); i++) // start at two, divide up to sqrt(n)
        {   
            if (n % i == 0) // check if dividing temp by i yields a remainder
            {
                sum += i; // if the remainder is 0, then i is a divisor of n, add it to sum
                sum += n/i; // add n divided by our factor to our sum as well
            }
        }

    return sum; // return the sum of all added factors
}

bool cmdLineRead(int argc, char** argv)
{   
    
    if (argc != 5) // Output the usage of the program if no arguments are entered
    {
        printf("Usage: ./amNums -t <threadCount> -l <limitValue>\n");
        return false;
    }
     
    if (strcmp(argv[1], "-t") != 0) // Check if the thread count specifier is valid
    {
        printf("Error, invalid thread count specifier.\n");
        return false;
    }

    char *c;
    long argCheck = strtol(argv[2], &c, 10);

    // Had to source cplusplus.com for info on strtol
    if (*c != '\0' || c == argv[2]) // Check if the thread count is in valid format
    {
        printf("Error, invalid thread count value.\n");
        return false;
    }
    
    if (atoi(argv[2]) < 1 || atoi(argv[2]) > 64) // Check if the thread count is within the bounds
    {
        printf("Error, thread count must be >= 1 and <= 64.\n");
        return false;
    }

    if (strcmp(argv[3], "-l") != 0) // Check if the limit value specifier is valid
    {
        printf("Error, invalid limit value specifier.\n");
        return false;
    }

    argCheck = strtol(argv[4], &c, 10);

    // Had to source cplusplus.com for info on strtol
    if (*c != '\0' || c == argv[4]) // Check if the limit count is in valid format
    {
        printf("Error, invalid limit value.\n");
        return false;
    }

    if (atol(argv[4]) < 64) // Check if the thread count is within the bounds
    {
        printf("Error, limit must be > 64.\n");
        return false;
    }

    args.threads = atoi(argv[2]); // convert the amount of threads to an int
    args.limit = atol(argv[4]); // convert the limit to a long

    return true; // return true when there were no errors
}
