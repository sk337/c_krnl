#include <stdint.h>
#include "VGA.hpp"


extern "C" void kernel_main() {
    VGA vga(0, 0, COLOR::White, COLOR::Black);
    vga.clear();

    vga.printStatus(Status::INFO, "Kernel Loaded");

    vga.printStatus(Status::OK, "Timer Started");

    vga.printStatus(Status::ERROR, "Oh noes! An error occurred!");
    vga.println("This is my program so far :P");

    while (1) {
        __asm__("hlt");
    }
}
