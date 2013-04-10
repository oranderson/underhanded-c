
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
//_____________________________________________________________________________
//                                                                      Defines
//_____________________________________________________________________________
//                                                                        Types
//Forward declaration of obsess_book_cb;
typedef struct _obsess_book_cb obsess_book_cb;
//Forward declaration of user structure type.
typedef struct user_struct user;

//Enumeration of return codes.
typedef enum _user_ret_code
{
   USER_RET_CODE_INVALID = -1,
   USER_INVALID_X,
   USER_INVALID_Y,
   USER_INVALID_PARAMER,
   USER_ALREADY_BFF,
   USER_SUCCESS,
}user_ret_code;
//_____________________________________________________________________________
//                                                                       Static
//_____________________________________________________________________________
//                                                                      Globals 
//_____________________________________________________________________________
//                                                            Private Functions
//_____________________________________________________________________________
//                                                             Public Functions 
user*             ob_new_user(obsess_book_cb *cb,char *name, char *ah);
user*             ob_find_user(obsess_book_cb *cb,char *name);
user_ret_code     ob_add_BFF(user *who, user *bff);
int               DERPCON(user *x, user *y);
void              ob_dump_data(obsess_book_cb *cb);
obsess_book_cb*   ob_init(void);
