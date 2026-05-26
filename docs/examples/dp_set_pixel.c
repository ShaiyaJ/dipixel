#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    Buffer* buf = dp_create_buffer(4, 4);

    // Set the pixel value at (1,3) to white 
    dp_set_pixel(buf, 1,3, 255,255,255);

    // If you draw out of bounds, then the function won't write to memory
    dp_set_pixel(buf, -1,-1, 255,0,0);
    dp_set_pixel(buf, 4,4, 255,0,0);

    // Draw it to the terminal
    dp_draw_buffer(buf, 5, 5);
    dp_free_buffer(buf);

    return 0;
}
