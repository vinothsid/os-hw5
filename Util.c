#include "Util.h"

char *itoa(int num) {
        char *str;
        str = (char *)malloc(5);
        sprintf(str,"%d",num);
        return str;
}

