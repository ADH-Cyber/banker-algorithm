/*
 * File: bankers_algorithm.c
 * Author: Austin Howard
 * Date: 2025-07-27
 * 
 * Description:
 *  Simulates the Banker’s Algorithm for deadlock avoidance by managing multiple customer 
 *  requests and releases for shared resources. The program checks for safe states using 
 *  the Safety Algorithm and prevents allocations that could lead to deadlock.
 * 
 * Course: 
 *  CS7343 - Operating Systems
 *  Dr. Maya El Dayeh
 *  Southern Methodist University
 * 
 * Compile using:
 *  gcc -o banker bankers_algorithm.c
*/