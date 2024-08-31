#include <iostream>

using namespace std;

#if 0
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

#endif

#include <cstring>
/****************************************************************************/
struct ATTACKTABTYPE {
    char  pieceset;               /* A set of king..pawn. gives the pieces, which can move to the square */
    char direction;              /* The direction from the piece to the square */
};

enum ENUMPIECE { no_piece, king, queen, rook, bishop, knight, pawn };
int DirTab[8] = { 0x01, -0x01, 0x10, -0x10, 0x11, -0x11, 0x0f, -0x0f };
int KnightDir[8] = {0x0E, -0x0E, 0x12, -0x12, 0x1f, -0x1f, 0x21, -0x21};

static ATTACKTABTYPE attack[0xF0];
static ATTACKTABTYPE *AttackTab = &attack[0x78];

void CalcAttackTab( void )
{
    for( int index = -0x77; index <= 0x77; ++index ) {
        AttackTab[index].pieceset = 0;
        AttackTab[index].direction = 0;
    }

    for( int dir=0; dir<8; ++dir ) {
        for( int i = 1; i < 8; i++ ) {

			int index = DirTab[dir] * i;

			AttackTab[index].pieceset |= (1 << queen);								/* queen moves both diagonally and straight */
            AttackTab[index].pieceset |= ( dir < 4 ) ? (1 << rook) : (1 << bishop);	/* 0..3 are straight directions, 4..7 are diagonals */

            AttackTab[index].direction = DirTab[dir];
        }

        AttackTab[DirTab[dir]].pieceset |= (1 << king);
        AttackTab[KnightDir[dir]].pieceset |= (1 << knight);

        AttackTab[KnightDir[dir]].direction = KnightDir[dir];
    }
}

int* CalcAttackTab2( void )
{
	static int attack2[0xF0];

	memset( attack2, 0, sizeof(int) * 0xF0 );

    for( int dir=0; dir<8; ++dir ) {

        for( int i = 1; i < 8; i++ ) {

			int index = DirTab[dir] * i + 0x78;

			attack2[index] |= (1 << queen);								/* queen moves both diagonally and straight */
            attack2[index] |= ( dir < 4 ) ? (1 << rook) : (1 << bishop);	/* 0..3 are straight directions, 4..7 are diagonals */

            attack2[index] |= (DirTab[dir] << 8);
        }

        attack2[DirTab[dir]+0x78]    |= (DirTab[dir] << 8) | (1 << king);
        attack2[KnightDir[dir]+0x78] |= (KnightDir[dir] << 8) | (1 << knight);
    }

	return attack2;
}

#include <iomanip>
int main()
{
	CalcAttackTab();
	int * second = CalcAttackTab2();

	cout << setfill('0') << uppercase << hex;

	cout << "legend" << endl;

	cout << "king: " << "0x" << setw(2) << (unsigned int )(1 << king) << endl;
	cout << "queen: " << "0x" << setw(2) << (unsigned int )(1 << queen) << endl;
	cout << "rook: " << "0x" << setw(2) << (unsigned int )(1 << rook) << endl;
	cout << "bishop: " << "0x" << setw(2) << (unsigned int )(1 << bishop) << endl;
	cout << "knight: " << "0x" << setw(2) << (unsigned int )(1 << knight) << endl;
	cout << "pawn: " << "0x" << setw(2) << (unsigned int )(1 << pawn) << endl;

    cout << endl << "Piece tables:\nOriginal:" << endl;
	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) )
			cout << endl << "0x" << setw(2) << abs(i - 0x78) << ": ";
		else
			cout << "0x" << setw(2) << (unsigned int)(attack[i].pieceset & 0xFF) << " ";
	}

	cout << endl << "New:" << endl;

	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) )
			cout << endl << (( i < 120 ) ? '-' : '+') << "0x" << setw(2) << abs(i - 0x78) << ": ";
		else
			cout << "0x" << setw(2) << (unsigned int)(second[i] & 0xFF) << " ";
	}

    cout << endl << "Direction tables:\nOriginal:" << endl;
	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) )
			cout << endl << "0x" << setw(2) << abs(i - 0x78) << ": ";
		else
			cout << "0x" << setw(2) << (unsigned int)(attack[i].direction & 0xFF) << " ";
	}

	cout << endl << "New:" << endl;

	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) )
			cout << endl << (( i < 120 ) ? '-' : '+') << "0x" << setw(2) << abs(i - 0x78) << ": ";
		else
			cout << "0x" << setw(2) << (unsigned int)((second[i] >> 8) & 0xFF) << " ";
	}

	cout << endl;

	return 0;
}
