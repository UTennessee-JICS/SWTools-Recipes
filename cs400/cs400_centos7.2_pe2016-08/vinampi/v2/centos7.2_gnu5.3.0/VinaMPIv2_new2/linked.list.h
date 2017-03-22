/*This code was obtained from http://www.thegeekstuff.com/2012/08/c-linked-list-example/*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>


struct test_struct
{
    int val;
    struct test_struct *next;
};

struct test_struct *head;
struct test_struct *curr;
struct test_struct *iter;

struct test_struct* create_list(int val);

struct test_struct* add_to_list(int val, bool add_to_end);

struct test_struct* search_in_list(int val, struct test_struct **prev);

void update_current(void);

int delete_from_list(int val);

void print_list(void);
