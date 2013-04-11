/*****************************************************************************
 *
 *     obsess_book.c   
 *
 *   Description: This file contains all the api code that will controll the 
 *                obsess book.  The file was specifically designed to conform
 *                with the specifications layed out in the underhanded-c contest
 *                for 2013 found here.  http://underhanded.xcott.com/?page_id=5.
 *
 *   Author: O'Ryan Anderson
 *
 *   Date:        4/10/2013
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
#define MAX_DREPCON 5

//A Prime number used to calculate the bucket hash.
#define HASH_PRIME  13

//Filter number used to calculate the bucket hash.
#define HASH_FILTER 6

//Check the user X parameter to make sure the User X parameter is valid and it's
//hash_value is sane.
#define CHECK_USER_PARAM_X (!(x &&  x->scratch < BUCKET_LEN))

//Check the user Y parameter to make sure the User Y parameter is valid and it's
//hash_value is sane.
#define CHECK_USER_PARAM_Y ((!y && y->scratch < BUCKET_LEN))
//_____________________________________________________________________________
//                                                                        Types

//Structure to define a user. Explicitly part of the contest, i would use a linked
//list for the BFFs so as to not do so many reallocs.  Probably implement a free list.
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
   user                   *data;
}user_list_node;

//control block structure used to hold all the special data of the obsess book app.
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
static void delete_user(user *usr);
static user_ret_code ob_add_BFF_helper(user *who, user *bff);
//_____________________________________________________________________________
//                                                             Public Functions 

/******************************************************************************
 * Function:    ob_init
 *
 * Description: initialize the control block and surrounding data structures.
 *
 * Params:      None.
 *
 * Returns:     obsess_book_cb* - pointer to the Obsess book cb.
 *
 * Notes:       None.
 *
 *****************************************************************************/
obsess_book_cb* ob_init(void)
{
   obsess_book_cb *cb;
   int i;
   //allocate a new cb structure
   cb = (obsess_book_cb*)malloc((int)sizeof(struct _obsess_book_cb) * sizeof(char));
   
   //Initialize values
   if(cb != NULL)
   {
      cb->static_id = 0L;
      for(i = 0;i< BUCKET_LEN;i++)
      {
         cb->user_list[i] = NULL;
      }
      memset(cb->padding,0L,sizeof(cb->padding));
   }
   return cb;
}
/******************************************************************************
 * Function:      ob_exit 
 *
 * Description:   Cleanup the obsess book.
 *
 * Params:        obsess_book_cb *cb - control block pointer.
 *
 * Returns:       None
 *
 * Notes:         once this call returns cb will no longer be valid.
 *
 *****************************************************************************/
void ob_exit(obsess_book_cb *cb)
{
   user_list_node *un;
   user_list_node *un_next;
   int i;

   //Look at each bucket and delete the user for each user.
   for(i = 0;i< BUCKET_LEN;i++)
   {
      un = cb->user_list[i];
      //each user in the bucket shall be deleted.
      while(un != NULL)
      {
         if(un->data != NULL)
         {
            delete_user(un->data);
         }

         //save the next pointer.
         un_next = un->next;
         //free the node
         free(un);
         //go to next.
         un = un_next;
      }
   }
   if(cb != NULL)
   {
      free(cb);
   }
}


/******************************************************************************
 * Function:    ob_new_user() 
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
 *              I use goto's in any function that can have errors and need to cleanup
 *              on error.  This makes the code more readable, less indented, and
 *              more efficient.  Also, it make it more likely that the cleanup will 
 *              happen on error.
 *
 *****************************************************************************/
user* ob_new_user(obsess_book_cb *cb,char *name, char *ah)
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
   
   //Allocate a new User structure.
   new_user = (user*)malloc(sizeof(struct user_struct)*sizeof(char));
   if(new_user == NULL)
   {//NO MEM.
      goto EXIT_add_user_0;
   }

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
   
   //Put it in a bucket and make it unique.
   user_node = malloc(sizeof(cb->user_list));
   user_node->data = new_user;

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
 * Function:      ob_add_BFF
 *
 * Description:   add BFF to the user's list of BFFs
 *
 * Params:        user *who - pointer to the user to whom the BFF is to be added.
 *                user *bff - pointer to the user who is to be the BFF.
 *
 * Returns:       user_ret_code USER_SUCCESS - bff added
 *                              USER_ALREADY_BFF - bff already a bff.
 *
 * Notes:         Just call the helper function to create Bffs.
 *
 *****************************************************************************/
user_ret_code ob_add_BFF(user *who, user *bff)
{
   user_ret_code rc;

   //Pair bffs as the request came from outside the obsess book system.
   rc = ob_add_BFF_helper(who, bff);
   if(rc == USER_SUCCESS)
   {
      //Add me as my BFF's BFF
      ob_add_BFF_helper(bff, who);
   }

   return rc;
}


