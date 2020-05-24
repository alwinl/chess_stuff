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
//#include <iostream>

using namespace std;

FENTranslator::FENTranslator()
{
    //ctor
}

map<STSquare,STPiece> FENTranslator::from_FEN( std::string FENstring )
{
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

            content.insert( std::pair<STSquare, STPiece>( square, piece) );
            ++square.file;
        }
    }

    return content;
}

std::string FENTranslator::to_FEN()
{
    ostringstream ss;
    char ch;

    for( int rank=7; rank >= 0; --rank ) {
        int blanks = 0;
        for( int file=0; file < 8; ++file ) {
            if( (ch = query_square( make_square(file, rank) )) != ' ' ) {
                if( blanks ) {
                    ss << blanks;
                    blanks = 0;
                }
                ss << ch;
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

bool FENTranslator::remove_from_square( STSquare square )
{
    map<STSquare, STPiece>::iterator it = content.find( square );
    if( it == content.end() )
        return false;

    content.erase( it );
    return true;
}

bool FENTranslator::add_to_square( STSquare square, char code )
{
    map<STSquare, STPiece>::iterator it = content.find( square );
	if( it != content.end() )
		return false;

	STPiece piece;

	piece.code = code;
	piece.is_white = ( string("KQRBNP").find( code ) != std::string::npos );

    content.insert( std::pair<STSquare, STPiece>( square, piece) );
    return true;
}

char FENTranslator::query_square( STSquare square )
{
    map<STSquare, STPiece>::iterator it = content.find( square );
    if( it != content.end() )
        return (*it).second.code;

    return ' ';
}

