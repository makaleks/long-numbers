#include <iostream>
using namespace std;

#include "lnum.hpp"

int main () {
    LNum a = 0xABCDEF12345; // == 11806310474565 
    cout << a.getu64() << endl;
    a.printHex();
    cout <<  endl;
    a     -= 0x0B0D0F02040;
    a.printHex();
    cout <<  endl;
    a = 987654321;
    LNum b = 123456789;
    LNum c = a + b; // == 1111111110
    cout << c.getu64() << endl;

    return 0;
}

