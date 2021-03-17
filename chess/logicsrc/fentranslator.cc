/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "fentranslator.h"
#include "../ADTsrc/pods.h"

#include <sstream>

using namespace std;

map<STSquare,STPiece> FENTranslator::from_FEN( std::string FENstring )
{
	std::map<STSquare, STPiece> content;

    STSquare square = make_square(0,7);         // first = file, second is rank

    for( char& code : FENstring ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
           square.file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --square.rank;
            square.file = 0;
        } else {								  // or an piece designator
        	STPiece piece;

        	piece.code = code;
			piece.is_white = ( string("KQRBNP").find( code ) != std::string::npos );
			piece.is_dragging = false;

            content.insert( std::pair<STSquare, STPiece>( square, piece) );
            ++square.file;
        }
    }

    return content;
}

std::string FENTranslator::to_FEN( std::map<STSquare, STPiece> content )
{
    ostringstream ss;

    for( int rank=7; rank >= 0; --rank ) {

        int blanks = 0;

        for( int file=0; file < 8; ++file ) {

			map<STSquare, STPiece>::iterator it = content.find( make_square(file, rank) );
			if( it != content.end() ) {
                if( blanks ) {
                    ss << blanks;
                    blanks = 0;
                }
                ss << (*it).second.code;
			} else
                ++blanks;
        }

        if( blanks )
            ss << blanks;

        if( rank )
            ss << "/";
    }

    return ss.str();
}
