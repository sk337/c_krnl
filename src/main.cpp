#include <stdint.h>
#include "VGA.hpp"

// MSR definitions
#define MSR_PLATFORM_INFO 0xCE

// Function to read MSR value (use assembly for rdmsr)
uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

// Function to get base CPU frequency from MSR_PLATFORM_INFO
unsigned int get_cpu_base_frequency() {
    uint64_t msr_value = read_msr(MSR_PLATFORM_INFO);

    // Bits 15:8 contain the base frequency in 100 MHz units
    unsigned int base_freq = (msr_value >> 8) & 0xFF;

    // Return the frequency in MHz
    return base_freq * 100;
}

// TSC timing code
unsigned long long rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}

struct Timer {
public:
    unsigned long long start;
    unsigned long long cpu_frequency_hz; // Store the CPU frequency in Hz

    Timer(unsigned long long cpu_frequency_hz) {
        this->cpu_frequency_hz = cpu_frequency_hz;
        start = rdtsc();
    }

    // Get elapsed time in seconds
    double elapsed_seconds() {
        return (double)(rdtsc() - start) / cpu_frequency_hz;
    }

    // Get elapsed time in milliseconds
    double elapsed_milliseconds() {
        return ((double)(rdtsc() - start) / cpu_frequency_hz) * 1000.0;
    }

    // Get elapsed time in microseconds
    double elapsed_microseconds() {
        return ((double)(rdtsc() - start) / cpu_frequency_hz) * 1000000.0;
    }



    void reset() {
        start = rdtsc();
    }
};


// Main kernel function
extern "C" void kernel_main() {
    VGA vga(0, 0, COLOR::White, COLOR::Black);
    vga.clear();

    vga.printStatus(Status::INFO, "Kernel Loaded");

    Timer timer(get_cpu_base_frequency());

    vga.printStatus(Status::OK, "Timer Started");

    vga.printStatus(Status::ERROR, "Oh noes! An error occurred!");
    vga.printNumber(timer.elapsed_microseconds());

    vga.println("This is my program so far :P");

    // Halt the CPU
    while (1) {
        __asm__("hlt");
    }
}
