#include "VGA.hpp"

volatile unsigned short* VGA_MEMORY = (unsigned short*)0xB8000;

typedef unsigned long size_t;


VGA::VGA(int row, int column, COLOR fg, COLOR bg) : row(row), column(column), color(createColor(fg, bg)) {}

unsigned char VGA::createColor(COLOR fg, COLOR bg) {
    return (char)((bg << 4) | fg);
}

void VGA::putChar(char c, int x, int y) {
    if (c == '\n') {
        incrementRow();
        return;
    }
    const int index = y * VGA_WIDTH + x;
    VGA_MEMORY[index] = ((unsigned short)color << 8) | c;
}

void VGA::print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (column >= VGA_WIDTH) {
            incrementRow();
        }
        putChar(str[i], column++, row);
    }
}

void VGA::println(const char* str) {
    print(str);
    incrementRow();
}

void VGA::printNumber(unsigned int num) {
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

void VGA::incrementRow() {
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

void VGA::clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            putChar(' ', x, y);
        }
    }
}

void VGA::setColor(COLOR fg, COLOR bg) {
    color = createColor(fg, bg);
}

void VGA::setCursor(int x, int y) {
    row = y;
    column = x;
}

void VGA::printStatus(Status status, const char *message) {
    print("[");
    if (status == OK) {
        setColor(COLOR::Green, COLOR::Black);
        print("OK");
    } else if (status == ERROR) {
        setColor(COLOR::LightRed, COLOR::Black);
        print("ERROR");
    } else if (status == INFO) {
        setColor(COLOR::LightBlue, COLOR::Black);
        print("INFO");
    }
    setColor(COLOR::White, COLOR::Black);
    print("] ");
    print(message);
    print("\n");
}
