#ifndef VGA_HPP
#define VGA_HPP


// Define VGA dimensions
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

// VGA memory address
extern volatile unsigned short* VGA_MEMORY;

// Color enumeration
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

enum Status {
    OK=0,
    ERROR=1,
    INFO=2,
};

// VGA class declaration
class VGA {
public:
    // Member variables
    int row;
    int column;
    unsigned char color;

    // Constructor
    VGA(int row, int column, COLOR fg, COLOR bg);

    // Member functions
    void putChar(char c, int x, int y);
    void print(const char* str);
    void println(const char* str);
    void printNumber(unsigned int num);
    void incrementRow();
    void clear();
    void setColor(COLOR fg, COLOR bg);
    void setCursor(int x, int y);
    void printStatus(Status status, const char* message);


private:
    // Helper function to create color code
    unsigned char createColor(COLOR fg, COLOR bg);
};





#endif // VGA_HPP
