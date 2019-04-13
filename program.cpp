#include "RCUList.h"
#include <random>
#include <iostream>
int g_size = 10;
int g_threads = 1;
long g_start_time = 0;
long g_end_time = 0;
int g_print_level = 1;
int *missCount;
int *readCount;
int *items;
RCUList list;
void ProcessArgs(int argc, char *argv[])
{
}
void OutputStats()
{
}
void ValidateList()
{
    std::cout << "Validating List" << std::endl;
    int itemCount = 0;
    bool listCorrect = true;
    int current = 0;
    int last = list.remove_from_beginning();
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
    ProcessArgs(argc, argv);
    items = new int[g_size];

    for (int i = 0; i < g_size; i++)
    {
        items[i] = rand();
        list.insert_at_beginning(items[i]);
    }
    // create a thread that sorts the list
    list.sort();
    // create reader threads that read until the sort is done
    //      pick random element of the array
    //      lookup the element
    //      inc if there was a miss
    ValidateList();
    OutputStats();
    delete[] items;
}