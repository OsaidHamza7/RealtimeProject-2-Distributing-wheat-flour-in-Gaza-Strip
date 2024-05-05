#include "header.h"
#include "ipcs.h"

//***********************************************************************************
void get_information_plane(char **argv);
void createContainers();
void printContainers();
void init_signals_handlers();
void signal_handler_CRASHED(int sig);
//***********************************************************************************
Plane *plane;
char *shmptr_plane;
int sem_planes;
Plane *planes;

int range_num_containers[2];
int range_num_bags[2];
int range_dropping_time[2];
int range_refill_planes[2];
int time_refill_plane; // the period of refilling the plane

int msg_ground;
int num_planes;
int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 7)
    {
        perror("The user should pass the arguments like: plane_num,range_num_containers, range_num_bages,range_dropping_time,range_refill_planes\n");
        exit(-1);
    }
    srand((unsigned)getpid()); // seed for the random function with the ID of the current process
    num_planes = atoi(argv[6]);
    init_signals_handlers();

    // open the ground message queue
    msg_ground = createMessageQueue(MSGQKEY_GROUND, "plane.c");

    shmptr_plane = createSharedMemory(SHKEY_PLANES, num_planes * sizeof(struct Plane), "plane.c");
    planes = (struct Plane *)shmptr_plane;

    sem_planes = createSemaphore(SEMKEY_PLANES, 1, 1, "plane.c");
    // get the information of the plane
    get_information_plane(argv);

    // write the conainers to the ground message queue,after dropping time of the container
    while (1)
    { // Firslty,the plane does not reached yet (it goes to bring containers)
        if (plane->is_refilling)
        {
            printf("The plane %d goes to bring containers\n", plane->plane_num);
            sleep(time_refill_plane);
            createContainers();
            // printContainers();
        }
        acquireSem(sem_planes, 0, "plane.c");
        plane->is_refilling = 0;
        releaseSem(sem_planes, 0, "plane.c");

        // start drop the containers
        printf("Plane %d start dropping the containers\n", plane->plane_num);
        fflush(stdout);
        for (int i = 0; i < plane->num_containers; i++)
        {
            sleep(1);
            printf("Container %d of plane %d is dropping with bags=%d\n", i + 1, plane->plane_num, plane->containers[i].capacity_of_bags);
            fflush(stdout);
            int n = sleep(plane->containers[i].dropping_time);

            if (n != 0) // if the plane is shotted by occupation
            {
                acquireSem(sem_planes, 0, "plane.c");
                double ratio = (double)n / plane->containers[i].dropping_time;
                if (ratio < 0.5)
                {
                    plane->containers[i].capacity_of_bags -= ratio * plane->containers[i].capacity_of_bags;
                    printf("Container %d of plane %d is killed PARTLY and new bags=%d\n", plane->containers[i].conatiner_num, plane->plane_num, plane->containers[i].capacity_of_bags);
                    fflush(stdout);
                }
                else
                {
                    plane->containers[i].capacity_of_bags = 0;
                    printf("Container %d of plane %d is killed TOTALY and new bags=%d\n", plane->containers[i].conatiner_num, plane->plane_num, plane->containers[i].capacity_of_bags);
                    fflush(stdout);
                }
                releaseSem(sem_planes, 0, "plane.c");
                if (plane->containers[i].capacity_of_bags == 0)
                {
                    continue;
                }
            }

            else
            {
                printf("Container %d of plane %d with %d bags reached ground SAFLY\n", plane->containers[i].conatiner_num, plane->plane_num, plane->containers[i].capacity_of_bags);
                fflush(stdout);
            }

            // send the container to the ground message queue
            if (msgsnd(msg_ground, &plane->containers[i], sizeof(plane->containers[i]), 0) == -1)
            {
                perror("Plane:msgsnd");
                return 3;
            }
            printf("Container %d of plane %d with bags=%d wrritten in ground meessage queue\n", plane->containers[i].conatiner_num, plane->plane_num, plane->containers[i].capacity_of_bags);
            fflush(stdout);
        }
        acquireSem(sem_planes, 0, "plane.c");
        plane->is_refilling = 1;
        releaseSem(sem_planes, 0, "plane.c");
    }

    return 0;
}

void init_signals_handlers()
{
    if (sigset(SIGHUP, signal_handler_CRASHED) == -1)
    { // set the signal handler for SIGINT
        perror("Signal Error\n");
        exit(-1);
    }
}

// function SIGHUB
void signal_handler_CRASHED(int sig)
{
    printf("The signal %d reached to the plane %d. \n\n", sig, plane->plane_num);
    fflush(stdout);
}

void createContainers()
{

    for (int i = 0; i < plane->num_containers; i++)
    {
        // fill the array struct of containers in the plane
        plane->containers[i].conatiner_num = i + 1;
        plane->containers[i].capacity_of_bags = get_random_number(range_num_bags[0], range_num_bags[1]);
        plane->containers[i].dropping_time = get_random_number(range_dropping_time[0], range_dropping_time[1]);
    }
}

void printContainers()
{
    printf("*****************************************************************************************\n");
    printf("The plane %d is ready to drop %d wheat flour containers:\n", plane->plane_num, plane->num_containers);
    for (int i = 0; i < plane->num_containers; i++)
    {
        printf("Container %d in plane %d:\n", plane->containers[i].conatiner_num, plane->plane_num);
        printf("Number of bags: %d\n", plane->containers[i].capacity_of_bags);
        printf("Dropping time: %d\n", plane->containers[i].dropping_time);
        printf("--------------------\n");
    }
    printf("*****************************************************************************************\n");
    fflush(stdout);
}

void get_information_plane(char **argv)
{
    int plane_num = atoi(argv[1]);
    plane = &planes[plane_num - 1];
    acquireSem(sem_planes, 0, "plane.c");

    plane->is_refilling = 1;
    plane->pid = getpid();
    plane->plane_num = plane_num;

    split_string(argv[2], range_num_containers);
    split_string(argv[3], range_num_bags);
    split_string(argv[4], range_dropping_time);
    split_string(argv[5], range_refill_planes);

    plane->num_containers = get_random_number(range_num_containers[0], range_num_containers[1]);
    time_refill_plane = get_random_number(range_refill_planes[0], range_refill_planes[1]);
    releaseSem(sem_planes, 0, "plane.c");
    /*printf("The range of the number of wheat flour containers is: %d - %d\n", range_num_containers[0], range_num_containers[1]);
    printf("The range of the number of bags is: %d - %d\n", range_num_bags[0], range_num_bags[1]);
    printf("The range of the dropping time is: %d - %d\n", rane_dropping_time[0], rane_dropping_time[1]);
    printf("The number of wheat flour containers is: %d\n", num_containers);
    printf("The number of bags is: %d\n", num_bags);*/
}