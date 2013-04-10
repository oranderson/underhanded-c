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
#include <stdlib.h>
#include <string.h>
#include "obsess_book.h"
//_____________________________________________________________________________
//                                                                      Defines
//Number of buckets to put users into.
#define BUCKET_LEN 575

//Maximum number of edges that can seperate BFFs until users are considerd strangers.
#define MAX_DREPCON 6

//A Prime number used to calculate the bucket hash.
#define HASH_PRIME  13

//Filter number used to calculate the bucket hash.
#define HASH_FILTER 6
//_____________________________________________________________________________
//                                                                        Types

//Structure to define a user.
struct user_struct {
  int user_ID;
  char * name;
  char * account_handle;
  int number_of_BFFs;
  user **BFF_list;
  int scratch;
};

//Structure used to define a user node.
typedef struct _user_list_node
{
   struct _user_list_node *prev;
   struct _user_list_node *next;
   user      *data;
}user_list_node;

struct _obsess_book_cb
{
   //unique id to give to each user.
   long static_id;
   //Array of bucket Lists to put each user into.  The list the user is put into
   //is determined by a hash function.  
   user_list_node *user_list[BUCKET_LEN];
   //Padding to ensure future structure fit in the same memory footprint.
   long padding[16];
};
//_____________________________________________________________________________
//                                                                       Static
//_____________________________________________________________________________
//                                                                      Globals 
//_____________________________________________________________________________
//                                                            Private Functions
static int DERPCON_helper(user *x, user *y, int depth);
static int generate_hash(char *name, int name_size);
static void print_bucket(user_list_node *ul);
static struct _obsess_book_cb ob_cb;
//_____________________________________________________________________________
//                                                             Public Functions 

obsess_book_cb* init_obsess_book(void)
{
   obsess_book_cb *cb;
   int i;

   cb = &ob_cb;
   if(cb != NULL)
   {
      cb->static_id = 0L;
      for(i = 0;i< BUCKET_LEN;i++)
      {
         cb->user_list[i] = NULL;
      }
      memset(cb->padding,0L,sizeof(long)*sizeof(cb->padding));
   }
   return cb;
}

/******************************************************************************
 * Function:    new_user() 
 *
 * Description: Function adds a new user to the user database.
 *
 * Params:      char *name - pointer to the name of the new user.
 *              char *ah - pointer to the account handle of the new user.
 *
 * Returns:     pointer to the newly created user or NULL if there is a problem.
 *
 * Notes:       new user is created and added to the database, the pointer to 
 *              the user structure is returned.
 *
 *****************************************************************************/
user* new_user(obsess_book_cb *cb,char *name, char *ah)
{
   user *new_user = NULL;        //Pointer to the new user.
   user_list_node *user_node = NULL;   //Pointer to the user node.
   int   name_size = 0;          //number of characters in the name.
   int   ah_size = 0;            //number of characters in the account handle.
   int   hash_val = 0;           //Value to hash

   //Parameter Checking.
   if(name == NULL)
   {
      goto EXIT_add_user_0;
   }
   
   if(ah == NULL)
   {
      goto EXIT_add_user_0;
   }
   printf("<%s, %d>** %s() %d \n",__FILE__,__LINE__,__FUNCTION__,sizeof(struct user_struct));
   
   //Allocate a new User structure.
   new_user = malloc(sizeof(struct user_struct));
   if(new_user == NULL)
   {//NO MEM.
      goto EXIT_add_user_0;
   }

printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   
   //Fill in name
   name_size = strlen(name);
   new_user->name = malloc(name_size+1);
   if(new_user->name == NULL)
   {//no Mem
      goto EXIT_add_user_1;
   }
   //Safely copy the new user's name into the structure.
   strncpy(new_user->name,name,name_size);
   new_user->name[name_size] = '\0';
   
   printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   //Fill in Account Handle
   ah_size = strlen(ah);
   new_user->account_handle = malloc(ah_size);
   if(new_user->account_handle == NULL)
   {
      goto EXIT_add_user_2;
   }
   //Safely copy the new user's account handle into the structure.
   strncpy(new_user->account_handle,name,ah_size);
   new_user->name[ah_size] = '\0';

   //Initilaize BFFs
   new_user->number_of_BFFs = 0;
   new_user->BFF_list = NULL;

   //Initialize User_ID;
   new_user->user_ID = cb->static_id++;

   //Generate a new hash value and put it in a bucket.
   hash_val = generate_hash(new_user->name,name_size);

   //initialize scratch
   new_user->scratch = hash_val;
   
   printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   //Put it in a bucket and make it unique.
   user_node = malloc(sizeof(cb->user_list));
   printf("<%s, %d>** %s()\n",__FILE__,__LINE__,__FUNCTION__);
   user_node->data = new_user;

   printf("inserting new user %s into %d\n",new_user->name,hash_val);
   //Just insert at at the head.
   if(cb->user_list[hash_val] == NULL)
   {
      user_node->prev = NULL;
      user_node->next = NULL;
   }
   else
   {
      cb->user_list[hash_val]->prev = user_node;
      user_node->next = cb->user_list[hash_val];
   }
   //Set the user data in the node.
   cb->user_list[hash_val] = user_node;


   //jump over the error processing code and 
   //return a pointer to the new user.
   goto EXIT_add_user_0;

EXIT_add_user_2:
   free(new_user->name);
EXIT_add_user_1:
   free(new_user);
   new_user = NULL;
EXIT_add_user_0:
   return new_user;
}



