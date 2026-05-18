#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    Buffer* buf = dp_create_buffer(10, 10);

    // To draw the buffer to the screen, supply dp_draw_buffer with the target location - this will show up as a square in the terminal
    dp_draw_buffer(buf, 5, 2);

    dp_free_buffer(buf);

    return 0;
}
