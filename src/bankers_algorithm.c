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
 *  gcc -o a.out src/bankers_algorithm.c
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
        fprintf(
            stderr, 
            "\033[1;31mError: \033[0m"    
            "Declare four resources for this command."
            "\033[90m\n   ex: %s <R1> <R2> <R3> <R4>\033[0m\n",
            argv[0]
        );
        exit(EXIT_FAILURE);
    }

    // Parse and populate available[] from command-line arguments
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        char *endptr;
        available[i] = strtol(argv[i + 1], &endptr, 10);

        // Checks that all args are integers
        if (*endptr != '\0') {
            fprintf(
                stderr,
                "\033[1;31mError: \033[0m"
                "Invalid character in resource declaration: '%s'\n",
                argv[i + 1]
            );
            exit(EXIT_FAILURE);
        }

        // Checks that all args are non-negative
        if (available[i] < 0) {
            fprintf(
                stderr,
                "\033[1;31mError: \033[0m"
                "Values must be non-negative integers.\n"
            );
            exit(EXIT_FAILURE);
        }
    }

    // Attempt to open the input file
    FILE *file = fopen("src/input.txt", "r");
    if (file == NULL) {
        perror(
            "\033[1;31mError: \033[0m"
            "Cannot open input.txt"
        );
        exit(EXIT_FAILURE);
    }

    // Read the maximum resource demand for each customer from the input file
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, "%d", &maximum[i][j]) != 1) {
                fprintf(
                    stderr, 
                    "\033[1;31mError: \033[0m"
                    "Invalid or insufficient data in input.txt\n"
                );
                fclose(file);
                exit(EXIT_FAILURE);
            }
            // Skip the commas
            if (j < NUMBER_OF_RESOURCES - 1) {
                fgetc(file); 
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

    // Input Menu
    char input[100];
    printf(
        "\033[1;95m"
        "\n ~~ BANKER'S ALGORITHM ~~"
        "\033[0;96m"
        "\n a u s t i n   h o w a r d"
        "\033[92m"
        "\n[======= COMMANDS =======]"
        "\033[90m"
        "\n    RQ: Request resources"
        "\n    RL: Release resources"
        "\n     *: Display resource arrays"
        "\nCtrl+D: Exit program"
        "\033[0m\n"
    );

    // Input Loop
    while (printf("> "), fgets(input, sizeof(input), stdin) != NULL) {
        // Handle request (RQ)
        if (input[0] == 'R' && input[1] == 'Q') {
            int customer_num, request[NUMBER_OF_RESOURCES];
            if (sscanf(input, "RQ %d %d %d %d %d",
                       &customer_num, &request[0], 
                       &request[1], &request[2], 
                       &request[3]) == 5) {
                if (request_resources(customer_num, request) == 0) {
                    printf("\033[1;32mRequest granted.\033[0m\n");
                } else {
                    printf("\033[1;31mRequest denied (unsafe state).\033[0m\n");
                }
            } else {
                printf(
                    "\033[1;31mError: \033[0m"
                    "Invalid RQ command format.\n"
                    "\033[90mex. RQ <customer_index> <R1> <R2> <R3> <R4>\033[0m\n"
                );
            }
        }

        // Handle release (RL)
        else if (input[0] == 'R' && input[1] == 'L') {
            int customer_num, release[NUMBER_OF_RESOURCES];
            if (sscanf(input, "RL %d %d %d %d %d",
                       &customer_num,
                       &release[0], &release[1], &release[2], &release[3]) == 5) {
                release_resources(customer_num, release);
            } else {
                printf(
                    "\033[1;31mError: \033[0m"
                    "Invalid RL command format.\n"
                    "\033[90mex. RL <customer_index> <R1> <R2> <R3> <R4>\033[0m\n"
                );
            }
        }

        // Handle wildcard (*)
        else if (input[0] == '*') {
            printf("\033[1;92m\nAvailable Resources:\033[0m\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("R%d ", i);
            }
            printf("\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("%2d ", available[i]);
            }

            printf(
                "\033[1;92m\n\nResource State by Customer:\033[0m\n"
                "Cust | Allocated | Maximum   | Need\n"
                "-----+-----------+-----------+-----------\n"
                );

            // Populate Table
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
            printf(
                "\033[1;31mError: \033[0m"
                "Unknown command.\n"
                "\033[90mEnter commands (RQ/RL/*/Ctrl+D):\033[0m\n"
            );
        }
    }

    printf("\033[1;92mExiting...\033[0m\n");

    return 0;
}


int is_safe() {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS] = {0};

    // Initialize work with the current available resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    int changed;
    do {
        changed = 0;

        // Attempt to find a customer whose needs can be satisfied
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                int can_finish = 1;
                
                // Check if the customer's need is <= work
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_finish = 0;
                        break;
                    }
                }

                // If the customer can finish, simulate them completing
                if (can_finish) {
                    
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = 1;
                    changed = 1;
                }
            }
        }
    } while (changed);

    // If any customer is not marked finished, system is unsafe
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) return 0;
    }
    return 1;
}


int request_resources(int customer_num, int request[]) {

    // Validate request
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        // Exceeds remaining need
        if (request[i] > need[customer_num][i]) {return -1;}
        // Exceeds available resources
        if (request[i] > available[i]) {return -1;}
    }

    // Tentatively allocate resources per request
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Safety check
    if (is_safe()) {
        return 0;
    } else {
        // Roll back
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }
}


void release_resources(int customer_num, int release[]) {

    // Validate the release does not exceed what was allocated
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            printf(
                "\033[1;31mError: \033[0m"
                "Cannot release more than allocated.\n"
            );
            return;
        }
    }

    // Apply the release as requested
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        allocation[customer_num][i] -= release[i];
        available[i] += release[i];
        need[customer_num][i] = maximum[customer_num][i] - allocation[customer_num][i];
    }

    printf("\033[1;32mResources released.\033[0m\n");
}