/******************************************************************************
 * Function:      add_BFF
 *
 * Description:   add BFF to the user's list of BFFs
 *
 * Params:        user *who - pointer to the user to whom the BFF is to be added.
 *                user *bff - pointer to the user who is to be the BFF.
 *
 * Returns:       user_ret_code USER_SUCCESS - bff added
 *                              USER_ALREADY_BFF - bff already a bff.
 *
 * Notes:         None.
 *
 *****************************************************************************/
user_ret_code add_BFF(user *who, user *bff)
{
   int i;
   
   //Look for duplicate
   for(i =0;i<who->number_of_BFFs;i++)
   {
      if(who->BFF_list[i]->user_ID == bff->user_ID)
      {//oops already a user
         printf("err already a bff\n");
         return USER_ALREADY_BFF;
      }
   }
   
   //Add the BFF to the list of the user's bffs.
   who->number_of_BFFs++;
   who->BFF_list = realloc(who->BFF_list,sizeof(user*) * who->number_of_BFFs);

   //Inser the BFF at the end of the array.
   who->BFF_list[who->number_of_BFFs-1] = bff; 
   return USER_SUCCESS;
}

/******************************************************************************
 * Function: 
 *
 * Description:
 *
 * Params:
 *
 * Returns:
 *
 * Notes:
 *
 *****************************************************************************/
user* findUser(obsess_book_cb *cb,char *name)
{
   int hashVal = generate_hash(name,strlen(name));
   user_list_node *ul;

   ul = cb->user_list[hashVal];

   while(ul != NULL)
   {
      if(strcmp(name,ul->data->name) == 0)
      {//found it, return
         return ul->data;
      }
      ul = ul->next;
   }
   printf("could not find user\n");

   //User not found.
   return NULL;
}


/******************************************************************************
 * Function:      DERPCON
 *
 * Description:   Function evealuates the DERPCON between the 2 users.
 *
 * Params:        user *x - ptr to user to whom to evaluate the DERPCON
 *                user *y - ptr to user to whom to evaluate the DERPCON
 *
 * Returns:       0-6 the DERPCON value of the BFFs or
 *                >0  Error.
 *
 * Notes:         None.
 *
 *****************************************************************************/
int DERPCON(user *x, user *y)
{
   int derpcon_ret;
   //check the parameters.
   if (x->scratch > BUCKET_LEN)
   {
      return -USER_INVALID_X;
   }

   if (y->scratch > BUCKET_LEN)
   {
      return -USER_INVALID_Y;
   }

   // call the recursive helper function.
   derpcon_ret = DERPCON_helper(x,y, 0);
   printf("%s -> %s derpcon = %d\n",x->name,y->name,derpcon_ret);
   return derpcon_ret;
}

/******************************************************************************
 * Function: 
 *
 * Description:
 *
 * Params:
 *
 * Returns:
 *
 * Notes:
 *
 *****************************************************************************/