/******************************************************************************
 * Function:      ob_find_user
 *
 * Description:   Function finds a user specified by the name.
 *
 * Params:        obsess_book_cb* - pointer to the obsess_book control block.
 *                char *name - pointer to the name you are looking for.
 *
 * Returns:       user* - pointer to the user structure.
 *
 * Notes:         None.
 *
 *****************************************************************************/
user* ob_find_user(obsess_book_cb *cb,char *name)
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
 * Notes:         Small change to contest rules, i defined this with pointers to
 *                the structures instead of passing the structures though the stack.
 *                Also, i made the DERPCON 0 based so 0 means BFF, 6 means no link.
 *
 *****************************************************************************/
int DERPCON(user *x, user *y)
{
   int derpcon_ret;

   //check the parameters.
   if (CHECK_USER_PARAM_X)
   {
      return -USER_INVALID_X;
   }

   if (CHECK_USER_PARAM_Y)
   {
      return -USER_INVALID_Y;
   }

   // call the recursive helper function.
   derpcon_ret = DERPCON_helper(x,y, 0);
   printf("%s -> %s derpcon = %d\n",x->name,y->name,derpcon_ret);
   return derpcon_ret;
}

/******************************************************************************
 * Function:      ob_dump_data()
 *
 * Description:   Debug function to dump the data structures in Obsess book.
 *
 * Params:        obsess_book_cb *cb - pointer to the obsess book control block.
 *
 * Returns:       None.
 *
 * Notes:         Outputs to stdout.
 *
 *****************************************************************************/
void ob_dump_data(obsess_book_cb *cb)
{
   int i;

   printf("-- Dumping Data --\n");
   for(i = 0; i < BUCKET_LEN;i++)
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
 *
 * Notes:         Algorithm:
 *                Want to return the lowest possible DERPCON.  Use recursion to 
 *                Search the friends of x or reuturn the max depth if not found
 *                or return the current depth if the user is in the BFF list.
 *
 *****************************************************************************/
static int DERPCON_helper(user *x, user *y, int depth)
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
 * Function:      ob_add_BFF_helper
 *
 * Description:   function to do the work of creating BFFs.
 *
 * Params:        user *who - pointer to the user to whom the BFF is to be added.
 *                user *bff - pointer to the user who is to be the BFF.
 *
 * Returns:       user_ret_code USER_SUCCESS - bff added
 *                              USER_ALREADY_BFF - bff already a bff.
 *
 * Notes:         Uses realloc as the BFF list was specificed by the Contest,
 *                I would use a linked list with a free list.
 *
 *****************************************************************************/
static user_ret_code ob_add_BFF_helper(user *who, user *bff)
{
   int i;
   
   //Look for duplicate
   for(i =0;i<who->number_of_BFFs;i++)
   {
      if(who->BFF_list[i]->user_ID == bff->user_ID)
      {//oops already a user
         printf("ERROR - Already a BFF.\n");
         return -USER_ALREADY_BFF;
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
 * Notes:         Hash values are not unique per user.
 *
 *****************************************************************************/
static int generate_hash(char *name, int name_size)
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
   //Each bucket will be a list of users.  Hash function designed to give a 
   //large distrabution of hash values.
   hash_val = ((id * HASH_FILTER % BUCKET_LEN) * HASH_PRIME) / HASH_FILTER;
   
   //incase any are out of bounds (should never happen)
   if(hash_val > BUCKET_LEN + 1)
   {
      hash_val %= BUCKET_LEN;
   }

   // Return the hash value generated.
   return hash_val;
}

/******************************************************************************
 * Function:      delete_user
 *
 * Description:   Delete a user structure and free all allocated memory that
 *                were allocated during creation.
 *
 * Params:        user *user - pointer to the structure to free.
 *
 * Returns:       None.
 *
 * Notes:         When the function returns, usr is no longer valid.
 *
 *****************************************************************************/
static void delete_user(user *usr)
{
   if(usr != NULL)
   {//Delete user
      if(usr->name != NULL)
      {//Delete Name
         free(usr->name);
      }
      if(usr->account_handle != NULL)
      {//Delete account Handle
         free(usr->account_handle);
      }
      if(usr->BFF_list != NULL)
      {//Delete BFF list
         free(usr->BFF_list);
      }
      free(usr);
   }
}
/******************************************************************************
 * Function:      print_bucket() 
 *
 * Description:   Print all the users in the bucket.
 *
 * Params:        user_list_node *ul-pointer to the node of the users to print.
 *
 * Returns:       None.
 *
 * Notes:         outputs to stdout.
 *
 *****************************************************************************/
static void print_bucket(user_list_node *ul)
{
   int i;

   while(ul != NULL)
   {
      printf("---------------------------------\n");
      printf("ul = %p\n",ul);
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

