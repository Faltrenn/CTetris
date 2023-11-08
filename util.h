//
//  utils.h
//  CTetris
//
//  Created by Emanuel on 08/11/23.
//

#ifndef util_h
#define util_h

typedef struct {
    char name[7];
    char password[10];
} Player;

void print_centered(int line_offset, int n, char* msg, int horizontal);
int key_is_letter(char key);
int verify_name_and_password(char name[7], char password[10]);
int try_login(char name[7], char password[10]);
int try_register(char name[7], char password[10]);
Player * search_player(char name[7]);

#endif /* utils_h */
