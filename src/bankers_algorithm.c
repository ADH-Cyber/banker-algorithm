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

    /*
    * FUNCTION PROTOTYPES
    */ 
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

        return 0;
    }
