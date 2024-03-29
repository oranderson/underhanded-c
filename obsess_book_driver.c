/*****************************************************************************
 *
 *       obsess_book_driver.c 
 *
 *   Description: Test harness for the obsess book.
 *
 *   Author: O'Ryan Anderson
 *
 *   Date:  4/10/2013
 *
 *****************************************************************************/

//_____________________________________________________________________________
//                                                                     Includes
#include <stdio.h>
#include <time.h>
#include "obsess_book.h"
//_____________________________________________________________________________
//                                                                        Types
typedef struct _td
{
   char *name;
   char *account_handle;
}td;
typedef int BOOLEAN;
//_____________________________________________________________________________
//                                                                      Defines

//Easy way to get a bunch of data into an array.
#define User(x,y) {x,y},
td user_data_list[] = {
#include "ob_data.h"
};

//Size of the user data array.
#define td_size (sizeof(user_data_list) / sizeof(user_data_list[0]))

//Standard stuff not defined in the h files i used.
#define FALSE 0
#define TRUE !FALSE
//_____________________________________________________________________________
//                                                                       Static
//_____________________________________________________________________________
//                                                                      Globals 
//_____________________________________________________________________________
//                                                            Private Functions
static int load_test_data();
static obsess_book_cb *cb;
//_____________________________________________________________________________
//                                                             Public Functions 

/******************************************************************************
 * Function:     main
 *
 * Description:  entry point of the program.
 *
 * Params:       argv, argc - not used.
 * 
 * Returns:      int 0.
 *
 * Notes:        None.
 *
 *****************************************************************************/
int main (int argv, char **argc)
{
   BOOLEAN exit = FALSE;

   cb = ob_init();
   load_test_data();
   exit_obsess_book();

   return 0;
}


/******************************************************************************
 * Function:      load test data.
 *       
 * Description:   Loads all the test data, prints out the data and evaluates
 *                DERPCONs.
 *
 * Params:        None
 *
 * Returns:       int 1.
 *
 * Notes:         None
 *
 *****************************************************************************/
int load_test_data()
{
   int i;
   int j;
   user *my_new_user;
   user *me;
   user *bff;
   time_t t;
   int derpcon;

   //Create a list of users.
   for(i = 0; i < td_size;i++)
   {
      printf("creating new user %s, %s\n",user_data_list[i].name,user_data_list[i].account_handle);
      my_new_user = ob_new_user(cb,user_data_list[i].name,user_data_list[i].account_handle);
   }


   //Populate BFFs
   srand(time(NULL));

   for(i = 0; i < td_size;i++)
   {
      me = ob_find_user(cb,user_data_list[i].name);
      //Everyone can have from 1 to 25 BFFs.
      for(j = 0;j < (rand() % 24) + 1;j++)
      {
         bff = ob_find_user(cb,user_data_list[(rand() % td_size)].name);
         ob_add_BFF(me,bff);
      }
   }

   //Dump the data inserted into the obsess book.
   ob_dump_data(cb);

   //Look at some random users.
   printf("DERPCON of random people\n");
   for(i = 0;i < td_size; i++)
   {
      printf("--------\n");
      me = ob_find_user(cb,user_data_list[rand() % td_size].name);
      bff = ob_find_user(cb,user_data_list[(rand() % td_size)].name);
      derpcon = DERPCON(me,bff);
      derpcon = DERPCON(bff,me);
   }

   //Look at every user and me.
   printf("Derpcon of me and every other user.\n");
   for(i = 0;i < td_size; i++)
   {
      me = ob_find_user(cb,"O\'Ryan Anderson");
      bff = ob_find_user(cb,user_data_list[(rand() % td_size)].name);
      derpcon = DERPCON(me,bff);
      printf("derpcon of me -> bff = %d\n",derpcon);
      derpcon = DERPCON(bff,me);
      printf("derpcon of bff -> me = %d\n",derpcon);
   }

   return 1;
}

/******************************************************************************
 * Function:    exit_obsess_book
 *
 * Description: exit and cleanup of obsess_book data structures.
 *
 * Params:      None.
 *
 * Returns:     int 1;
 *
 * Notes:       None.
 *
 *****************************************************************************/
int exit_obsess_book()
{
   ob_exit(cb);
   return 1;

}
