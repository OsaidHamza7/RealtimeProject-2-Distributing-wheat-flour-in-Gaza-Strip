#ifndef LIBRARIES
#define LIBRARIES

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>

#define FILE_NAME "arguments.txt"
#define GUIFIFO "/tmp/GUIFIFO"
#define MAX_LINES 100
#define MAX_LINE_LENGTH 255
#define MAX_NUM_PROCESSES 100

struct Container
{
    int num;
    int num_bags;
    int dropping_time;
    int status; // 0: not crashed, 1: crashed
};

typedef struct Container Container;

// ====================================================================================
extern int num_cargo_planes;
extern int range_num_wheat_flour_containers[2];
extern int range_num_bages[2];
extern int period_dropping_wheat_flour_container[2];
extern int period_refill_planes[2];
extern int num_families;
extern int num_collecting_relief_committees;
extern int num_workers_in_collecting_committe;
extern int num_splitting_relief_workers;
extern int num_distributing_relief_workers;
extern int range_bags_per_distrib_worker[2];
extern int distrib_relief_worker_threshold;
extern int simulation_threshold_time;
extern int threshold_families_death_rate;
extern int threshold_num_cargo_planes_crashed;
extern int threshold_wheat_flour_containers_shoted;
extern int threshold_martyred_collecting_committee;
extern int threshold_martyred_distributing_workers;
extern int threshold_num_deceased_families;
// ====================================================================================
void readArgumentsFromFile(char *filename);
void readFromFile(const char *filename, int *array);
char *trim(char *str);
void killAllProcesses(int *arr_pid, int size);
void split_string(char *argv, int arr[]);
int get_random_number(int min, int max);
#endif
