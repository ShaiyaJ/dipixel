#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    // Buffers use the `Buffer` type - when creating it, use dp_create_buffer with the width and height
    Buffer* buf = dp_create_buffer(10, 10); // The height is measured in pixels, not in character cells

    // After you've used the buffer, free it
    dp_free_buffer(buf);

    return 0;
}
