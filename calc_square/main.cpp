#include <iostream>

using namespace std;

int main()
{
    int square = 0;
    char str[3];

    str[2] = '\0';
    do {
        str[0] = 'a' + square % 16;
        str[1] = '1' + square / 16;

        cout << "Square = " << str << endl;

        square ^=  0x77;

        if( !( square & 4 ) ) {
            if( square >= 0x70 )
                square = ( square + 0x11 ) & 0x73;
            else
                square += 0x10;
        }

    } while( square );

    return 0;
}
