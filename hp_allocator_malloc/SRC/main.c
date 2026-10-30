#include <stdlib.h>
#include <string.h>
#include "hp_allocator.h"

int main(){
    int i;
    int* t = malloc(3*sizeof(int));
    int* st = hp_malloc(3*sizeof(int));

    for(i=0; i<3;i++){
        t[i]=i+1;
        st[i]=i+1;
    }

    if( ! memcmp(t,st,3))
        printf("succes\n");
    else
        printf("failure\n");

    return 0;

}
