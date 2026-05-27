#define DIPIXEL_IMPLEMENTATION
#include "../../dipixel.h"

int main(void) {
    Buffer* buf = dp_create_buffer(10, 10);

    // dp_draw_buffer covers most drawing needs, but perhaps you're in a very low-performance environment
    // dp_draw_buffer_quick can mitigate some of that, but if you're in an environment where you 
    // can configure the size of your terminal then this command can draw the entire buffer in a 
    // single write call
    dp_draw_buffer_single(buf);

    dp_free_buffer(buf);

    return 0;
}
