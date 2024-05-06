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
void signal_handler_SIGCLD(int sig);
void init_signals_handlers();
void createPlanes();
void getArguments(int *numberArray);
void printArguments();
void initializeIPCResources();
void exitProgram();
void createOccupation();
void createCollectingCommittees();
void createSplittingWorkers();
void createDistributingWorkers();
void createFamilies();
void createGUI();
//***********************************************************************************
int arr_of_arguments[MAX_LINES];
int is_alarmed = 0;
int x = 0;
int size = 0;
int i = 0;
int pid = 0;
int number_of_occupations = 3;
int is_end = 0;
int msg_gui;
struct msgbuf msg;

// arrays of pids for all the processes
pid_t arr_pids_occupation[MAX_NUM_OCUPATIONS];
pid_t arr_pids_planes[MAX_NUM_PLANES];
pid_t arr_pids_collecting_committees[MAX_NUM_COLLECTION_COMMITTEES];
pid_t arr_pids_splitting_workers[MAX_NUM_SPLITTING_WORKERS];
pid_t arr_pids_distributing_workers[MAX_NUM_DISTRIBUTING_WORKERS];
pid_t arr_pids_families[MAX_NUM_FAMILIES];

// arrays of structs for all the processes
Plane planes[MAX_NUM_PLANES];
Collecting_Committee collecting_committees[MAX_NUM_COLLECTION_COMMITTEES];
Distributing_Worker distributing_workers[MAX_NUM_DISTRIBUTING_WORKERS];
Family *families;
Splitting_Worker splitting_workers[MAX_NUM_SPLITTING_WORKERS];

// arguments from the file
char range_num_containers[10];
char range_num_bags[10];
char range_dropping_time[10];
char range_refill_planes[10];
char str_period_trip_committees[10];
char str_range_energy_workers[10];
char str_period_energy_reduction[10];
char str_energy_loss[10];
char range_num_bags_distrib_worker[10];
char str_num_cargo_planes[10];
char plane_num[10];
char committee_num[10];
char splitting_worker_num[10];
char distributing_worker_num[10];
char number_of_committees[10];
char number_of_workers[10];
char number_of_families[10];
char range_starv_increase[10];
char range_starv_decrease[10];
char str_period_starvation_increase[10];
char occupation_num[10];
char num_splitting_workers[10];
// IPCs resources

// message queues
int msg_ground;
int msg_safe_area;

// shared memories for struct of processes
char *shmptr_plane;
char *shmptr_collecting_committees;
char *shmptr_splitting_workers;
char *shmptr_splitted_bages;
char *shmptr_distributing_workers;
char *shmptr_families;

// shared memories for thresholds
char *shmptr_threshold_num_cargo_planes_crashed;
char *shmptr_threshold_wheat_flour_containers_shoted;
char *shmptr_threshold_martyred_collecting_committee_workers;
char *shmptr_threshold_martyred_distributing_workers;
char *shmptr_threshold_num_deceased_families;

