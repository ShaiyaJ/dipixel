#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    Buffer* buf = dp_create_buffer(4, 4);

    // Set the pixel value to white and draw it to the terminal at 0,0
    dp_set_pixel(buf, 1,3, 255,255,255);
    dp_draw_buffer_quick(buf);

    dp_free_buffer(buf);

    return 0;
}
