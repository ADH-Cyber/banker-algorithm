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
 *  gcc -o banker bankers_algorithm.c
*/

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