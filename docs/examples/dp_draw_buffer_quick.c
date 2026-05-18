#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    Buffer* buf = dp_create_buffer(10, 10);

    // dp_draw_buffer covers most drawing needs - but there is a common use case for drawing:
    // drawing to 0,0
    // This can benefit from some simple optimisations which is why dp_draw_buffer_quick exists.
    dp_draw_buffer_quick(buf);

    dp_free_buffer(buf);

    return 0;
}
