/*****************************************************************************
 *
 *   
 *
 *   Description: 
 *
 *   Author: O'Ryan Anderson
 *
 *   Date:
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
#define User(x,y) {x,y},
td user_data_list[] = {
#include "ob_data.h"
};

#define td_size (sizeof(user_data_list) / sizeof(user_data_list[0]))

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

int main (int argv, char **argc)
{
   BOOLEAN exit = FALSE;

printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   cb = init_obsess_book();
   printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   load_test_data();

 //  while(exit == FALSE)
   {

   }
   exit_obsess_book();
}

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
      my_new_user = new_user(cb,user_data_list[i].name,user_data_list[i].account_handle);
   }
   //Populate BFFs
   srand(time(NULL));

   for(i = 0; i < td_size;i++)
   {
      me = findUser(cb,user_data_list[i].name);
      //Everyone can have from 1 to 25 BFFs.
      for(j = 0;j < (rand() % 24) + 1;j++)
      {
         bff = findUser(cb,user_data_list[(rand() % td_size)].name);
         add_BFF(me,bff);
      }
   }
   dump_data(cb);

   for(i = 0;i < td_size; i++)
   {
      //me = findUser("O\'Ryan Anderson");
      me = findUser(cb,user_data_list[rand() % td_size].name);
      bff = findUser(cb,user_data_list[(rand() % td_size)].name);
      derpcon = DERPCON(me,bff);
   }


   return 1;
}

int exit_obsess_book()
{

}
