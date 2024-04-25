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
#define MAX_NUM_CONTINARS 20
#define MAX_NUM_PLANES 100
#define MAX_NUM_OCUPATIONS 10
#define MAX_NUM_WORKERS_IN_COMMITTEE 10

#define MSGQKEY_GROUND 1111    // key for message queue for the ground
#define MSGQKEY_SAFE_AREA 2222 // key for message queue the safe storage area

#define SHKEY_FAMILIES 3333 // key for shared memory pid of the families
#define SHKEY_PLANES 4444   // key for shared memory pid of the planes

struct String
{
    char str[MAX_LINE_LENGTH];
};

struct Container
{
    int conatiner_num;
    int capacity_of_bags;
    int dropping_time;
};
typedef struct Container Container;

struct Plane
{
    int pid;
    int plane_num;
    int num_containers;
    Container containers[MAX_NUM_CONTINARS];
    int is_refilling;
};
typedef struct Plane Plane;



struct Worker
{
    int worker_num;
    int energy;
    int trip_time;
    int is_tripping;
};
typedef struct Worker Worker;

struct Collecting_Committee
{
    int committee_num;
    int num_workers;
    Worker workers[MAX_NUM_WORKERS_IN_COMMITTEE];
};
typedef struct Collecting_Committee Collecting_Committee;
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
