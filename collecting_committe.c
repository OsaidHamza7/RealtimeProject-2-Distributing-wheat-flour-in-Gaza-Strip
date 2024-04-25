#include "header.h"
#include "ipcs.h"
//***********************************************************************************

//***********************************************************************************

int num_workers_in_committee;
int msg_ground_id;

Container *containers;
Committee *committee;

int main(int argc, char **argv)
{
    // check the number of arguments
    if (argc < 3)
    {
        perror("The user shouldn pass an argument like the number of the committee and the number of workers in the committee.\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    
    // open the ground message queue
    msg_ground_id = createMessageQueue(MSGQKEY_GROUND, "collecting_committe.c");

    committee = (Committee*)malloc(num_collecting_relief_committees * sizeof(Committee));
    
    committee->num = atoi(argv[1]);
    committee->num_workers = atoi(argv[2]);

    while(1){//the commitee collect the wheat flour from the ground every specified time
    if (msgrcv(msg_ground_id, &containers, sizeof(containers), 0, 0) == -1)
    {
        perror("Committe:msgrcv");
        return 3;
    }
    //print the information of the container that the commettee collected
    printf("Container %d is collecting from the ground with %d bags of wheat flour\n", containers[i+1].num, containers[i+1].num_bags);
    fflush(stdout);
    sleep(5);
    }
    
            
    



    return 0;
}