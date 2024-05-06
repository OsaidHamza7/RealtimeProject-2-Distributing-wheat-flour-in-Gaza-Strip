#include "header.h"
#include "ipcs.h"
//***********************************************************************************
// Function Prototypes
void getInformation(char **argv, int splitting_worker_num);
void init_signals_handlers();
void signal_handler_REPLACED(int sig);
//***********************************************************************************
int msg_safe_area_id;
int sem_splitted_bags;
int splitting_worker_num;
char *shmptr_splitted_bages;
char *shmptr_splitting_workers;
int num_splitted_bages;
int sem_spaces_available;
int splitting_worker_num;

Container container;
Splitting_Worker *splitting_worker;
Splitting_Worker *splitting_workers;
int num_splitting_workers;
int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 3)
    {
        perror("The user should pass an argument like:splitting_worker_num,num_splitting_workers\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get information from the arguments
    num_splitting_workers = atoi(argv[2]);
    splitting_worker_num = atoi(argv[1]);
    printf("Number of splitting workers is %d\n", num_splitting_workers);

    // open the safe storage area message queues
    msg_safe_area_id = createMessageQueue(MSGQKEY_SAFE_AREA, "splitting_worker.c");

    // Create a shared memory for splitted bages
    shmptr_splitted_bages = createSharedMemory(SHKEY_SPLITTED_BAGS, sizeof(Container), "splitting_worker.c");
    shmptr_splitting_workers = createSharedMemory(SHKEY_SPLITTING_WORKERS, num_splitting_workers * sizeof(Splitting_Worker), "splitting_worker.c"); // Create a shared memory for all struct of the splitting workers
    splitting_workers = (struct Splitting_Worker *)shmptr_splitting_workers;

    getInformation(argv, splitting_worker_num);
    init_signals_handlers();

    sem_splitted_bags = createSemaphore(SEMKEY_SPLITTED_BAGS, 1, 0, "splitting.c");
    sem_spaces_available = createSemaphore(SEMKEY_SPACES_AVAILABLE, 1, 1, "splitting.c");
    while (1)
    { // the splitting worker is waiting for a container from the safe storage area
        printf("Splitting worker %d is waiting for a container from the safe storage area.\n", splitting_worker_num);

        if (msgrcv(msg_safe_area_id, &container, sizeof(struct Container), 0, 0) == -1)
        {
            if (errno == EINTR)
            {
                // printf("Committee:msgrcv interrupted by signal. Retrying...\n");
                continue;
            }
            else
            {
                perror("Splitting:msgrcv");
                break;
            }
        }

        printf("Splitting worker %d received a container %d with bags %d,then he splits it into a number of bags(1k).\n", splitting_worker_num, container.conatiner_num, container.capacity_of_bags);
        // write the number of splitted bages to the shared memory

        acquireSem(sem_spaces_available, 0, "splitting_worker.c");
        printf("Splitting worker %d is acquire spaces\n", splitting_worker_num);
        int y = *shmptr_splitted_bages;
        sleep(1); // Time taken to split the container into the bags
        *shmptr_splitted_bages += container.capacity_of_bags;
        releaseSem(sem_spaces_available, 0, "splitting_worker.c");
        printf("Splitting worker %d is release spaces\n", splitting_worker_num);
        if (y == 0)
        {
            releaseSem(sem_splitted_bags, 0, "splitting_worker.c");
            printf("Splitting worker %d is release avialbe bags\n", splitting_worker_num);
        }
        printf("Splitting worker %d wrote splitted bages to the shared memory,value=%d.\n", splitting_worker_num, *shmptr_splitted_bages);
        sleep(1); // time to take another container to split it
    }
    return 0;
}

void getInformation(char **argv, int splitting_worker_num)
{
    splitting_worker = &splitting_workers[splitting_worker_num - 1];
    splitting_worker->pid = getpid();
    splitting_worker->worker_num = splitting_worker_num;
    splitting_worker->is_replaced = 0;
}

void init_signals_handlers()
{
    // when the signal SIGHUP is received, the worker will be replaced
    if (sigset(SIGCLD, signal_handler_REPLACED) == -1)
    { // set the signal handler for SIGINT
        perror("Signal Error\n");
        exit(-1);
    }
}

void signal_handler_REPLACED(int sig)
{
    printf("The signal %d reached,then the splitting worker %d will replaced\n\n", sig, splitting_worker->worker_num);
    fflush(stdout);
    splitting_worker->is_replaced = 1;
    exit(0);
}