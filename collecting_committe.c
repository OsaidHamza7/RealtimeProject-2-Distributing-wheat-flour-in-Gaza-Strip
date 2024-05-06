#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void init_signals_handlers();
void signal_handler_MARTYRED(int sig);
void signal_handler_SIGALRM(int sig);
void fillEnergyWorkers();
int apply_trip_time();
void get_information_committee(char **argv, int committee_num);
void killWorker();
int compareCollWorkersEnergy(const void *a, const void *b);
//***********************************************************************************

int msg_ground_id;
int msg_safe_area_id;
char *shmptr_collecting_committees;
int sem_collecting_committees;
int period_trip_committee[2];
int range_energy_workers[2];
int periodic_energy_reduction;
int range_energy_loss[2];
int is_alarmed;
int is_worker_dead;
int number_of_committees;
Container container;
Collecting_Committee *collecting_committee;

Collecting_Committee *collecting_committees;
Collecting_Committee *temp;
Worker *sorted_current_collecting_workers;
int main(int argc, char **argv)

{
    // check the number of arguments
    if (argc < 8)
    {
        perror("The user shouldn pass an argument like:committee_num, number_of_workers, period_trip_committee, range_energy_workers, period_energy_reduction, energy_loss_range,number_of_committees\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get information from the arguments
    int committee_num = atoi(argv[1]);
    number_of_committees = atoi(argv[7]);

    // open the message queues
    msg_ground_id = createMessageQueue(MSGQKEY_GROUND, "collecting_committe.c");
    msg_safe_area_id = createMessageQueue(MSGQKEY_SAFE_AREA, "collecting_committe.c");

    // open the shared memory of the committees
    shmptr_collecting_committees = createSharedMemory(SHKEY_COLLECTION_COMMITTEES, number_of_committees * sizeof(struct Collecting_Committee), "collecting_committe.c");
    collecting_committees = (struct Collecting_Committee *)shmptr_collecting_committees;

    sem_collecting_committees = createSemaphore(SEMKEY_COLLECTING_COMMITTEES, 1, 1, "collecting_committe.c");

    get_information_committee(argv, committee_num);

    fillEnergyWorkers();

    // initialize the signal handlers
    init_signals_handlers();
    // alarm(periodic_energy_reduction);

    while (1)
    { // the committee collect the wheat flour from the ground every specified time

        acquireSem(sem_collecting_committees, 0, "collecting_committe.c");
        collecting_committee->is_tripping = 0;
        releaseSem(sem_collecting_committees, 0, "collecting_committe.c");

        if (collecting_committee->num_workers == collecting_committee->num_killed_workers)
        {
            printf("Committee %d has no workers,so can't take containers\n", collecting_committee->committee_num);
            sleep(2);
            break;
        }
        printf("Committee %d with %d workers is waiting for a container from the ground\n", collecting_committee->committee_num, collecting_committee->num_workers);
 
        while (1)
        {
            if (msgrcv(msg_ground_id, &container, sizeof(container), 0, 0) == -1)
            {
                if (errno == EINTR)
                {
                    // printf("Committee:msgrcv interrupted by signal. Retrying...\n");
                    continue;
                }
                else
                {
                    perror("Committee:msgrcv");
                    break;
                }
            }
            // Process the message here
            break;
        }
        collecting_committee = temp;
        // print the information of the container that the commettee collected
        printf("Committee %d collect container %d with %d bags and going to the safe area\n", collecting_committee->committee_num, container.conatiner_num, container.capacity_of_bags);
        fflush(stdout);

        acquireSem(sem_collecting_committees, 0, "collecting_committe.c");
        collecting_committee->is_tripping = 1;
        releaseSem(sem_collecting_committees, 0, "collecting_committe.c");

        apply_trip_time(); // time for going from ground to the safe storage area
        if (collecting_committee->num_workers == collecting_committee->num_killed_workers)
        {
            printf("Committee %d has no workers\n", collecting_committee->committee_num);
            return 0;
        }
        // send the container to the safe area
        if (msgsnd(msg_safe_area_id, &container, sizeof(container), 0) == -1)
        {
            perror("Committee:msgsnd");
            return 4;
        }
        printf("Committee %d collectting container %d with %d bags have reached the safe area and going back to the ground\n", collecting_committee->committee_num, container.conatiner_num, container.capacity_of_bags);
        fflush(stdout);

        apply_trip_time(); // time for going back to the ground
    }
    return 0;
}

void init_signals_handlers()
{
    // when the signal SIGHUP is received, the worker will be martyred
    if (sigset(SIGINT, signal_handler_MARTYRED) == -1)
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
}

// function SIGHUB
void signal_handler_MARTYRED(int sig)
{
    printf("The signal %d reached to the committee:%d then one of the workers maybe martyred \n\n", sig, collecting_committee->committee_num);
    fflush(stdout);
    killWorker();
}

void signal_handler_SIGALRM(int sig)
{
    printf("The alarm signal %d reached to the committee:%d then the energy of the workers will be reduced\n\n", sig, collecting_committee->committee_num);
    is_alarmed = 1;
    acquireSem(sem_collecting_committees, 0, "collecting_committe.c");
    for (int i = 0; i < collecting_committee->num_workers; i++)
    {

        collecting_committee->workers[i].energy -= get_random_number(range_energy_loss[0], range_energy_loss[1]);
        if (collecting_committee->workers[i].energy < 0)
        {
            collecting_committee->workers[i].energy = 0;
        }
        printf("Worker %d in committee %d has energy %d\n", collecting_committee->workers[i].worker_num, collecting_committee->committee_num, collecting_committee->workers[i].energy);
    }
    releaseSem(sem_collecting_committees, 0, "collecting_committe.c");
    alarm(periodic_energy_reduction);
}

void fillEnergyWorkers()
{
    acquireSem(sem_collecting_committees, 0, "collecting_committe.c");
    for (int i = 0; i < collecting_committee->num_workers; i++)
    {
        // fill the array struct of workers in the committee
        collecting_committee->workers[i].worker_num = i + 1;
        collecting_committee->workers[i].energy = get_random_number(range_energy_workers[0], range_energy_workers[1]);
        // printf("Worker %d in committee %d has energy %d\n", collecting_committee->workers[i].worker_num, collecting_committee->committee_num, collecting_committee->workers[i].energy);
    }
    releaseSem(sem_collecting_committees, 0, "collecting_committe.c");
}

int apply_trip_time()
{
    int pause_time = sleep(collecting_committee->trip_time);

    while (pause_time != 0)
    {
        if (is_alarmed == 1)
        {
            is_alarmed = 0;
        }
        else if (collecting_committee->num_workers == collecting_committee->num_killed_workers)
        {
            printf("Committee %d has no workers,so can't continue the trip\n", collecting_committee->committee_num);
            return 0;
        }
        printf("Committee %d is continuing the trip\n", collecting_committee->committee_num);
        // intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
        pause_time = sleep(pause_time);
    }
    return 1;
}

void get_information_committee(char **argv, int committee_num)
{
    // get the information of the committee from the arguments
    collecting_committee = &collecting_committees[committee_num - 1];
    temp = collecting_committee;

    periodic_energy_reduction = atoi(argv[5]);
    split_string(argv[3], period_trip_committee);
    split_string(argv[4], range_energy_workers);
    split_string(argv[6], range_energy_loss);

    acquireSem(sem_collecting_committees, 0, "collecting_committe.c");

    collecting_committee->pid = getpid();
    collecting_committee->committee_num = committee_num;
    collecting_committee->num_workers = atoi(argv[2]);
    collecting_committee->is_tripping = 0;
    collecting_committee->trip_time = get_random_number(period_trip_committee[0], period_trip_committee[1]);

    releaseSem(sem_collecting_committees, 0, "collecting_committe.c");

    printf("Committee %d has %d workers and the trip time is %d\n", collecting_committee->committee_num, collecting_committee->num_workers, collecting_committee->trip_time);
    fflush(stdout);
}

void killWorker()
{
    // Sort the current collecting workers based on the energy
    sorted_current_collecting_workers = malloc(collecting_committee->num_workers * sizeof(Worker));

    // Copying the data
    memcpy(sorted_current_collecting_workers, collecting_committee->workers, collecting_committee->num_workers * sizeof(Worker));

    qsort(sorted_current_collecting_workers, collecting_committee->num_workers, sizeof(Worker), compareCollWorkersEnergy);

    int dead_worker_num = 0;
    acquireSem(sem_collecting_committees, 0, "collecting_committe.c");
    // get current collecting workers and determine the probability of the worker to be killed

    for (int i = 0; i < collecting_committee->num_workers; i++)
    {
        if (sorted_current_collecting_workers[i].is_martyred)
        {
            continue;
        }

        printf("Committie %d, Collectting Worker %d has energy %d..\n", collecting_committee->committee_num, sorted_current_collecting_workers[i].worker_num, sorted_current_collecting_workers[i].energy);

        // Determine the probability of the worker to be killed
        float probability_killed = (100 - sorted_current_collecting_workers[i].energy) / 100.0;
        int worker_killed = ((float)rand() / (float)RAND_MAX) < probability_killed;
        if (worker_killed)
        {
            dead_worker_num = sorted_current_collecting_workers[i].worker_num;
            collecting_committee->workers[dead_worker_num - 1].is_martyred = 1;
            printf("Dead Worker number is %d\n", dead_worker_num);
            collecting_committee->num_killed_workers += 1;
            break;
        }
    }
    releaseSem(sem_collecting_committees, 0, "collecting_committe.c");
    free(sorted_current_collecting_workers);
}

int compareCollWorkersEnergy(const void *a, const void *b)
{
    Worker *worker1 = (Worker *)a;
    Worker *worker2 = (Worker *)b;
    return worker1->energy - worker2->energy;
}