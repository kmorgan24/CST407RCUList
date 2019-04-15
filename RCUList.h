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
        if (rcu_dereference(head) == nullptr)
        {
            node *temp;
            temp = (node *)malloc(sizeof(node));
            temp->next = nullptr;
            temp->last = nullptr;
            temp->data = input;
            rcu_assign_pointer(head, temp);
            rcu_assign_pointer(tail, temp);
        }
        else
        {
            node *temp;
            temp = (node *)malloc(sizeof(node));
            temp->next = head;
            temp->last = nullptr;
            temp->data = input;

            rcu_assign_pointer(head->last, temp);
            rcu_assign_pointer(head, temp);
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

            rcu_assign_pointer(tail->next, temp);
            rcu_assign_pointer(tail, temp);
        }
    }

    int remove_from_beginning()
    {
        node *temp = head;
        int rval = head->data;
        if (rcu_dereference(head->next) != nullptr)
            rcu_assign_pointer(head->next->last, nullptr);
        rcu_assign_pointer(head, head->next);
        urcu_memb_defer_rcu(&myFree, temp);
        return rval;
    }

    int remove_from_end()
    {
        node *temp = rcu_dereference(tail);
        int rval = tail->data;
        rcu_assign_pointer(tail->last->next, nullptr);
        rcu_assign_pointer(tail, tail->last);
        urcu_memb_defer_rcu(&myFree, temp);
        return rval;
    }

    void sort()
    {
        if (debugPrintLevel > 2)
        {
            std::cout << "starting sort" << std::endl;
        }

        int startPoint = 0;
        node *largest = rcu_dereference(head);
        node *current = rcu_dereference(head);
        node *stop = rcu_dereference(head);
        if (debugPrintLevel > 2)
        {
            std::cout << "starting outer while" << std::endl;
        }
        while (stop != nullptr)
        {
            largest = rcu_dereference(head);
            current = rcu_dereference(head);
            // this loop gets me to the unsorted part of the list
            int count = 0;
            if (debugPrintLevel > 2)
            {
                std::cout << "advancing to the unsorted part of the list"
                          << std::endl;
            }
            while (count < startPoint)
            {
                rcu_assign_pointer(current, current->next);
                rcu_assign_pointer(largest, current);
                rcu_assign_pointer(stop, current);
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
                if (debugPrintLevel > 2)
                {
                    std::cout << "about to mem sync" << std::endl;
                }
                // urcu_memb_synchronize_rcu();
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
        urcu_memb_read_lock(); //maybe dont exsit any more
        bool rval = false;
        node *current = head;
        while (current != nullptr)
        {
            if (current->data == target)
                rval = true;
            current = current->next;
        }
        urcu_memb_read_unlock();
        return rval;
    }

  protected:
};
static void myFree(void *n)
{
    free(n);
}
