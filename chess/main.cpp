/*
 * [program] Copyright 2014 Alwin Leerling <alwin@jambo>
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
 *
 *
 */


 #include "chessengine.h"

#ifdef TESTING
#include "testapplication.h"
#else
#include "chessapplication.h"
#endif

int main( int argc, char *argv[] )
{
#ifdef TESTING
    return TestApplication().run();
#else
    ChessEngine engine;
	return (ChessApplication::create( engine ))->run( argc, argv );
#endif // TESTING
}
