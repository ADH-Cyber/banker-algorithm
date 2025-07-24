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

    // Attempt to open the input file
    FILE *file = fopen("src/input.txt", "r");
    if (file == NULL) {
        perror("Error opening input.txt");
        exit(EXIT_FAILURE);
    }

    // Read the maximum resource demand for each customer from the input file
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, "%d", &maximum[i][j]) != 1) {
                fprintf(stderr, "Error: Invalid or insufficient data in input.txt\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            if (j < NUMBER_OF_RESOURCES - 1) {
                fgetc(file); // Skip comma
            }
        }
    }

    fclose(file);

    // Initialize allocation[][] and compute need[][]
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }

    // Debug - print maximum matrix
    printf("\nMaximum matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }

    // Debug - print need matrix
    printf("\nNeed matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    // Input Loop
    char input[100];
    printf("\nEnter commands (RQ/RL/*), or Ctrl+D to exit:\n");

    while (printf("> "), fgets(input, sizeof(input), stdin) != NULL) {
        // Handle request (RQ)
        if (input[0] == 'R' && input[1] == 'Q') {
            int customer_num, request[NUMBER_OF_RESOURCES];
            if (sscanf(input, "RQ %d %d %d %d %d",
                       &customer_num,
                       &request[0], &request[1], &request[2], &request[3]) == 5) {
                if (request_resources(customer_num, request) == 0) {
                    printf("Request granted.\n");
                } else {
                    printf("Request denied (unsafe state).\n");
                }
            } else {
                printf("Invalid RQ command format.\n");
            }
        }

        // Handle release (RL)
        else if (input[0] == 'R' && input[1] == 'L') {
            int customer_num, release[NUMBER_OF_RESOURCES];
            if (sscanf(input, "RL %d %d %d %d %d",
                       &customer_num,
                       &release[0], &release[1], &release[2], &release[3]) == 5) {
                release_resources(customer_num, release);
                printf("Resources released.\n");
            } else {
                printf("Invalid RL command format.\n");
            }
        }

        // Handle wildcard (*)
        else if (input[0] == '*') {
            printf("\nAvailable Resources:\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("R%d ", i);
            }
            printf("\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("%2d ", available[i]);
            }

            printf("\n\nResource State by Customer:\n");
            printf("Cust | Allocated | Maximum   | Need\n");
            printf("-----+-----------+-----------+-----------\n");

            for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
                printf("%d    |", i);
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    printf("%2d", allocation[i][j]);
                }
                printf("   |");
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    printf("%2d", maximum[i][j]);
                }
                printf("   |");
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    printf("%2d", need[i][j]);
                }
                printf("\n");
            }
        }


        else {
            printf("Unknown command.\n");
        }
    }

    printf("Exiting...\n");

    return 0;
}


int is_safe() {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS] = {0};

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    int changed;
    do {
        changed = 0;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                int can_finish = 1;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_finish = 0;
                        break;
                    }
                }
                if (can_finish) {
                    printf("Customer %d can finish. Releasing resources back to work.\n", i);
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = 1;
                    changed = 1;
                } else {
                    printf("Customer %d CANNOT finish. Need: ", i);
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        printf("%d ", need[i][j]);
                    }
                    printf(" | Work: ");
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        printf("%d ", work[j]);
                    }
                    printf("\n");
                }
            }
        }
    } while (changed);

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) return 0;
    }
    return 1;
}


int request_resources(int customer_num, int request[]) {
    printf("Simulating safety check for request: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", request[i]);
    }
    printf("\n");

    // Step 1: Validate request
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("Error: Request exceeds customer's remaining need.\n");
            return -1;
        }
        if (request[i] > available[i]) {
            printf("Error: Request exceeds available resources.\n");
            return -1;
        }
    }

    // Step 2: Tentatively allocate
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Step 3: Safety check
    if (is_safe()) {
        printf("Request granted. New available resources: ");
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            printf("%d ", available[i]);
        }
        printf("\n");
        return 0;
    } else {
        // Roll back
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        printf("Request denied (unsafe state). Current available: ");
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            printf("%d ", available[i]);
        }
        printf("\n");
        return -1;
    }
}


void release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            printf("Error: Cannot release more than allocated. Ignoring release.\n");
            return;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        allocation[customer_num][i] -= release[i];
        available[i] += release[i];
        need[customer_num][i] = maximum[customer_num][i] - allocation[customer_num][i];
    }

    printf("Resources released. New available resources: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    printf("\n");
}
