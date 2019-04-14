#pragma once
#include "stdlib.h"
#include <iostream>
#define _LGPL_SOURCE
#define URCU_INLINE_SMALL_FUNCTIONS
#include <urcu/urcu-memb.h>

typedef struct node
{
    int data;
    node *next;
    node *last;
} node;
static void myFree(void *);
class RCUList
{
    node *head;
    node *tail;
    int debugPrintLevel;

  public:
    RCUList()
    {
        head = nullptr;
        tail = head;
        debugPrintLevel = 0;
        //rcu_init();
    }
    RCUList(int print_level)
    {
        head = nullptr;
        tail = head;
        debugPrintLevel = print_level;
        //rcu_init();
    }
    bool listnotempty()
    {
        if (head != nullptr)
        {
            return true;
        }
        return false;
    }
    void insert_at_beginning(int input)
    {
        if (head == nullptr)
        {
            node *temp;
            temp = (node *)malloc(sizeof(node));
            temp->next = nullptr;
            temp->last = nullptr;
            temp->data = input;
            head = temp;
            tail = temp;
        }
        else
        {
            node *temp;
            temp = (node *)malloc(sizeof(node));
            temp->next = head;
            temp->last = nullptr;
            temp->data = input;

            head->last = temp;
            head = temp;
        }
        if (debugPrintLevel > 1)
        {
            std::cout << head->data << std::endl;
        }
    }

    void insert_at_end(int input)
    {
        if (tail == nullptr)
        {
            insert_at_beginning(input);
        }
        else
        {
            node *temp;
            temp = (node *)malloc(sizeof(node));
            temp->next = nullptr;
            temp->last = tail;
            temp->data = input;

            tail->next = temp;
            tail = temp;
        }
    }

    int remove_from_beginning()
    {
        node *temp = head;
        int rval = head->data;
        if (head->next != nullptr)
            head->next->last = nullptr;
        head = head->next;
        urcu_memb_defer_rcu(&myFree, temp);
        return rval;
    }

    int remove_from_end()
    {
        node *temp = tail;
        int rval = tail->data;
        tail->last->next = nullptr;
        tail = tail->last;
        urcu_memb_defer_rcu(&myFree, temp);
        return rval;
    }

    void sort()
    {
        int startPoint = 0;
        node *largest = head;
        node *current = head;
        node *stop = head;
        while (stop != nullptr)
        {
            largest = head;
            current = head;
            // this loop gets me to the unsorted part of the list
            int count = 0;
            while (count < startPoint)
            {
                current = current->next;
                largest = current;
                stop = current;
                ++count;
                if (current == nullptr) // then stop your done
                {
                    return;
                }
            }
            //this loop finds the largest number in the list
            while (current != nullptr)
            {
                if (current->data > largest->data)
                {
                    largest = current;
                }
                current = current->next;
            }

            //moves it to the front
            if (largest != head) // there is no point in moving it
            {
                insert_at_beginning(largest->data);
                //deletes the old node
                //      probably block for a grace period here to make sure list
                //integrity is maintained
                if (largest->last != nullptr)
                    largest->last->next = largest->next;
                if (largest->next != nullptr)
                    largest->next->last = largest->last;
                urcu_memb_defer_rcu(&myFree, largest);
            }
            ++startPoint;
            if (debugPrintLevel > 2)
            {
                std::cout << startPoint << std::endl;
            }
        }
    }

    bool lookup(int target)
    {
        rcu_read_lock();
        bool rval = false;
        node *current = head;
        while (current != nullptr)
        {
            if (current->data == target)
                rval = true;
            current = current->next;
        }
        rcu_read_unlock();
        return rval;
    }

  protected:
};
static void myFree(void *n)
{
    free(n);
}