// semaphores
int sem_splitted_bags;
int sem_spaces_available;
int sem_planes;
int sem_collecting_committees;
int sem_distributing_workers;
int sem_starviation_familes;

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

    //  create the GUI
    createGUI();
    createPlanes();
    //  create the workers
    //  1- collecting relief workers
    createCollectingCommittees();
    //  2- splitting relief workers
    createSplittingWorkers();
    //  3- distributing relief workers
    createDistributingWorkers();
    createFamilies();
    createOccupation();

    while (1)
    {
        pause();
        is_end = 0;
        if (is_alarmed)
        {
            printf("The trishold time is reached, the program is finished.\n\n");
            is_end = 1;
        }
        if (threshold_num_cargo_planes_crashed <= *shmptr_threshold_num_cargo_planes_crashed)
        {
            printf("The number of crashed planes reached the threshold, the program is finished.\n\n");
            is_end = 1;
        }
        if (threshold_wheat_flour_containers_shoted <= *shmptr_threshold_wheat_flour_containers_shoted)
        {
            printf("The number of shoted wheat flour containers reached the threshold, the program is finished.\n\n");
            is_end = 1;
        }
        if (threshold_martyred_collecting_committee_workers <= *shmptr_threshold_martyred_collecting_committee_workers)
        {
            printf("The number of martyred collecting committees workers reached the threshold, the program is finished.\n\n");
            is_end = 1;
        }
        if (threshold_martyred_distributing_workers <= *shmptr_threshold_martyred_distributing_workers)
        {
            printf("The number of martyred distributing workers reached the threshold, the program is finished.\n\n");
            is_end = 1;
        }
        if (threshold_num_deceased_families <= *shmptr_threshold_num_deceased_families)
        {
            printf("The number of deceased families reached the threshold, the program is finished.\n\n");
            is_end = 1;
        }

        if (is_end)
        {
            break;
        }
    }

    exitProgram();

    return 0;
}

void init_signals_handlers()
{
    if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    { // set the signal handler for SIGALRM
        perror("Signal Error\n");
        exit(-1);
    }
    if (sigset(SIGCLD, signal_handler_SIGCLD) == -1)
    { // set the signal handler for SIGALRM
        perror("Signal Error\n");
        exit(-1);
    }
}

