//
//  main.c
//  CTetris
//
//  Created by Emanuel on 07/11/23.
//

#include "menu_main.h"
#include <stdlib.h>
#include "util.h"

int main(void) {
//    menu_main();
    
    if(search_player("manel") != NULL) {
        printf("Encontrou\n");
    } else {
        printf("Não existe\n");
    }
    
    getchar();
    
    return 0;
}
