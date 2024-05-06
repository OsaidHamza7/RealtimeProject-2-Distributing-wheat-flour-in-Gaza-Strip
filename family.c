#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void init_signals_handlers();
void signal_handler_SIGALRM(int sig);
void getInformation(char **argv);
void fillFamilies(int number_of_families, Family *families);
//***********************************************************************************
int number_of_families;
char *shmptr_families;
char *shmptr_threshold_num_deceased_families;
Family *families;

int periodic_starvation_increase;
int range_starv_increase[2];

int sem_starviation_familes;
int main(int argc, char **argv)

{
    // check the number of arguments
    if (argc < 4)
    {
        perror("The user should pass an argument like:number_families, period_starvation_increase, range_starvation_increase\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process
    // get information from the arguments
    number_of_families = atoi(argv[1]);

    //  open the shared memory of all families
    shmptr_families = createSharedMemory(SHKEY_FAMILIES, number_of_families * sizeof(struct Family), "family.c");
    shmptr_threshold_num_deceased_families = createSharedMemory(SHKEY_THRESHOLD_NUM_DECEASED_FAMILIES, sizeof(int), "family.c");

    families = (struct Family *)shmptr_families;

    sem_starviation_familes = createSemaphore(SEMKEY_STARVATION_FAMILIES, 1, 1, "family.c");

    getInformation(argv);
    fillFamilies(number_of_families, families);
    init_signals_handlers();             // initialize the signals handlers
    alarm(periodic_starvation_increase); // set the alarm signal

    while (1)
    {
        pause();
    }

    return 0;
}

void init_signals_handlers()
{
    // SIGALARM is used for changing the energy of the worker every specific time
    if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    {
        perror("Signal Error\n");
        exit(-1);
    }
}

void signal_handler_SIGALRM(int sig)
{
    printf("The alarm signal %d reached to the families,then the starvation of the families will be increased by random number\n\n", sig);
    acquireSem(sem_starviation_familes, 0, "family.c");
    printf("family acquire\n");

    for (int i = 0; i < number_of_families; i++)
    {
        if (families[i].starvation_level != 0) // the family who is not dead
        {
            families[i].starvation_level += get_random_number(range_starv_increase[0], range_starv_increase[1]);
            if (families[i].starvation_level >= 100)
            {
                families[i].starvation_level = 0; // dead
                *shmptr_threshold_num_deceased_families += 1;
                kill(getppid(), SIGCLD);
            }
        }
        printf("The starvation of the family %d is %d\n", families[i].family_num, families[i].starvation_level);
    }

    sleep(2);
    printf("family realase\n");

    releaseSem(sem_starviation_familes, 0, "family.c");

    alarm(periodic_starvation_increase);
}

void fillFamilies(int number_of_families, Family *families)
{
    acquireSem(sem_starviation_familes, 0, "family.c");
    for (int i = 0; i < number_of_families; i++)
    {
        families[i].family_num = i + 1;
        families[i].starvation_level = 50;
    }
    releaseSem(sem_starviation_familes, 0, "family.c");
}

void getInformation(char **argv)
{
    periodic_starvation_increase = atoi(argv[2]);
    split_string(argv[3], range_starv_increase);

    /*printf("The number of families is %d\n", number_of_families);
    printf("The periodic starvation increase is %d\n", periodic_starvation_increase);
    printf("The range of starvation increase is %d to %d\n", range_starv_increase[0], range_starv_increase[1]);
*/
}