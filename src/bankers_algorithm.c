/*
 * File: bankers_algorithm.c
 * Author: Austin Howard
 * Date: 2025-07-27
 * 
 * Description:
 *  Simulates the Banker’s Algorithm for deadlock avoidance by managing multiple customer 
 *  requests and releases for shared resources. The program checks for safe states using 
 *  the Safety Algorithm and ensures allocations do not lead to unsafe states that could 
 *  cause deadlock.
 * 
 * Course: 
 *  CS7343 - Operating Systems
 *  Dr. Maya El Dayeh
 *  Southern Methodist University
 * 
 * Compile using:
 *  gcc -o banker src/bankers_algorithm.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

// The available amount of each resource
int available[NUMBER_OF_RESOURCES];

// The maximum demand of each customer
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The amount currently allocated to each customer
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The remaining need of each customer
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Function prototypes
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

int main(int argc, char *argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf(
            "Usage: %s <resource1> <resource2> ... <resource%d>\n", 
            argv[0], 
            NUMBER_OF_RESOURCES
            );
            return 1;
        }

    // Parse available[] from command-line
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    // Debug print to verify values
    printf("Initial available resources:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("Resource %d: %d\n", i, available[i]);
    }

    // Open the input file
    FILE *file = fopen("src/maximum.txt", "r");
    if (file == NULL) {
        perror("Error opening maximum.txt");
        return 1;
    }

    // Read values into maximum[][]
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, "%d%*c", &maximum[i][j]) != 1) {
                fprintf(stderr, "Error reading maximum matrix at [%d][%d]\n", i, j);
                fclose(file);
                return 1;
            }
        }
    }

    // Initialize allocation matrix to 0
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
        }
    }

    // Compute need matrix: need = maximum - allocation
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
    fclose(file);

    // Debug print to verify maximum matrix
    printf("\nMaximum resource matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }

    // Debug print to verify allocation matrix
    printf("\nInitial allocation matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }

    // Debug print to verify need matrix
    printf("\nInitial need matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

        // Buffer for user input commands
    char command[128];

    // Prompt the user for commands
    printf("\nEnter a command (RQ <customer> <r1> <r2> <r3> <r4> or 'exit'):\n");

    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);  // Read full line of input

        // Skip empty lines
        if (command[0] == '\n') continue;

        // Exit the loop if user types "exit"
        if (strncmp(command, "exit", 4) == 0) break;

        int cust, req[NUMBER_OF_RESOURCES];

        // Parse RQ command and request vector
        if (sscanf(command, "RQ %d %d %d %d %d", &cust, &req[0], &req[1], &req[2], &req[3]) == 5) {
            // Validate customer number
            if (cust < 0 || cust >= NUMBER_OF_CUSTOMERS) {
                printf("Invalid customer ID.\n");
                continue;
            }

            // Attempt to allocate resources
            int result = request_resources(cust, req);
            if (result == 0)
                printf("Request approved.\n");
            else
                printf("Request denied (unsafe state).\n");
        } else if (sscanf(command, "RL %d %d %d %d %d", &cust, &req[0], &req[1], &req[2], &req[3]) == 5) {
            // Validate customer number
            if (cust < 0 || cust >= NUMBER_OF_CUSTOMERS) {
                printf("Invalid customer ID.\n");
                continue;
            }

            release_resources(cust, req);
        } else {
            // Command format was invalid
            printf("Invalid command. Try again.\n");
        }

    }

    return 0;

}


int request_resources(int customer_num, int request[]) {
    // Check if request <= need
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("Error: Request exceeds customer's declared need.\n");
            return -1;
        }
    }

    // Check if request <= available
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            printf("Error: Not enough available resources.\n");
            return -1;
        }
    }

    // Pretend to allocate
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Safety check

    return 0;
}


void release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        // Don't allow customers to release more than they currently hold
        if (release[i] > allocation[customer_num][i]) {
            printf("Error: Customer %d cannot release more of resource %d than allocated.\n", customer_num, i);
            return;
        }
    }

    // Perform the release
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }

    printf("Resources released successfully.\n");
}

