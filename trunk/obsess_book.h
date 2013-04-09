
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
//Forward declaration of user structure type.
typedef struct user_struct user;

//Enumeration of return codes.
typedef enum _user_ret_code
{
   USER_RET_CODE_INVALID = -1;
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
user* new_user(char *name, char *ah);
ret_code add_BFF(user *who, user *bff);
int DERPCON(user x, user y);
