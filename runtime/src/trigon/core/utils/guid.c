#include "guid.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>


static char guid_symbols[] = {
    'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    '0','1','2','3','4','5','6','7','8','9'
};

static char get_random_guid_symbol() {
    int index = rand() % (sizeof(guid_symbols) / sizeof(guid_symbols[0]));
    return guid_symbols[index];
}

void guid_gen(guid_t id) {
    srand((uint32_t)(time(0)));
    for (uint32_t i = 0; i < sizeof(guid_t) - 1; i++) {
        id[i] = get_random_guid_symbol();
    }
    id[sizeof(guid_t) - 1] = '\0';
}

void serialkey_gen(serialkey_t id) {
    srand((uint32_t)(time(0)));
    uint32_t real_index = 0;
    for (uint32_t i = 0; i < sizeof(serialkey_t) ; i++) {

        if ( real_index != 0 && real_index % 4 == 0 && id[i-1] != '-') {
            id[i] = '-';
        }
        else {
            id[i] = get_random_guid_symbol();
            real_index++;
        }
        printf("%c", id[i]);
    }
    id[sizeof(serialkey_t) - 1] = '\0';
    printf("\n");
}