// function signal_handler_SIGALRM
void signal_handler_SIGALRM(int sig)
{
    is_alarmed = 1;
    printf("The signal %d reached the parent\n\n", sig);
    fflush(stdout);
}
void signal_handler_SIGCLD(int sig)
{
    printf("The signal %d reached the parent\n\n", sig);
    fflush(stdout);
}
void createPlanes()
{

    for (i = 0; i < num_cargo_planes; i++)
    {
        switch (pid = fork())
        {

        case -1: // Fork Failed
            perror("Error:Fork Plane Failed.\n");
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
    for (i = 0; i < number_of_occupations; i++)
    {

        switch (pid = fork())
        {

        case -1: // Fork Failed
            perror("Error:Fork Occupation Failed.\n");
            exit(1);
            break;

        case 0: // I'm occupation

            sprintf(str_num_cargo_planes, "%d", num_cargo_planes);
            sprintf(number_of_committees, "%d", num_collecting_relief_committees);
            sprintf(number_of_workers, "%d", num_distributing_relief_workers);
            sprintf(occupation_num, "%d", i + 1);

            msg.mtype = 4;
            msg.x = 3;
            msg.y = 3;
            // send the message to gui to print the container
            if (msgsnd(msg_gui, &msg, sizeof(msg), 0) == -1)
            {
                perror("p tryarent:msgsnd");
                return;
            }
            execlp("./occupation", "occupation", str_num_cargo_planes, number_of_committees, number_of_workers, occupation_num, NULL);
            perror("Error:Execute occupation Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_occupation[i] = pid;
            break;
        }
    }
}

// created the workers
void createCollectingCommittees()
{
    printf("*******************************************************************************\n");
    for (i = 0; i < num_collecting_relief_committees; i++)
    {
        switch (pid = fork())
        {
        case -1: // Fork Failed
            perror("Error:Fork Committee Failed.\n");
            exit(1);
            break;

        case 0: // I'm committee
            sprintf(str_period_trip_committees, "%d %d", period_trip_collecting_committees[0], period_trip_collecting_committees[1]);
            sprintf(number_of_workers, "%d", num_workers_in_collecting_committee);
            sprintf(str_range_energy_workers, "%d %d", range_energy_of_workers[0], range_energy_of_workers[1]);
            sprintf(str_period_energy_reduction, "%d", period_energy_reduction);
            sprintf(str_energy_loss, "%d %d", energy_loss_range[0], energy_loss_range[1]);
            sprintf(number_of_committees, "%d", num_collecting_relief_committees);
            sprintf(committee_num, "%d", i + 1);
            sprintf(num_splitting_workers, "%d", num_splitting_relief_workers);

            msg.mtype = 1;
            msg.x = 1;
            msg.y = 2;

            // send the message to gui
            if (msgsnd(msg_gui, &msg, sizeof(msg), 0) == -1)
            {
                perror("p tryarent:msgsnd");
                return;
            }
            execlp("./collecting_committe", "collecting_committe", committee_num, number_of_workers, str_period_trip_committees, str_range_energy_workers, str_period_energy_reduction, str_energy_loss, number_of_committees, num_splitting_workers, NULL);
            perror("Error:Execute committee Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_collecting_committees[i] = pid;
            break;
        }
    }
    printf("*******************************************************************************\n");
}

// created the workers
void createSplittingWorkers()
{
    printf("*******************************************************************************\n");
    for (i = 0; i < num_splitting_relief_workers; i++)
    {
        switch (pid = fork())
        {
        case -1: // Fork Failed
            perror("Error:Fork Splitting Committee Failed.\n");
            exit(1);
            break;

        case 0: // I'm splitting worker

            sprintf(splitting_worker_num, "%d", i + 1);
            sprintf(num_splitting_workers, "%d", num_splitting_relief_workers);

            msg.mtype = 2;
            msg.x = 1;
            msg.y = 1;

            // send the message to gui to print the container
            if (msgsnd(msg_gui, &msg, sizeof(msg), 0) == -1)
            {
                perror("p tryarent:msgsnd");
                return;
            }
            execlp("./splitting_worker", "splitting_worker", splitting_worker_num, num_splitting_workers, NULL);
            perror("Error:Execute spltting worker Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_splitting_workers[i] = pid;
            break;
        }
    }
    printf("*******************************************************************************\n");
}

// created the workers
void createDistributingWorkers()
{
    printf("*******************************************************************************\n");
    for (i = 0; i < num_distributing_relief_workers; i++)
    {
        switch (pid = fork())
        {
        case -1: // Fork Failed
            perror("Error:Fork Distributing Worker Failed.\n");
            exit(1);
            break;

        case 0: // I'm splitting worker
            sprintf(str_period_trip_committees, "%d %d", period_trip_collecting_committees[0], period_trip_collecting_committees[1]);
            sprintf(number_of_workers, "%d", num_distributing_relief_workers);
            sprintf(str_range_energy_workers, "%d %d", range_energy_of_workers[0], range_energy_of_workers[1]);
            sprintf(str_period_energy_reduction, "%d", period_energy_reduction);
            sprintf(str_energy_loss, "%d %d", energy_loss_range[0], energy_loss_range[1]);
            sprintf(distributing_worker_num, "%d", i + 1);
            sprintf(range_num_bags_distrib_worker, "%d %d", range_bags_per_distrib_worker[0], range_bags_per_distrib_worker[1]);
            sprintf(number_of_families, "%d", num_families);
            sprintf(range_starv_decrease, " %d %d", range_starvation_decrease[0], range_starvation_decrease[1]);
            sprintf(num_splitting_workers, "%d", num_splitting_relief_workers);

            msg.mtype = 3;
            msg.x = 2;
            msg.y = 2;
            // send the message to gui to print the container
            if (msgsnd(msg_gui, &msg, sizeof(msg), 0) == -1)
            {
                perror("p tryarent:msgsnd");
                return;
            }

            execlp("./distributing_worker", "distributing_worker", distributing_worker_num, range_num_bags_distrib_worker, str_period_trip_committees, str_range_energy_workers, str_period_energy_reduction, str_energy_loss, number_of_workers, number_of_families, range_starv_decrease, num_splitting_workers, NULL);
            perror("Error:Execute  Distributing worker Failed.\n");
            exit(1);
            break;

        default: // I'm parent
            arr_pids_distributing_workers[i] = pid;
            break;
        }
    }
    printf("*******************************************************************************\n");
}

void createFamilies()
{

    switch (pid = fork())
    {
    case -1: // Fork Failed
        perror("Error:Fork Family Failed.\n");
        exit(1);
        break;

    case 0: // I'm family

        sprintf(number_of_families, "%d", num_families);
        sprintf(range_starv_increase, " %d %d", range_starvation_increase[0], range_starvation_increase[1]);
        sprintf(str_period_starvation_increase, "%d", period_starvation_increase);

        execlp("./family", "family", number_of_families, str_period_starvation_increase, range_starv_increase, NULL);
        perror("Error:Execute family Failed.\n");
        exit(1);
        break;

    default: // I'm parent
        arr_pids_families[0] = pid;
        break;
    }
}
/*
function to create gui process
*/
void createGUI()
{
    switch (pid = fork())
    {
    case -1: // Fork Failed
        perror("Error:Fork GUI Failed.\n");
        exit(1);
        break;

    case 0: // I'm GUI
        execlp("./gui", "gui", NULL);
        perror("Error:Execute GUI Failed.\n");
        exit(1);
        break;

    default: // I'm parent
        break;
    }
}
/*
function to initialize IPCs resources (shared memory, semaphores, message queues)
*/
void initializeIPCResources()
{
    // Create a Massage Queues (2 queues done)
    msg_ground = createMessageQueue(MSGQKEY_GROUND, "parent.c");       // Create a massage queue for the ground
    msg_safe_area = createMessageQueue(MSGQKEY_SAFE_AREA, "parent.c"); // Create a massage queue for the safe storage area

    // Create a Shared Memories for struct of processes (4 shared memories done)
    shmptr_plane = createSharedMemory(SHKEY_PLANES, num_cargo_planes * sizeof(struct Plane), "parent.c");                                                        // Create a shared memory for all struct planes
    shmptr_collecting_committees = createSharedMemory(SHKEY_COLLECTION_COMMITTEES, num_collecting_relief_committees * sizeof(Collecting_Committee), "parent.c"); // Create a shared memory for all struct of the collecting committees
    shmptr_splitting_workers = createSharedMemory(SHKEY_SPLITTING_WORKERS, num_splitting_relief_workers * sizeof(Splitting_Worker), "parent.c");                 // Create a shared memory for all struct of the splitting workers
    shmptr_splitted_bages = createSharedMemory(SHKEY_SPLITTED_BAGS, sizeof(Container), "parent.c");                                                              // Create a shared memory for splitted bages
    shmptr_distributing_workers = createSharedMemory(SHKEY_DISTRIBUTING_WORKERS, num_distributing_relief_workers * sizeof(Distributing_Worker), "parent.c");
    shmptr_families = createSharedMemory(SHKEY_FAMILIES, num_families * sizeof(struct Family), "parent.c");

    // Create a Shared Memories for thresholds (5 shared memories done)
    shmptr_threshold_num_cargo_planes_crashed = createSharedMemory(SHKEY_THRESHOLD_NUM_CARGO_PLANES_CRASHED, sizeof(int), "parent.c");
    shmptr_threshold_wheat_flour_containers_shoted = createSharedMemory(SHKEY_THRESHOLD_WHEAT_FLOUR_CONTAINERS_SHOTED, sizeof(int), "parent.c");
    shmptr_threshold_martyred_collecting_committee_workers = createSharedMemory(SHKEY_THRESHOLD_MARTYRED_COLLECTING_COMMITTEE, sizeof(int), "parent.c");
    shmptr_threshold_martyred_distributing_workers = createSharedMemory(SHKEY_THRESHOLD_MARTYRED_DISTRIBUTING_WORKERS, sizeof(int), "parent.c");
    shmptr_threshold_num_deceased_families = createSharedMemory(SHKEY_THRESHOLD_NUM_DECEASED_FAMILIES, sizeof(int), "parent.c");

    // Copy the the shared memories
    memcpy(shmptr_plane, planes, num_cargo_planes * sizeof(struct Plane));                                                               // Copy the struct of all planes to the shared memory
    memcpy(shmptr_collecting_committees, collecting_committees, num_collecting_relief_committees * sizeof(struct Collecting_Committee)); // Copy the struct of all planes to the shared memory
    memcpy(shmptr_splitting_workers, splitting_workers, num_splitting_relief_workers * sizeof(struct Splitting_Worker));                 // Copy the struct of all planes to the shared memory
    memcpy(shmptr_splitted_bages, &x, sizeof(Container));                                                                                // Copy the struct of all planes to the shared memory
    memcpy(shmptr_distributing_workers, distributing_workers, num_distributing_relief_workers * sizeof(struct Distributing_Worker));     // Copy the struct of all planes to the shared memory

    // Copy the the shared memories for thresholds
    memcpy(shmptr_threshold_num_cargo_planes_crashed, &x, sizeof(int));
    memcpy(shmptr_threshold_wheat_flour_containers_shoted, &x, sizeof(int));
    memcpy(shmptr_threshold_martyred_collecting_committee_workers, &x, sizeof(int));
    memcpy(shmptr_threshold_martyred_distributing_workers, &x, sizeof(int));
    memcpy(shmptr_threshold_num_deceased_families, &x, sizeof(int));

    // Create a Semaphores
    sem_planes = createSemaphore(SEMKEY_PLANES, 1, 1, "parent.c");
    sem_collecting_committees = createSemaphore(SEMKEY_COLLECTING_COMMITTEES, 1, 1, "parent.c");
    sem_splitted_bags = createSemaphore(SEMKEY_SPLITTED_BAGS, 1, 0, "parent.c");
    sem_spaces_available = createSemaphore(SEMKEY_SPACES_AVAILABLE, 1, 1, "parent.c");
    sem_distributing_workers = createSemaphore(SEMKEY_DISTRIBUTING_WORKERS, 1, 1, "parent.c");
    sem_starviation_familes = createSemaphore(SEMKEY_STARVATION_FAMILIES, 1, 1, "parent.c");
}

void getArguments(int *numberArray)
{
    num_cargo_planes = numberArray[0];
    period_energy_reduction = numberArray[1];
    num_families = numberArray[2];
    num_collecting_relief_committees = numberArray[3];
    num_workers_in_collecting_committee = numberArray[4];
    num_splitting_relief_workers = numberArray[5];
    num_distributing_relief_workers = numberArray[6];
    distrib_relief_worker_threshold = numberArray[7];
    simulation_threshold_time = numberArray[8];
    threshold_families_death_rate = numberArray[9];
    threshold_num_cargo_planes_crashed = numberArray[10];
    threshold_wheat_flour_containers_shoted = numberArray[11];
    threshold_martyred_collecting_committee_workers = numberArray[12];
    threshold_martyred_distributing_workers = numberArray[13];
    threshold_num_deceased_families = numberArray[14];
    period_starvation_increase = numberArray[15];
}

void printArguments()
{
    // print the arguments read from the file
    printf("num_cargo_planes = %d\n", num_cargo_planes);
    printf("range_num_wheat_flour_containers = %d %d\n", range_num_wheat_flour_containers[0], range_num_wheat_flour_containers[1]);
    printf("range_num_bages = %d %d\n", range_num_bages[0], range_num_bages[1]);
    printf("period_dropping_wheat_flour_container = %d %d\n", period_dropping_wheat_flour_container[0], period_dropping_wheat_flour_container[1]);
    printf("period_refill_planes = %d %d\n", period_refill_planes[0], period_refill_planes[1]);
    printf("period_trip_collecting_committees = %d %d\n", period_trip_collecting_committees[0], period_trip_collecting_committees[1]);
    printf("range_energy_of_workers = %d %d\n", range_energy_of_workers[0], range_energy_of_workers[1]);
    printf("period_energy_reduction = %d\n", period_energy_reduction);
    printf("energy_loss_range = %d %d\n", energy_loss_range[0], energy_loss_range[1]);
    printf("num_families = %d\n", num_families);
    printf("num_collecting_relief_committees = %d\n", num_collecting_relief_committees);
    printf("num_workers_in_collecting_committee = %d\n", num_workers_in_collecting_committee);
    printf("num_splitting_relief_workers = %d\n", num_splitting_relief_workers);
    printf("num_distributing_relief_workers = %d\n", num_distributing_relief_workers);
    printf("range_bags_per_distrib_worker = %d %d\n", range_bags_per_distrib_worker[0], range_bags_per_distrib_worker[1]);
    printf("distrib_relief_worker_threshold = %d\n", distrib_relief_worker_threshold);
    printf("simulation_threshold_time = %d\n", simulation_threshold_time);
    printf("threshold_families_death_rate = %d\n", threshold_families_death_rate);
    printf("threshold_num_cargo_planes_crashed = %d\n", threshold_num_cargo_planes_crashed);
    printf("threshold_wheat_flour_containers_shoted = %d\n", threshold_wheat_flour_containers_shoted);
    printf("threshold_martyred_collecting_committee = %d\n", threshold_martyred_collecting_committee_workers);
    printf("threshold_martyred_distributing_workers = %d\n", threshold_martyred_distributing_workers);
    printf("threshold_num_deceased_families = %d\n", threshold_num_deceased_families);
    printf("period_starvation_increase = %d\n", period_starvation_increase);
    printf("range_starvation_increase = %d %d\n", range_starvation_increase[0], range_starvation_increase[1]);
    printf("range_starvation_decrease = %d %d\n", range_starvation_decrease[0], range_starvation_decrease[1]);
    printf("\n");
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

void exitProgram()
{

    printf("\nKilling all processes...\n");
    fflush(stdout);

    // kill all the child processes
    killAllProcesses(arr_pids_planes, num_cargo_planes);
    killAllProcesses(arr_pids_occupation, number_of_occupations);
    killAllProcesses(arr_pids_collecting_committees, num_collecting_relief_committees);
    killAllProcesses(arr_pids_distributing_workers, num_distributing_relief_workers);
    killAllProcesses(arr_pids_splitting_workers, num_splitting_relief_workers);
    killAllProcesses(arr_pids_families, 1);
    printf("All child processes killed\n");

    printf("Cleaning up IPC resources...\n");
    fflush(stdout);

    deleteMessageQueue(msg_ground);
    deleteMessageQueue(msg_safe_area);

    deleteSharedMemory(SHKEY_PLANES, num_cargo_planes * sizeof(struct Plane), shmptr_plane);
    deleteSharedMemory(SHKEY_COLLECTION_COMMITTEES, num_collecting_relief_committees * sizeof(struct Collecting_Committee), shmptr_collecting_committees);
    deleteSharedMemory(SHKEY_SPLITTED_BAGS, sizeof(Container), shmptr_splitted_bages);
    deleteSharedMemory(SHKEY_DISTRIBUTING_WORKERS, num_distributing_relief_workers * sizeof(struct Distributing_Worker), shmptr_distributing_workers);
    deleteSharedMemory(SHKEY_FAMILIES, num_families * sizeof(struct Family), shmptr_families);

    deleteSemaphore(sem_splitted_bags);
    deleteSemaphore(sem_spaces_available);
    deleteSemaphore(sem_planes);
    deleteSemaphore(sem_collecting_committees);
    deleteSemaphore(sem_distributing_workers);
    deleteSemaphore(sem_starviation_familes);

    printf("IPC resources cleaned up successfully\n");
    printf("Exiting...\n");
    fflush(stdout);
    exit(0);
}
