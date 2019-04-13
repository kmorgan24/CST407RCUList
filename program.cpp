#include "RCUList.h"
#include "stdlib.h"
#include "unistd.h"
#include <random>
#include <iostream>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

int g_size = 10;
int g_threads = 1;
int g_print_level = 0;

long g_start_time = 0;
long g_end_time = 0;

int *missCount;
int *readCount;
int *items;
RCUList list;
void ProcessArgs(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "s:n:h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printf("%s\n"
                   "-h print this help message and exit\n"
                   "-s <start tree size>\n"
                   "-n <number of threads>\n"
                   "-p <print level>\n",
                   argv[0]);
            exit(1);
        case 's':
            // size
            g_size = atoi(optarg);
            break;
        case 'n':
            // nthreads
            g_threads = atoi(optarg);
            break;
        case 'p':
            g_print_level = atoi(optarg);
            break;
        }
    }
}
void OutputStats()
{
    long time = g_end_time - g_start_time;
    long reads = 0;
    long misses = 0;
    for (int i = 0; i < g_size; i++)
    {
        reads += readCount[i];
    }
    for (int j = 0; j < g_size; j++)
    {
        misses += missCount[j];
    }

    std::cout << "Sort Time: " << time << std::endl;
    std::cout << "Reads/Sec: " << reads / time << std::endl;
    std::cout << "Misses: " << misses << std::endl;
}
inline void startTiming()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    g_start_time = now.tv_sec;
}
inline void endTiming()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    g_end_time = now.tv_sec;
}
void ValidateList()
{
    std::cout << "Validating List" << std::endl;
    int itemCount = 0;
    bool listCorrect = true;
    int current = 0;
    int last = list.remove_from_beginning();
    ++itemCount;
    if (g_print_level > 0)
    {
        std::cout << last << std::endl;
    }
    while (list.listnotempty())
    {
        current = list.remove_from_beginning();
        if (g_print_level > 0)
        {
            std::cout << current << std::endl;
        }

        if (current < last)
        {
            std::cout << "error" << std::endl;
            listCorrect = false;
        }
        last = current;
        ++itemCount;
    }
    if (itemCount < g_size)
    {
        std::cout << "less items than expected" << std::endl;
        listCorrect = false;
    }
    else if (itemCount > g_size)
    {
        std::cout << "more items than expected" << std::endl;
        listCorrect = false;
    }
    if (listCorrect)
    {
        std::cout << "List is Correct" << std::endl;
    }
    else
    {
        std::cout << "list is NOT Correct" << std::endl;
    }
}
int main(int argc, char *argv[])
{
    if (g_print_level > 1)
    {
        std::cout << "Calling process args" << std::endl;
    }
    ProcessArgs(argc, argv);
    items = new int[g_size];
    missCount = new int[g_size];
    readCount = new int[g_size];

    for (int i = 0; i < g_size; i++)
    {
        items[i] = rand();
        list.insert_at_beginning(items[i]);
    }
    if (g_print_level > 1)
    {
        std::cout << "Calling start timing" << std::endl;
    }
    // create a thread that sorts the list
    startTiming();
    list.sort();
    if (g_print_level > 1)
    {
        std::cout << "Calling end Timing" << std::endl;
    }
    endTiming();
    // create reader threads that read until the sort is done
    //      pick random element of the array
    //      lookup the element
    //      inc if there was a miss
    if (g_print_level > 1)
    {
        std::cout << "Calling validate list" << std::endl;
    }
    ValidateList();
    if (g_print_level > 1)
    {
        std::cout << "Calling output stats" << std::endl;
    }
    OutputStats();
    delete[] items;
    delete[] missCount;
    delete[] readCount;
}