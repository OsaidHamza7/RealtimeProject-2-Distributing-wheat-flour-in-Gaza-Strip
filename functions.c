#include "header.h"

int num_cargo_planes = 5;
int range_num_wheat_flour_containers[2] = {5, 10};
int range_num_bages[2] = {5, 10};
int period_dropping_wheat_flour_container[2] = {2, 10};
int period_refill_planes[2] = {10, 20};
int num_families = 20;
int num_collecting_relief_committees = 10;
int num_workers_in_collecting_committe = 5;
int num_splitting_relief_workers = 10;
int num_distributing_relief_workers = 10;
int range_bags_per_distrib_worker[2] = {1, 10};
int distrib_relief_worker_threshold = 5;
int simulation_threshold_time = 180;
int threshold_families_death_rate = 30;
int threshold_num_cargo_planes_crashed = 3;
int threshold_wheat_flour_containers_shoted = 5;
int threshold_martyred_collecting_committee = 2;
int threshold_martyred_distributing_workers = 5;
int threshold_num_deceased_families = 5;

void readArgumentsFromFile(char *filename)

{
    /*char line[200];
    char label[50];

    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("The file not exist\n");
        exit(-2);
    }
    char separator[] = " ";

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "NUMBER_OF_LOST_ROUNDS") == 0)
        {
            NUMBER_OF_LOST_ROUNDS = atoi(str);
        }

        else if (strcmp(label, "SIMULATION_THRISHOLD") == 0)
        {
            SIMULATION_THRISHOLD = atoi(str);
        }
        else if (strcmp(label, "ROUND_TIME") == 0)
        {
            ROUND_TIME = atoi(str);
        }
        else if (strcmp(label, "RANGE_ENERGY") == 0)
        {
            RANGE_ENERGY[0] = atoi(str);
            str = strtok(NULL, separator);
            RANGE_ENERGY[1] = atoi(str);
        }
    }
    fclose(file);*/
}

void readFromFile(const char *filename, int *array)
{
    char tempLine[MAX_LINE_LENGTH];
    char varName[MAX_LINE_LENGTH];
    char valueStr[MAX_LINE_LENGTH];

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int numLines = 0;

    while (fgets(tempLine, sizeof(tempLine), file) != NULL)
    {
        // Split the line into variable name and value
        sscanf(tempLine, "%s %s", varName, valueStr);

        // Remove leading and trailing whitespaces from the variable name and value
        trim(varName);
        trim(valueStr);

        int min = 0, max = 0;
        // If the value is a multiple numbers, split them into min and max
        if (strstr(valueStr, ",") != NULL)
        {
            // If the value is a range, convert it to two integers
            sscanf(valueStr, "%d,%d", &min, &max);
        }
        else // If the value is a single number, convert it to an integer
        {
            array[numLines] = atoi(valueStr);
            numLines++;
        }

        // Assign values based on variable name
        if (strcmp(varName, "range_num_wheat_flour_containers") == 0)
        {
            range_num_wheat_flour_containers[0] = min;
            range_num_wheat_flour_containers[1] = max;
        }
        else if (strcmp(varName, "range_num_bages") == 0)
        {
            range_num_bages[0] = min;
            range_num_bages[1] = max;
        }
        else if (strcmp(varName, "period_dropping_wheat_flour_container") == 0)
        {
            period_dropping_wheat_flour_container[0] = min;
            period_dropping_wheat_flour_container[1] = max;
        }
        else if (strcmp(varName, "period_refill_planes") == 0)
        {
            period_refill_planes[0] = min;
            period_refill_planes[1] = max;
        }
        else if (strcmp(varName, "range_bags_per_distrib_worker") == 0)
        {
            range_bags_per_distrib_worker[0] = min;
            range_bags_per_distrib_worker[1] = max;
        }
    }
    fclose(file); // closing the file
}

void killAllProcesses(int *arr_pid, int size)
{
    printf("\nStart kill all processes\n");
    fflush(stdout);
    for (int i = 0; i < size; i++)
    {
        kill(arr_pid[i], SIGQUIT);
    }
}
void split_string(char *argv, int arr[])
{
    char *token = strtok(argv, " ");
    int i = 0;
    while (token != NULL)
    {
        arr[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
    }
}

int get_random_number(int min, int max)
{
    if (min > max)
    {
        printf("Error: min should be less than or equal to max.\n");
        return -1; // Return an error code
    }
    // Calculate the range and generate a random number within that range
    int range = max - min + 1;
    int randomNumber = rand() % range + min;

    return randomNumber;
}

// trim function
// this function removes the white spaces from the beginning and the end of a string
char *trim(char *str)
{
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n'))
    {
        str++;
    }
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n'))
    {
        len--;
    }
    str[len] = '\0';

    return str;
}