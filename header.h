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
#define NUM_SEMAPHORES 2

#define MAX_NUM_WORKERS 100
#define MAX_NUM_FAMILIES 100
#define MAX_NUM_CONTINARS 20
#define MAX_NUM_PLANES 100
#define MAX_NUM_OCUPATIONS 10
#define MAX_NUM_DISTRIBUTING_WORKERS 100
#define MAX_NUM_WORKERS_IN_COMMITTEE 10
#define MAX_NUM_COLLECTION_COMMITTEES 100
#define MAX_NUM_SPLITTING_WORKERS 100

#define MSGQKEY_GROUND 1111    // key for message queue for the ground
#define MSGQKEY_SAFE_AREA 2222 // key for message queue the safe storage area

#define SHKEY_PLANES 4444                // key for shared memory pid of the planes
#define SHKEY_COLLECTION_COMMITTEES 5555 // key for shared memory pid of the collection committees
#define SHKEY_SPLITTED_BAGS 6666         // key for shared memory pid of the splitting workers
#define SHKEY_DISTRIBUTING_WORKERS 5566  // key for shared memory pid of the distributing workers
#define SHKEY_FAMILIES 3333              // key for shared memory pid of the families

#define SHKEY_THRESHOLD_NUM_CARGO_PLANES_CRASHED 8888
#define SHKEY_THRESHOLD_WHEAT_FLOUR_CONTAINERS_SHOTED 9999
#define SHKEY_THRESHOLD_MARTYRED_COLLECTING_COMMITTEE 1010
#define SHKEY_THRESHOLD_MARTYRED_DISTRIBUTING_WORKERS 1111
#define SHKEY_THRESHOLD_NUM_DECEASED_FAMILIES 1212


#define SEMKEY_PLANES 1166                // key for semaphore for the planes
#define SEMKEY_COLLECTING_COMMITTEES 2255 // key for semaphore for the collection committees
#define SEMKEY_SPLITTED_BAGS 7777         // key for semaphore for the planes
#define SEMKEY_SPACES_AVAILABLE 8888      // key for semaphore for the spaces available in the safe storage area
#define SEMKEY_DISTRIBUTING_WORKERS 5533  // key for semaphore for the distributing workers
#define SEMKEY_STARVATION_FAMILIES 9999

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
    pid_t pid;
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
    int is_martyred;
};
typedef struct Worker Worker;

struct Distributing_Worker
{
    pid_t pid;
    int worker_num;
    int energy;
    int num_bags;
    int trip_time;
    int is_tripping;
    int is_martyred;
};
typedef struct Distributing_Worker Distributing_Worker;

struct Collecting_Committee
{
    pid_t pid;
    int committee_num;
    int num_workers;
    Worker workers[MAX_NUM_WORKERS_IN_COMMITTEE];
    int num_killed_workers;
    int trip_time;
    int is_tripping;
};
typedef struct Collecting_Committee Collecting_Committee;

struct Family
{
    int family_num;
    int starvation_level;
};
typedef struct Family Family;

// ====================================================================================
extern int num_cargo_planes;
extern int range_num_wheat_flour_containers[2];
extern int range_num_bages[2];
extern int period_dropping_wheat_flour_container[2];
extern int period_refill_planes[2];
extern int period_trip_collecting_committees[2];
extern int range_energy_of_workers[2];
extern int period_energy_reduction;
extern int energy_loss_range[2];
extern int num_families;
extern int num_collecting_relief_committees;
extern int num_workers_in_collecting_committee;
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
extern int period_starvation_increase;
extern int range_starvation_increase[2];
extern int range_starvation_decrease[2];
// ====================================================================================
void readArgumentsFromFile(char *filename);
void readFromFile(const char *filename, int *array);
char *trim(char *str);
void killAllProcesses(int *arr_pid, int size);
void split_string(char *argv, int arr[]);
int get_random_number(int min, int max);

#endif
