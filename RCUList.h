#pragma once
#include "stdlib.h"
#include <stdio>
typedef struct node
{
    int data;
    node *next;
    node *last;
} node;

class RCUList
{
    node *head;
    node *tail;

  public:
    RCUList()
    {
        head = nullptr;
        tail = head;
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
        if (head->next != nullptr)
            head->next->last = nullptr;
        head = head->next;
        myFree(temp);
    }

    int remove_from_end()
    {
        node *temp = tail;
        tail->last->next = nullptr;
        tail = tail->last;
        myFree(temp);
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
            while (current != nullptr && count < startPoint)
            {
                current = current->next;
                largest = current;
                stop = current;
                ++count;
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
            insert_at_beginning(largest->data);
            //deletes the old node
            //      probably block for a grace period here to make sure list
            //integrity is maintained
            largest->last->next = largest->next;
            largest->next->last = largest->last;
            myFree(largest);
            std::cout << ++startPoint << std::endl;
        }
    }

    bool lookup(int target)
    {
        node *current = head;
        while (current != nullptr)
        {
            if (current->data == target)
                return true;
            current = current->next;
        }
        return false;
    }

  protected:
    void myFree(node *n)
    {
        free(n);
    }
};
