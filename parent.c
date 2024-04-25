#include "header.h"
#include "ipcs.h"
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
void initializeIPCResources();
void exitProgram(int signum);
void createOccupation();
//***********************************************************************************
int arr_of_arguments[MAX_LINES];
int is_alarmed = 0;
pid_t arr_pids_occupation[MAX_NUM_OCUPATIONS];
pid_t arr_pids_planes[MAX_NUM_PLANES];
Plane planes[MAX_NUM_PLANES];
int size = 0;
int i = 0;
int pid = 0;
char range_num_containers[10];
char range_num_bags[10];
char range_dropping_time[10];
char range_refill_planes[10];
int msg_ground;
int msg_safe_area;
int num_occupation = 1;
char *shmptr_plane;
char str_num_cargo_planes[10];
char plane_num[10];

int main(int argc, char **argv)
{
    char *file_name = (char *)malloc(50 * sizeof(char));

    // Start the program
    printf("*******************************************\nStart the program, My process ID is %d\n\n", getpid());

    // check a number of arguments,and read a file name
    checkArguments(argc, argv, file_name);

    // to read from User defined numbers file (filename.txt)
    readFromFile(file_name, arr_of_arguments);

    // get the arguments from the file
    getArguments(arr_of_arguments);
    printArguments();

    // initialize IPCs resources (shared memory, semaphores, message queues)
    initializeIPCResources();

    init_signals_handlers();
    alarm(simulation_threshold_time);

    createPlanes();
    createOccupation();

    while (1)
    {
        pause();
        if (is_alarmed)
        {
            break;
        }
    }
    // exitProgram();
    return 0;
}

void init_signals_handlers()
{
    if (sigset(SIGALRM, exitProgram) == -1)
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
            sprintf(range_dropping_time, "%d %d", period_dropping_wheat_flour_container[0], period_dropping_wheat_flour_container[1]);
            sprintf(range_refill_planes, "%d %d", period_refill_planes[0], period_refill_planes[1]);
            sprintf(str_num_cargo_planes, "%d", num_cargo_planes);

            sprintf(plane_num, "%d", i + 1);

            execlp("./plane", "plane", plane_num, range_num_containers, range_num_bags, range_dropping_time, range_refill_planes, str_num_cargo_planes, NULL);
            perror("Error:Execute plane Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_planes[i] = pid;
            break;
        }
    }
}

void createOccupation()
{
    for (i = 0; i < num_occupation; i++)
    {

        switch (pid = fork())
        {

        case -1: // Fork Failed
            perror("Error:Fork occupation Failed.\n");
            exit(1);
            break;

        case 0: // I'm occupation

            sprintf(str_num_cargo_planes, "%d", num_cargo_planes);

            execlp("./occupation", "occupation", str_num_cargo_planes, NULL);
            perror("Error:Execute occupation Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_occupation[i] = pid;
            break;
        }
    }
}

/*
function to initialize IPCs resources (shared memory, semaphores, message queues)
*/
void initializeIPCResources()
{
    // Create a shared memory for all struct planes
    shmptr_plane = createSharedMemory(SHKEY_PLANES, num_cargo_planes * sizeof(struct Plane), "parent.c");

    // Copy the struct of all planes to the shared memory
    memcpy(shmptr_plane, planes, num_cargo_planes * sizeof(struct Plane));

    // Create a massage queue for the ground
    msg_ground = createMessageQueue(MSGQKEY_GROUND, "parent.c");

    // Create a massage queue for the safe storage area
    msg_safe_area = createMessageQueue(MSGQKEY_SAFE_AREA, "parent.c");
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

void exitProgram(int signum)
{
    is_alarmed = 1;
    printf("The signal %d reached the parent, the program is finished.\n\n", signum);
    fflush(stdout);

    printf("\nKilling all processes...\n");
    fflush(stdout);

    // kill all the planes processes
    killAllProcesses(arr_pids_planes, num_cargo_planes);

    // kill all the ocupations processes
    killAllProcesses(arr_pids_occupation, num_occupation);
    printf("All child processes killed\n");

    printf("Cleaning up IPC resources...\n");
    fflush(stdout);
    // sleep(5);
    //  delete the message queue for the ground
    deleteMessageQueue(msg_ground);

    printf("IPC resources cleaned up successfully\n");
    printf("Exiting...\n");
    fflush(stdout);
    exit(0);
}