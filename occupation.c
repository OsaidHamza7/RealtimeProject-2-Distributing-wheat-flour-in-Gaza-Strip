#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void getCurrentPlaneNumbers();
void getCurrentCollectingCommitteesNumbers();
void killPlanes();
void killCommittees();
void killDistributingWorkers();
int getCurrentDistributingWorkers();
int compareDistWorkersEnergy(const void *a, const void *b);
//***********************************************************************************
char *shmptr_plane;
char *shmptr_collecting_committees;
Plane *planes;
Collecting_Committee *collecting_committees;
int current_planes_numbers[10];                // array of the current plane numbers
int crrent_number_planes;                      // Number of current planes
int is_planes_here = 0;                        // flag to check if there are planes or not
int num_planes;                                // number of cargo planes
int number_of_committees;                      // number of committees
int current_collecting_committees_numbers[10]; // array of the current plane numbers
int crrent_number_collecting_committees;       // Number of current planes
int is_committees_here = 0;                    // flag to check if there are planes or not
char *shmptr_distributing_workers;
int num_distributing_workers;
Distributing_Worker *distributing_workers;
Distributing_Worker *current_distributing_workers; // array of the current plane numbers
Distributing_Worker *sorted_current_distributing_workers;
int crrent_number_distributing_workers; // Number of current planes
int is_distributing_workers_here = 0;   // flag to check if there are planes or not
int occupation_num;
int sem_collecting_committees;
//***********************************************************************************.

int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 5)
    {
        perror("The user should pass the arguments like: num_cargo_planes,num_collecting_committes,num_distributing_workers,occupation_num\n");
        exit(-1);
    }
    num_planes = atoi(argv[1]);
    number_of_committees = atoi(argv[2]);
    num_distributing_workers = atoi(argv[3]);
    occupation_num = atoi(argv[4]);

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    shmptr_plane = createSharedMemory(SHKEY_PLANES, num_planes * sizeof(struct Plane), "occupation.c");
    shmptr_collecting_committees = createSharedMemory(SHKEY_COLLECTION_COMMITTEES, number_of_committees * sizeof(struct Collecting_Committee), "occupation.c");
    shmptr_distributing_workers = createSharedMemory(SHKEY_DISTRIBUTING_WORKERS, num_distributing_workers * sizeof(struct Distributing_Worker), "occupation.c");

    planes = (struct Plane *)shmptr_plane;
    collecting_committees = (struct Collecting_Committee *)shmptr_collecting_committees;
    distributing_workers = (struct Distributing_Worker *)shmptr_distributing_workers;

    sem_collecting_committees = createSemaphore(SEMKEY_COLLECTING_COMMITTEES, 1, 1, "occupation.c");

    while (1)
    {
        sleep(5);
        switch (occupation_num)

        {
        case 1:
            // killPlanes();
            break;
        case 2:
            killCommittees();
            break;
        case 3:
            killDistributingWorkers();
            break;

        default:
            break;
        }
    }
    return 0;
}
void killPlanes()
{
    getCurrentPlaneNumbers();
    if (!is_planes_here)
    {
        printf("There are no planes\n");
        return;
    }
    int plane_indx = rand() % crrent_number_planes; // get a random index of current planes numbers array
    int plane_num = current_planes_numbers[plane_indx];
    kill(planes[plane_num - 1].pid, SIGHUP); // kill the current container in the plane
    printf("Dropping container in plane %d with pid=%d is shotted\n", plane_num, planes[plane_num - 1].pid);

    printf("=====================================================\n");
}

void killCommittees()
{
    getCurrentCollectingCommitteesNumbers();

    if (!is_committees_here)
    {
        printf("There are no committees to kill\n");
        return;
    }

    int committee_indx = rand() % crrent_number_collecting_committees; // get a random index of current planes numbers array
    int committee_num = current_collecting_committees_numbers[committee_indx];
    kill(collecting_committees[committee_num - 1].pid, SIGINT); // kill the current container in the plane
    printf("Committee %d with pid=%d is shotted by the snipers\n", committee_num, collecting_committees[committee_num - 1].pid);

    printf("=====================================================\n");
}

