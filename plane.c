#include "header.h"

//***********************************************************************************
void get_information_plane(char **argv);
void createContainers();
//***********************************************************************************
int range_num_containers[2];
int range_num_bags[2];
int rane_dropping_time[2];

int num_containers = 0;

Container *containers;
int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 4)
    {
        perror("The user should pass the arguments like: range_num_wheat_flour_containers, range_num_bages,range_dropping_time\n");
        exit(-1);
    }
    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get the information of the plane
    get_information_plane(argv);

    createContainers();

    // write the bags to the shared memory,after dropping time of the container
    for (int i = 0; i < num_containers; i++)
    { // always reach the ground safly
        sleep(containers[i].dropping_time);
        printf("Container %d is dropping %d bags of wheat flour\n", containers[i].num, containers[i].num_bags);
        fflush(stdout);
        // write the bags to the shared memory
        // write_to_sh(containers[i].num_bags,shared_memory);
    }

    sleep(50);
    return 0;
}

void createContainers()
{
    containers = (Container *)malloc(num_containers * sizeof(Container));
    for (int i = 0; i < num_containers; i++)
    {

        // fill the structs of the containers
        containers[i].num = i + 1;
        containers[i].num_bags = get_random_number(range_num_bags[0], range_num_bags[1]);
        containers[i].dropping_time = get_random_number(rane_dropping_time[0], rane_dropping_time[1]);
        containers[i].status = 0;
        printf("Container %d is ready to drop %d bags of wheat flour in %d seconds\n", containers[i].num, containers[i].num_bags, containers[i].dropping_time);
        fflush(stdout);
    }
}

void get_information_plane(char **argv)
{

    split_string(argv[1], range_num_containers);
    split_string(argv[2], range_num_bags);
    split_string(argv[3], rane_dropping_time);

    num_containers = get_random_number(range_num_containers[0], range_num_containers[1]);

    /*printf("The range of the number of wheat flour containers is: %d - %d\n", range_num_containers[0], range_num_containers[1]);
    printf("The range of the number of bags is: %d - %d\n", range_num_bags[0], range_num_bags[1]);
    printf("The range of the dropping time is: %d - %d\n", rane_dropping_time[0], rane_dropping_time[1]);
    printf("The number of wheat flour containers is: %d\n", num_containers);
    printf("The number of bags is: %d\n", num_bags);
    printf("The dropping time is: %d\n", num_dropping_time);*/
}