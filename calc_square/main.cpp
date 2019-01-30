#include <iostream>

using namespace std;

static int calc_next_square_old( int square )
{
    square ^=  0x77;

    if( !( square & 4 ) ) {
        if( square >= 0x70 )
            square = ( square + 0x11 ) & 0x73;
        else
            square += 0x10;
    }

    return square;
}


static int calc_next_square_new( int square )
{
    square ^= 0x77;

    if( !(square & 4) )
        square = ( square >= 0x70 ) ? ( square + 0x11 ) & 0x73 : square + 0x10;

    return square;
}



int main()
{
    int square = 0;
    char str[3];

    str[2] = '\0';

    do {
        str[0] = 'a' + square % 16;
        str[1] = '1' + square / 16;

        cout << "Square = " << str << " " << std::hex << square << endl;

        square ^=  0x77;

        if( !( square & 4 ) ) {
            if( square >= 0x70 )
                square = ( square + 0x11 ) & 0x73;
            else
                square += 0x10;
        } else
            cout << "Not going in there" << endl;

    } while( square );

/*
    for( int square = 0; square != 0; square = calc_next_square_new(square) ) {

        str[0] = 'a' + square % 16;
        str[1] = '1' + square / 16;

        cout << "Square = " << str << " " << std::hex << square << endl;
    }
*/
    return 0;
}
