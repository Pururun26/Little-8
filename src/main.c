#include "little8.h"

void draw(void)
{
    cls(1);
    print("hello little-8!", 1, 1, 2);
}


int main(void)
{
    little8(0, 0, draw);
    return 0;
}