#include "header.h"
#include "ipcs.h"
//***********************************************************************************
void getCurrentPlaneNumbers();
//***********************************************************************************
char *shmptr_plane;
char *shmptr_collecting_committees;
Plane *planes;
Collecting_Committee *collecting_committees;
int current_planes_numbers[10]; // array of the current plane numbers
int crrent_number_planes;       // Number of current planes
int is_planes_here = 0;         // flag to check if there are planes or not
int num_planes;                 // number of cargo planes
int number_of_committees;       // number of committees
int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 2)
    {
        perror("The user should pass the arguments like: num_cargo_planes\n");
        exit(-1);
    }

    num_planes = atoi(argv[1]);
    number_of_committees = atoi(argv[2]);

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    shmptr_plane = createSharedMemory(SHKEY_PLANES, num_planes * sizeof(struct Plane), "plane.c");
    planes = (struct Plane *)shmptr_plane;

    // open the shared memory of all struct of the collecting committees
    shmptr_collecting_committees = createSharedMemory(SHKEY_COLLECTION_COMMITTEES, number_of_committees * sizeof(struct Collecting_Committee), "collecting_committe.c");
    collecting_committees = (struct Collecting_Committee *)shmptr_collecting_committees;

    // every specific time kill a random plane
    while (1)
    {
        sleep(10);
        // open the shared memory to get the array of struct of the planes
        getCurrentPlaneNumbers();
        if (!is_planes_here)
        {
            printf("There are no planes\n");
            continue;
        }
        int plane_indx = rand() % crrent_number_planes; // get a random index of current planes numbers array
        int plane_num = current_planes_numbers[plane_indx];
        kill(planes[plane_num - 1].pid, SIGHUP); // kill the current container in the plane
        printf("Dropping container in plane %d with pid=%d is shotted\n", plane_num, planes[plane_num - 1].pid);

        printf("=====================================================\n");
    }
    return 0;
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