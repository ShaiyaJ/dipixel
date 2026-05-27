# dipixel
Dipixel is a small C library for displaying pixels in the terminal using ANSI escape sequences and the unicode half block character.

It is designed as a companion library to diseq.

## Use
Dipixel is an stb-style single header library - that means that the entire library is contained within a single header file - definitions and all! It is possible to configure some library functions using compile-time definitions - you can also opt-out of the default implementation (which depends on libc) and roll your own.

Simply include the header file in your code, and use the functions. If you want the default implementation, do `#define DIPIXEL_IMPLEMENTATION` before the `#include` directive. 

## Warning
This library is in an early beta. The API is bare-bones and subject to change. Future releases will try and maintain backwards compatibility. 
