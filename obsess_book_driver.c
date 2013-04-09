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
#include obsess_book.h
//_____________________________________________________________________________
//                                                                        Types
typedef struct _td
{
   char *name;
   char *account_handle;
}td;
//_____________________________________________________________________________
//                                                                      Defines
#define USER(x,y) {x,y},
td user_data_list[] = {
#include "ob_data.h"
}

#define td_size = sizeof(user_data_list) / sizeof(user_data_list[0])
//_____________________________________________________________________________
//                                                                       Static
//_____________________________________________________________________________
//                                                                      Globals 
//_____________________________________________________________________________
//                                                            Private Functions
//_____________________________________________________________________________
//                                                             Public Functions 

int main (int argv, char **argc)
{
   bool exit = false;

   init_obsess_book();
   load_test_data();

   while(exit == false)
   {

   }
   exit_obsess_book();
}

int init_osess_book(void)
{
   return 1;
}

int load_test_data()
{
   int i;
   for(i = 0; i < td_size;i++)
   {
      new_user(td[i].name,td[i].account_handle);
   }

   return 1;
}

int exit_obsess_book()
{

}
