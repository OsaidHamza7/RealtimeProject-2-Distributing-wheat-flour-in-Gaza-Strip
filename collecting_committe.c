#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void init_signals_handlers();
void signal_handler_MARTYRED(int sig);
void signal_handler_SIGALRM(int sig);
void fillEnergyWorkers();
int apply_trip_time();
void get_information_committee(char **argv, int committee_num);
//***********************************************************************************

int msg_ground_id;
int msg_safe_area_id;
char *shmptr_collecting_committees;
int period_trip_committee[2];
int range_energy_workers[2];
int periodic_energy_reduction;
int range_energy_loss[2];
int is_alarmed;
int is_worker_dead;
int number_of_committees;
Container container;
Collecting_Committee collecting_committee;

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

    // open the ground message queues
    msg_ground_id = createMessageQueue(MSGQKEY_GROUND, "collecting_committe.c");
    msg_safe_area_id = createMessageQueue(MSGQKEY_SAFE_AREA, "collecting_committe.c");
    //  open the shared memory of the committees
    //  Create a shared memory for all struct of the collecting committees
    // shmptr_collecting_committees = createSharedMemory(SHKEY_COLLECTION_COMMITTEES, number_of_committees * sizeof(struct Collecting_Committee), "collecting_committe.c");
    // collecting_committees = (struct Collecting_Committee *)shmptr_collecting_committees;

    get_information_committee(argv, committee_num);

    fillEnergyWorkers();

    // initialize the signal handlers
    // init_signals_handlers();
    // alarm(periodic_energy_reduction);

    while (1)
    { // the committee collect the wheat flour from the ground every specified time
        printf("Committee %d is waiting for a container from the ground\n", collecting_committee.committee_num);
        collecting_committee.is_tripping = 0;
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
        // print the information of the container that the commettee collected
        printf("Committee %d collect container %d with %d bags and going to the safe area\n", collecting_committee.committee_num, container.conatiner_num, container.capacity_of_bags);
        fflush(stdout);
        collecting_committee.is_tripping = 1;

        apply_trip_time(); // time for going from ground to the safe storage area

        // send the container to the safe area
        if (msgsnd(msg_safe_area_id, &container, sizeof(container), 0) == -1)
        {
            perror("Committee:msgsnd");
            return 4;
        }
        printf("Committee %d collectting container %d with %d bags have reached the safe area\n", collecting_committee.committee_num, container.conatiner_num, container.capacity_of_bags);
        fflush(stdout);
        apply_trip_time(); // time for going back to the ground
    }
    printf("wtfffffffffffffffffffff\n");
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
}

// function SIGHUB
void signal_handler_MARTYRED(int sig)
{
    printf("The signal %d reached to the committee:%d then one of the workers is martyred \n\n", sig, collecting_committee.committee_num);
    fflush(stdout);
}

void signal_handler_SIGALRM(int sig)
{
    printf("The alarm signal %d reached to the committee:%d then the energy of the workers will be reduced\n\n", sig, collecting_committee.committee_num);
    is_alarmed = 1;
    /*for (int i = 0; i < collecting_committee->num_workers; i++)
    {
        if (collecting_committee->workers[i].energy >= 5)
        {
            collecting_committee->workers[i].energy -= get_random_number(range_energy_loss[0], range_energy_loss[1]);
            printf("Worker %d in committee %d has energy %d\n", collecting_committee->workers[i].worker_num, collecting_committee->committee_num, collecting_committee->workers[i].energy);
        }
    }
    alarm(periodic_energy_reduction);*/
}

void fillEnergyWorkers()
{
    for (int i = 0; i < collecting_committee.num_workers; i++)
    {
        // fill the array struct of workers in the committee
        collecting_committee.workers[i].worker_num = i + 1;
        collecting_committee.workers[i].energy = get_random_number(range_energy_workers[0], range_energy_workers[1]);
        printf("Worker %d in committee %d has energy %d\n", collecting_committee.workers[i].worker_num, collecting_committee.committee_num, collecting_committee.workers[i].energy);
    }
}

int apply_trip_time()
{
    int pause_time = sleep(collecting_committee.trip_time);

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

void get_information_committee(char **argv, int committee_num)
{
    // get the information of the committee from the arguments
    // collecting_committee = &collecting_committees[committee_num - 1];
    collecting_committee.committee_num = committee_num;
    collecting_committee.num_workers = atoi(argv[2]);

    periodic_energy_reduction = atoi(argv[5]);
    split_string(argv[3], period_trip_committee);
    split_string(argv[4], range_energy_workers);
    split_string(argv[6], range_energy_loss);

    collecting_committee.is_tripping = 0;
    collecting_committee.trip_time = get_random_number(period_trip_committee[0], period_trip_committee[1]);

    printf("Committee %d has %d workers and the trip time is %d\n", collecting_committee.committee_num, collecting_committee.num_workers, collecting_committee.trip_time);
    fflush(stdout);
}