void dump_data(obsess_book_cb *cb)
{
   int i;

   printf("-- Dumping Data --\n");
   for(i = 0; i < BUCKET_LEN + 2;i++)
   {
      user_list_node *ul = cb->user_list[i];
      printf("\n***********************************\n");
      printf("***********************************\n");
      printf("printing bucket %d\n",i);
      printf("***********************************\n");
      print_bucket(ul);
      printf("***********************************\n");
      printf("***********************************\n");
   }
}

//_____________________________________________________________________________
//                                                            Private Functions

/******************************************************************************
 * Function:      DERPCON_helper
 *
 * Description:   Recursive helper function used to calculate the DERPCON of the
 *                two BFFs.
 *
 * Params:        user *x - pointer to the user to calculate the DERPCON.
 *                user *y - pointer to the user to calculate the DERPCON.
 *                int depth - Current depth of the recursive dive.
 *
 * Returns:       int 0 - 6 the DERPCON level between the BFFs.
 *                    -1    not a common friend.
 *
 * Notes:         Algorithm:
 *                Want to return the lowest possible DERPCON.  User recursion to 
 *                Search the friends of x.
 *
 *****************************************************************************/
int DERPCON_helper(user *x, user *y, int depth)
{
   int derpcon = MAX_DREPCON;       //Return derpcon of the friend.
   int derpcon_ret = MAX_DREPCON;   //The lowest derpcon returned by the friend.   
   int i;

   //More than 6 edges seperates, return a 6.
   if(depth == MAX_DREPCON)
   {//Maximum depth reached, return the MAX.
      derpcon_ret = depth;
   }
   else
   {
      //For each friend, x and y are either friends and the current depth is returned
      //if the current friend is not a bff, search the friends list of BFFS.
      for(i = 0; i < x->number_of_BFFs;i++)
      {
         if(x->BFF_list[i]->user_ID == y->user_ID)
         {//found BFF return depth i.e. DREPCON.
            derpcon_ret = depth;
            break; 
         }
         else
         {//not a BFF, check friend's friends.
            derpcon = DERPCON_helper(x->BFF_list[i],y,depth+1);

            //Check the returned derpcon to see if it is a new low.
            if(derpcon < derpcon_ret)
            {//derpcon is less than the lowest derpcon, so it is the new derpcon.
               derpcon_ret = derpcon;
            }
         }
      }
   }

   //Return the found derpcon.
   return derpcon_ret;
}

/******************************************************************************
 * Function:      generate_hash.
 *
 * Description:   Generate the hash value to put each user into the correct list
 *                of users.
 *
 * Params:        char *name - the name of the user.
 *                int   name_size - number of characters in the user's name.
 *
 * Returns:       int - the hash value generated.
 *
 * Notes:
 *
 *****************************************************************************/
int generate_hash(char *name, int name_size)
{
   int hash_val;
   int i;
   int id=0;

   //add up all the characters int the user's name to seed the hash.
   for(i = 0; i < name_size;i++)
   {
      id += name[i];
   }

   //Hash function will put each name into a bucket for easy searching.
   //Each bucket will be a list of users.
   hash_val = ((id * HASH_FILTER % BUCKET_LEN) * HASH_PRIME) / HASH_FILTER;
   
   //incase any are out of bounds (should never happen)
   if(hash_val > BUCKET_LEN + 1)
   {
      hash_val %= BUCKET_LEN;
   }

   // Return the hash value generated.
   return hash_val;
}

void print_bucket(user_list_node *ul)
{
   int i;

   printf("ul = %p\n",ul);
   while(ul != NULL)
   {
      printf("---------------------------------\n");
      printf("prev = %p\n",ul->prev);
      printf("next = %p\n",ul->next);
      printf("node = %p\n",ul->data);
      printf("node->id = %d\n",ul->data->user_ID); 
      printf("node->name = %s\n",ul->data->name);
      printf("node->account_handle = %s\n",ul->data->account_handle);
      printf("note->number_of_BFFs = %d\n",ul->data->number_of_BFFs);
      printf("node->BFF_list = %p\n",ul->data->BFF_list);
      for(i = 0; i < ul->data->number_of_BFFs;i++)
      {
         printf("bff %p name = %s\n",ul->data->BFF_list[i],ul->data->BFF_list[i]->name);
      }
      printf("node->scratch = %d\n",ul->data->scratch);
      ul = ul->next;
   }
}

