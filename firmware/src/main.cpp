#include <cstdint>

int main(void)
{
    volatile uint32_t counter = 0;
    for(;;)
        counter = counter + 1;
}

