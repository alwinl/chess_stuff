/*
 * gtest_attacktable.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <iomanip>
#include <cstring>

struct ATTACKTABTYPE {
    char  pieceset;               /* A set of king..pawn. gives the pieces, which can move to the square */
    char direction;              /* The direction from the piece to the square */
};

enum ENUMPIECE { no_piece, king, queen, rook, bishop, knight, pawn };
int DirTab[8] = { 0x01, -0x01, 0x10, -0x10, 0x11, -0x11, 0x0f, -0x0f };
int KnightDir[8] = {0x0E, -0x0E, 0x12, -0x12, 0x1f, -0x1f, 0x21, -0x21};

static ATTACKTABTYPE attack[0xF0];
static ATTACKTABTYPE *AttackTab = &attack[0x78];

void CalcAttackTab_orig( void )
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

int* CalcAttackTab_new( void )
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

TEST( Chess, do_test )
{
	CalcAttackTab_orig();
	int * second = CalcAttackTab_new();

	std::stringstream expected;
	std::stringstream actual;

	// Piece table part
	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) ) {
			expected << std::endl << "0x" << std::setw(2) << abs(i - 0x78) << ": ";
			actual   << std::endl << "0x" << std::setw(2) << abs(i - 0x78) << ": ";
		} else {
			expected << "0x" << std::setw(2) << (unsigned int)(attack[i].pieceset & 0xFF) << " ";
			actual   << "0x" << std::setw(2) << (unsigned int)((second[i] & 0xFF) & 0xFF) << " ";
		}
	}

    ASSERT_EQ( expected.str(), actual.str() ) << "Piece table parts don't match";

	// Direction table part
	for( int i = 0; i< 240; i++ ) {
		if( !(i%16) ) {
			expected << std::endl << "0x" << std::setw(2) << abs(i - 0x78) << ": ";
			actual   << std::endl << "0x" << std::setw(2) << abs(i - 0x78) << ": ";
		} else {
			expected << "0x" << std::setw(2) << (unsigned int)(attack[i].direction & 0xFF) << " ";
			actual   << "0x" << std::setw(2) << (unsigned int)((second[i] >> 8) & 0xFF) << " ";
		}
	}


    ASSERT_EQ( expected.str(), actual.str() ) << "Direction table parts don't match";
}

