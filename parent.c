#include "header.h"
/*
  Osaid Hamza - Team Leader
  Razan Abdelrahman
  Maisam Alaa
  Ansam Rihan
*/

//***********************************************************************************
void checkArguments(int argc, char **argv, char *file_name);
void signal_handler_SIGALRM(int sig);
void init_signals_handlers();
void createPlanes();
void getArguments(int *numberArray);
void printArguments();
//***********************************************************************************
int numberArray[MAX_LINES];
int is_alarmed = 0;
int arr_pids[MAX_NUM_PROCESSES];
int size = 0;
int i = 0;
int pid = 0;
char range_num_containers[50];
char range_num_bags[50];
char dropping_time[50];
int main(int argc, char **argv)
{
    char *file_name = (char *)malloc(50 * sizeof(char));

    // Start the program
    printf("*******************************************\nStart the program, My process ID is %d\n\n", getpid());

    // check a number of arguments,and read a file name
    checkArguments(argc, argv, file_name);

    // to read from User defined numbers file (filename.txt)
    readFromFile(file_name, numberArray);

    // get the arguments from the file
    getArguments(numberArray);
    printArguments();

    init_signals_handlers();

    alarm(simulation_threshold_time);
    createPlanes();

    while (1)
    {
        pause();
        if (is_alarmed)
        {
            break;
        }
    }

    killAllProcesses(arr_pids, num_cargo_planes);
    return 0;
}

void init_signals_handlers()
{
    if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    { // set the signal handler for SIGALRM
        perror("Signal Error\n");
        exit(-1);
    }
}

// function signal_handler_SIGALRM
void signal_handler_SIGALRM(int sig)
{
    is_alarmed = 1;
    printf("The signal %d reached the parent, the program is finished.\n\n", sig);
    fflush(stdout);
}

void createPlanes()
{

    for (i = 0; i < num_cargo_planes; i++)
    {
        switch (pid = fork())
        {

        case -1: // Fork Failed
            perror("Error:Fork Failed.\n");
            exit(1);
            break;

        case 0: // I'm plane
            sprintf(range_num_containers, "%d %d", range_num_wheat_flour_containers[0], range_num_wheat_flour_containers[1]);
            sprintf(range_num_bags, "%d %d", range_num_bages[0], range_num_bages[1]);
            sprintf(dropping_time, "%d %d", period_dropping_wheat_flour_container[0], period_dropping_wheat_flour_container[1]);

            execlp("./plane", "plane", range_num_containers, range_num_bags, dropping_time, NULL);
            perror("Error:Execute plane Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids[i] = pid;
            break;
        }
    }
}

void getArguments(int *numberArray)
{
    num_cargo_planes = numberArray[0];
    num_families = numberArray[1];
    num_collecting_relief_committees = numberArray[2];
    num_workers_in_collecting_committe = numberArray[3];
    num_splitting_relief_workers = numberArray[4];
    num_distributing_relief_workers = numberArray[5];
    distrib_relief_worker_threshold = numberArray[6];
    simulation_threshold_time = numberArray[7];
    threshold_families_death_rate = numberArray[8];
    threshold_num_cargo_planes_crashed = numberArray[9];
    threshold_wheat_flour_containers_shoted = numberArray[10];
    threshold_martyred_collecting_committee = numberArray[11];
    threshold_martyred_distributing_workers = numberArray[12];
    threshold_num_deceased_families = numberArray[13];
}

void printArguments()
{
    // print the arguments read from the file
    printf("num_cargo_planes = %d\n", num_cargo_planes);
    printf("range_num_wheat_flour_containers = %d %d\n", range_num_wheat_flour_containers[0], range_num_wheat_flour_containers[1]);
    printf("range_num_bages = %d %d\n", range_num_bages[0], range_num_bages[1]);
    printf("period_dropping_wheat_flour_container = %d %d\n", period_dropping_wheat_flour_container[0], period_dropping_wheat_flour_container[1]);
    printf("period_refill_planes = %d %d\n", period_refill_planes[0], period_refill_planes[1]);
    printf("num_families = %d\n", num_families);
    printf("num_collecting_relief_committees = %d\n", num_collecting_relief_committees);
    printf("num_workers_in_collecting_committe = %d\n", num_workers_in_collecting_committe);
    printf("num_splitting_relief_workers = %d\n", num_splitting_relief_workers);
    printf("num_distributing_relief_workers = %d\n", num_distributing_relief_workers);
    printf("range_bags_per_distrib_worker = %d %d\n", range_bags_per_distrib_worker[0], range_bags_per_distrib_worker[1]);
    printf("distrib_relief_worker_threshold = %d\n", distrib_relief_worker_threshold);
    printf("simulation_threshold_time = %d\n", simulation_threshold_time);
    printf("threshold_families_death_rate = %d\n", threshold_families_death_rate);
    printf("threshold_num_cargo_planes_crashed = %d\n", threshold_num_cargo_planes_crashed);
    printf("threshold_wheat_flour_containers_shoted = %d\n", threshold_wheat_flour_containers_shoted);
    printf("threshold_martyred_collecting_committee = %d\n", threshold_martyred_collecting_committee);
    printf("threshold_martyred_distributing_workers = %d\n", threshold_martyred_distributing_workers);
    printf("threshold_num_deceased_families = %d\n", threshold_num_deceased_families);
}
// function checkArguments
void checkArguments(int argc, char **argv, char *file_name)
{
    if (argc != 2) // check if the user passed the correct arguments
    {
        printf("Usage: Invalid arguments.\n"); // Use the default file names
        printf("Using default file names: arguments.txt\n");
        strcpy(file_name, FILE_NAME);
    }
    else
    {
        strcpy(file_name, argv[1]); // Use the file names provided by the user
    }
}