#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void init_signals_handlers();
void signal_handler_MARTYRED(int sig);
void signal_handler_SIGALRM(int sig);
void signal_handler_SIGINT(int sig);
int apply_trip_time();
void get_information_worker(char **argv, int worker_num);
void takeBagsFromShMem();
void distributeBagsToFamillies();
Family *sort_families();
int compareFamilies(const void *a, const void *b);
//***********************************************************************************

char *shmptr_distributing_workers;
char *shmptr_splitted_bages;
int sem_splitted_bags;
int sem_spaces_available;

int period_trip_worker[2];
int range_energy_workers[2];
int periodic_energy_reduction;
int range_energy_loss[2];
int range_num_bags_distrib_worker[2];

int is_alarmed;
int is_worker_dead;
int distributing_worker_num;

Distributing_Worker *distributing_worker;
Distributing_Worker *distributing_workers;
Distributing_Worker *temp;
char *shmptr_families;
Family *families;
int sem_starviation_familes;
int number_of_families;
int number_of_workers;
int range_starv_decrease[2];
Family *sorted_families;

int main(int argc, char **argv)

{
    // check the number of arguments
    if (argc < 10)
    {
        perror("The user should pass an argument like:distributing_worker_num,range_num_bags, period_trip_workers, range_energy_workers, period_energy_reduction, energy_loss_range,number_of_workers,num_familiesint range_starv_decrease\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get the arguments
    distributing_worker_num = atoi(argv[1]);
    number_of_workers = atoi(argv[7]);
    number_of_families = atoi(argv[8]);

    shmptr_distributing_workers = createSharedMemory(SHKEY_DISTRIBUTING_WORKERS, number_of_workers * sizeof(struct Distributing_Worker), "distributing_worker.c");
    distributing_workers = (struct Distributing_Worker *)shmptr_distributing_workers;

    // Create a shared memory for splitted bages
    shmptr_splitted_bages = createSharedMemory(SHKEY_SPLITTING_WORKERS, sizeof(Container), "distributing_worker.c");

    //  open the shared memory of all families
    shmptr_families = createSharedMemory(SHKEY_FAMILIES, number_of_families * sizeof(struct Family), "distributing_worker.c");
    families = (struct Family *)shmptr_families;

    sem_starviation_familes = createSemaphore(SEMKEY_STARVATION_FAMILIES, 1, 1, "distributing_wroker.c");

    sem_splitted_bags = createSemaphore(SEMKEY_SPLITTED_BAGS, 1, 0, "distributing_wroker.c");
    sem_spaces_available = createSemaphore(SEMKEY_SPACES_AVAILABLE, 1, 1, "distributing_wroker.c");

    get_information_worker(argv, distributing_worker_num);
    init_signals_handlers();

    while (1)
    {
        printf("Disttributing worker %d with %d bags is waiting for a bags from the splitting\n", distributing_worker->worker_num, distributing_worker->num_bags);
        distributing_worker->is_tripping = 0;
        takeBagsFromShMem();
        distributeBagsToFamillies();
    }

    return 0;
}

void init_signals_handlers()
{
    // when the signal SIGHUP is received, the worker will be martyred
    if (sigset(SIGHUP, signal_handler_MARTYRED) == -1)
    { // set the signal handler for SIGINT
        perror("Signal Error\n");
        exit(-1);
    }
    // SIGALARM is used for changing the energy of the worker every specific time
    if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    {
        perror("Signal Error\n");
        exit(-1);
    }
    if (sigset(SIGINT, signal_handler_SIGINT) == -1)
    {
        perror("Signal Error\n");
        exit(-1);
    }
}

// function SIGHUB
void signal_handler_MARTYRED(int sig)
{
    printf("The signal %d reached,then the distributing worker %d is killed\n\n", sig, distributing_worker->worker_num);
    fflush(stdout);
}

void signal_handler_SIGALRM(int sig)
{
    /*printf("The alarm signal %d reached to the distributing worker:%d then the energy reduced\n\n", sig, distributing_worker->worker_num);
    is_alarmed = 1;
    for (int i = 0; i < collecting_committee->num_workers; i++)
    {
        if (collecting_committee->workers[i].energy >= 5)
        {
            collecting_committee->workers[i].energy -= get_random_number(range_energy_loss[0], range_energy_loss[1]);
            printf("Worker %d in committee %d has energy %d\n", collecting_committee->workers[i].worker_num, collecting_committee->committee_num, collecting_committee->workers[i].energy);
        }
    }
    alarm(periodic_energy_reduction);*/
}

void signal_handler_SIGINT(int sig)
{
    printf("The signal %d reached,distributing worker %d will take number of bags\n\n", sig, distributing_worker->worker_num);
    fflush(stdout);
}

int apply_trip_time()
{
    int pause_time = sleep(distributing_worker->trip_time);

    while (pause_time != 0)
    {
        if (is_alarmed == 1)
        {
            is_alarmed = 0;
        }
        else if (is_worker_dead == 1)
        {
            is_worker_dead = 0;
        }
        // intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
        pause_time = sleep(pause_time);
    }
    return 1;
}

void get_information_worker(char **argv, int distributing_worker_num)
{
    // get the information of the committee from the arguments
    distributing_worker = &distributing_workers[distributing_worker_num - 1];
    temp = distributing_worker;

    split_string(argv[2], range_num_bags_distrib_worker);
    split_string(argv[3], period_trip_worker);
    split_string(argv[4], range_energy_workers);
    periodic_energy_reduction = atoi(argv[5]);
    split_string(argv[6], range_energy_loss);
    split_string(argv[9], range_starv_decrease);

    distributing_worker->worker_num = distributing_worker_num;
    distributing_worker->energy = get_random_number(range_energy_workers[0], range_energy_workers[1]);
    distributing_worker->num_bags = get_random_number(range_num_bags_distrib_worker[0], range_num_bags_distrib_worker[1]);
    distributing_worker->trip_time = get_random_number(period_trip_worker[0], period_trip_worker[1]);
    distributing_worker->is_tripping = 0;

    printf("Distributing worker %d has energy:%d\n", distributing_worker->worker_num, distributing_worker->energy);
    printf("Distributing worker %d with max num_bags:%d and the trip time is %d\n", distributing_worker->worker_num, distributing_worker->num_bags, distributing_worker->trip_time);
    fflush(stdout);
}

void takeBagsFromShMem()
{
    printf("Disttributing worker %d is waiting for available bags\n", distributing_worker_num);
    acquireSem(sem_splitted_bags, 0, "disttributing_worker.c"); // wait untill there are bags to distribute
    // printf("Disttributing worker %d is acquire available bags\n", distributing_worker_num);
    acquireSem(sem_spaces_available, 0, "disttributing_worker.c"); // ensure that tere is no one editing memory,wait untill there are spaces to distribute
    printf("Disttributing worker %d is acquire spaces\n", distributing_worker_num);
    if (*shmptr_splitted_bages > 0)
    {
        if (*shmptr_splitted_bages >= distributing_worker->num_bags)
        {
            *shmptr_splitted_bages -= distributing_worker->num_bags;
            printf("The distributing worker %d took %d bags\n", distributing_worker->worker_num, distributing_worker->num_bags);
            fflush(stdout);
        }
        else
        {
            printf("The distributing worker %d took %d bags\n", distributing_worker->worker_num, *shmptr_splitted_bages);
            fflush(stdout);
            *shmptr_splitted_bages = 0;
        }
    }
    else
    {
        printf("There are no splitted bags\n");
        fflush(stdout);
        return;
    }

    releaseSem(sem_spaces_available, 0, "disttributing_worker.c"); // release the semaphore to allow other workers to edit the memory
    // printf("Disttributing worker %d is release spaces\n", distributing_worker_num);
    printf("Distributing worker %d toke splitted bages from the shared memory,value=%d.\n", distributing_worker_num, *shmptr_splitted_bages);
    fflush(stdout);

    if (*shmptr_splitted_bages > 0)
    {
        releaseSem(sem_splitted_bags, 0, "disttributing_worker.c"); // release the semaphore to allow other workers to edit the memory
        // printf("Disttributing worker %d is release available bags\n", distributing_worker_num);
    }

    distributing_worker->is_tripping = 1;
    apply_trip_time(); // disttributing worker will distributes the bags to the families
}

void distributeBagsToFamillies()
{

    acquireSem(sem_starviation_familes, 0, "distributing_worker.c");
    printf("distributing worker %d acquire\n", distributing_worker->worker_num);

    Family *sortedFamiles = sort_families();
    for (int i = 0; i < distributing_worker->num_bags; i++)
    {
        if (sortedFamiles[i].starvation_level == 0) // when reached to the dead families
        {
            break;
        }
        printf("The distributing worker %d will distribute the bag to the family %d\n", distributing_worker->worker_num, sortedFamiles[i].family_num);
        families[sortedFamiles[i].family_num - 1].starvation_level -= get_random_number(range_starv_decrease[0], range_starv_decrease[1]);
        if (families[sortedFamiles[i].family_num - 1].starvation_level < 0)
        {
            families[sortedFamiles[i].family_num - 1].starvation_level = 1;
        }
    }

    sleep(2);
    printf("distributing worker %d realase\n", distributing_worker->worker_num);

    releaseSem(sem_starviation_familes, 0, "distributing_worker.c");

    apply_trip_time(); // go back to the safe storage area
}

Family *sort_families()
{
    // sort the families based on the starvation level
    sorted_families = malloc(number_of_families * sizeof(Family));
    // Copying the data
    memcpy(sorted_families, families, number_of_families * sizeof(Family));

    // Sorting the copied array
    qsort(sorted_families, number_of_families, sizeof(struct Family), compareFamilies);

    // print the sorted families
    printf("The sorted families based on the starvation level are:\n");
    for (int i = 0; i < number_of_families; i++)
    {
        printf("The starvation of the family %d is %d\n", sorted_families[i].family_num, sorted_families[i].starvation_level);
    }
    return sorted_families;
}

int compareFamilies(const void *a, const void *b)
{
    struct Family *familyA = (struct Family *)a;
    struct Family *familyB = (struct Family *)b;
    return familyB->starvation_level - familyA->starvation_level;
}