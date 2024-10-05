#include <stdint.h>

typedef unsigned long size_t;
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;
volatile unsigned short* VGA_MEMORY = (unsigned short*)0xB8000;

enum COLOR {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    LightBrown = 14,
    White = 15
};

char createColor(COLOR fg, COLOR bg) {
    return (char)((bg << 4) | fg);
}

class VGA {
public:
    int row;
    int column;
    unsigned char color;

    VGA(int row, int column, COLOR fg, COLOR bg) : row(row), column(column), color(createColor(fg, bg)) {}

    void putChar(char c, int x, int y) {
        if (c == '\n') {
            incrementRow();
            return;
        }
        const int index = y * VGA_WIDTH + x;
        VGA_MEMORY[index] = ((unsigned short)color << 8) | c;
    }

    void print(const char* str) {
        for (size_t i = 0; str[i] != '\0'; i++) {
            if (column >= VGA_WIDTH) {
                incrementRow();
            }
            putChar(str[i], column++, row);
        }
    }

    void println(const char* str) {
        print(str);
        incrementRow();
    }

    void printNumber(unsigned int num) {
        char buffer[16];
        int i = 0;
        if (num == 0) {
            putChar('0', column++, row);
            return;
        }
        while (num > 0) {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        }
        while (i > 0) {
            putChar(buffer[--i], column++, row);
        }
    }

    void incrementRow() {
        row++;
        column = 0;
        if (row >= VGA_HEIGHT) {
            // Push all rows up by one
            for (int y = 0; y < VGA_HEIGHT - 1; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    const int index = y * VGA_WIDTH + x;
                    const int nextIndex = (y + 1) * VGA_WIDTH + x;
                    VGA_MEMORY[index] = VGA_MEMORY[nextIndex];
                }
            }
            // Clear the last row
            for (int x = 0; x < VGA_WIDTH; x++) {
                putChar(' ', x, VGA_HEIGHT - 1);
            }
            row = VGA_HEIGHT - 1;
        }
    }

    void clear() {
        for (int y = 0; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                putChar(' ', x, y);
            }
        }
    }

    void setColor(COLOR fg, COLOR bg) {
        color = createColor(fg, bg);
    }

    void setCursor(int x, int y) {
        row = y;
        column = x;
    }


};

// MSR definitions and CPU frequency retrieval
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


enum Status {
    OK=0,
    ERROR=1,
    INFO=2,
};

void printStatus(VGA& vga, Status status, const char* message) {
    vga.print("[");
    if (status == OK) {
        vga.setColor(COLOR::Green, COLOR::Black);
        vga.print("OK");
    } else if (status == ERROR) {
        vga.setColor(COLOR::LightRed, COLOR::Black);
        vga.print("ERROR");
    } else if (status == INFO) {
        vga.setColor(COLOR::LightBlue, COLOR::Black);
        vga.print("INFO");
    }
    vga.setColor(COLOR::White, COLOR::Black);
    vga.print("] ");
    vga.print(message);
    vga.print("\n");
}



// Main kernel function
extern "C" void kernel_main() {
    VGA vga(0, 0, COLOR::White, COLOR::Black);
    vga.clear();

    printStatus(vga, Status::INFO, "Kernel Loaded");

    Timer timer(get_cpu_base_frequency());

    printStatus(vga, Status::OK, "Timer Started");

    printStatus(vga, Status::ERROR, "Oh noes! An error occurred!");

    vga.println("This is my program so far :P");

    // Halt the CPU
    while (1) {
        __asm__("hlt");
    }
}