void killDistributingWorkers()
{
    // get the current distributing workers,then calculate the probability of the workers to be killed based on the energy
    int dead_worker_num = getCurrentDistributingWorkers();
    if (!is_distributing_workers_here)
    {
        printf("There are no distributing workers\n");
        fflush(stdout);
        return;
    }
    if (dead_worker_num == 0) // there is no killed workers
    {
        printf("There is no killed workers\n");
        fflush(stdout);
        return;
    }

    kill(distributing_workers[dead_worker_num - 1].pid, SIGHUP); // kill the current container in the plane
    printf("Distributing worker %d with pid=%d is shotted by the snipers\n", dead_worker_num, distributing_workers[dead_worker_num - 1].pid);

    printf("=====================================================\n");
}

void getCurrentPlaneNumbers()
{
    // make the array current_planes_numbers empty

    is_planes_here = 0;

    crrent_number_planes = 0;
    for (int i = 0; i < num_planes; i++)
    {
        if (!planes[i].is_refilling)
        {
            current_planes_numbers[crrent_number_planes] = i + 1;
            crrent_number_planes++;
            is_planes_here = 1;
        }
    }
}

void getCurrentCollectingCommitteesNumbers()
{
    is_committees_here = 0;
    crrent_number_collecting_committees = 0;
    acquireSem(sem_collecting_committees, 0, "occupation.c");

    for (int i = 0; i < number_of_committees; i++)
    {
        if (collecting_committees[i].is_tripping && collecting_committees[i].num_workers != collecting_committees[i].num_killed_workers)
        {
            printf("Committee %d is tripping\n", collecting_committees[i].committee_num);
            current_collecting_committees_numbers[crrent_number_collecting_committees] = i + 1;
            crrent_number_collecting_committees++;
            is_committees_here = 1;
        }
    }
    releaseSem(sem_collecting_committees, 0, "occupation.c");
}

int getCurrentDistributingWorkers()
{
    is_distributing_workers_here = 0;
    crrent_number_distributing_workers = 0;
    current_distributing_workers = malloc(num_distributing_workers * sizeof(Distributing_Worker));

    // get the current distributing workers in trip
    for (int i = 0; i < num_distributing_workers; i++)
    {
        if (!distributing_workers[i].is_martyred && distributing_workers[i].is_tripping)
        {
            current_distributing_workers[crrent_number_distributing_workers] = distributing_workers[i];
            crrent_number_distributing_workers++;
            is_distributing_workers_here = 1;
        }
    }
    // there are no distributing workers in trip
    if (!is_distributing_workers_here)
    {
        free(current_distributing_workers);
        return 0;
    }

    // Sort the current distributing workers based on the energy
    sorted_current_distributing_workers = malloc(crrent_number_distributing_workers * sizeof(Distributing_Worker));
    // Copying the data
    memcpy(sorted_current_distributing_workers, current_distributing_workers, crrent_number_distributing_workers * sizeof(Distributing_Worker));

    qsort(sorted_current_distributing_workers, crrent_number_distributing_workers, sizeof(Distributing_Worker), compareDistWorkersEnergy);

    int dead_worker_num = 0;
    // get current distributing workers and determine the probability of the worker to be killed
    for (int i = 0; i < crrent_number_distributing_workers; i++)
    {
        printf("Distributin Worker %d has energy %d\n", sorted_current_distributing_workers[i].worker_num, sorted_current_distributing_workers[i].energy);

        // Determine the probability of the worker to be killed
        float probability_killed = (100 - sorted_current_distributing_workers[i].energy) / 100.0;
        int worker_killed = ((float)rand() / (float)RAND_MAX) < probability_killed;
        if (worker_killed)
        {
            dead_worker_num = sorted_current_distributing_workers[i].worker_num;
            printf("Dead Worker number is %d\n", dead_worker_num);
            break;
        }
    }

    free(current_distributing_workers);
    free(sorted_current_distributing_workers);
    return dead_worker_num;
}

int compareDistWorkersEnergy(const void *a, const void *b)
{
    Distributing_Worker *distworkerA = (Distributing_Worker *)a;
    Distributing_Worker *distworkerB = (Distributing_Worker *)b;
    return distworkerA->energy - distworkerB->energy;
}