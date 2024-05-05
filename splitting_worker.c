#include "header.h"
#include "ipcs.h"
//***********************************************************************************

//***********************************************************************************
int msg_safe_area_id;
int sem_splitted_bags;
int splitting_worker_num;
char *shmptr_splitted_bages;
int num_splitted_bages;
int sem_spaces_available;

Container container;
int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 2)
    {
        perror("The user shouldn pass an argument like:splitting_worker_num,\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get information from the arguments
    int splitting_worker_num = atoi(argv[1]);

    // open the safe storage area message queues
    msg_safe_area_id = createMessageQueue(MSGQKEY_SAFE_AREA, "splitting_worker.c");

    // Create a shared memory for splitted bages
    shmptr_splitted_bages = createSharedMemory(SHKEY_SPLITTED_BAGS, sizeof(Container), "splitting_worker.c");

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
