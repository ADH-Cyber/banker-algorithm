#include <stdio.h>
#include <stdlib.h>

// Constants
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

// Arrays to track resource state
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Function prototypes
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

int main(int argc, char *argv[]) {
    // Check if correct number of arguments provided
    if (argc != NUMBER_OF_RESOURCES + 1) {
        fprintf(stderr, "Usage: %s <R1> <R2> <R3> <R4>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse and populate available[] from command-line arguments
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
        if (available[i] < 0) {
            fprintf(stderr, "Error: Resource values must be non-negative integers.\n");
            exit(EXIT_FAILURE);
        }
    }

    // available[] has been initialized, print to screen
    printf("Initial available resources: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    printf("\n");

    return 0;
}
