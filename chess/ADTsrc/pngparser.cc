/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

 /* https://en.wikipedia.org/wiki/Portable_Game_Notation */

 /* http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm */

#include "pngparser.h"

#include <istream>

PNGParser::PNGParser()
{
    //ctor
}

/*
std::map<std::string, std::string> PNGParser::tags = {
	{ "Event", "" },
	{ "Site", "" },
	{ "Date", "" },
	{ "Round", "" },
	{ "White", "" },
	{ "Black", "" },
	{ "Result", "" },
	{ "Annotator", "" },
	{ "PlyCount", "" },
	{ "TimeControl", "" },
	{ "Time", "" },
	{ "Termination", "" },
	{ "Mode", "" },
	{ "FEN", "" },
	{ "SetUp", "" },
};


Tokens:

	<string> ::= """ any printable character """
	<integer> ::= { <digit> }
	<movenoindicator> ::= "."
	<game_terminator> ::= "*"
	<tag_start> ::= "["
	<tag_end> ::= "]"
	<RAV_start> ::= "("
	<RAV_end> ::= ")"
	<reserved_start> ::= "<"
	<reserved_end> ::= ">"
	<NAG> ::= "$" { <digit> }
	<symbol> ::=  <letter> | <digit>  { <letter> | <digit> | <specials> }

	letter ::= [A-Za-z]
	digit ::= [0-9]
	specials ::= "_" | "+" | "#" | "=" | "-"

class Token
{
	enum eTokens { STRING, INTEGER, MOVENOINDICATOR, GAMETERMINATOR, TAGSTART, TAGEND, RAVSTART, RAVEND, RESERVEDSTART, RESERVEDEND, NAG, SYMBOL };

	eToken kind;

	std::string str_value;
	int int_value;
}

class Lexer
{

	Token get_next_token( istream& is ) {

		char ch;
		is.get( ch );


	}
}
*/


/**********************************************************
 * \brief
 *
 * Format of a tag value pair is [<tag> "<value"]
 *
 * Example:
 *	[Event "F/S Return Match"] : tag = Event, value = F/S Return Match
 *
 */
void PNGParser::extract_tag_pair( ChessGame& game, std::string line )
{
	std::string tag = line.substr( 1, line.find_first_of( ' ' ) );
	std::string::size_type value_start_pos = line.find_first_of("\"") + 1;
	std::string value = line.substr( value_start_pos, line.find_first_of("\"", value_start_pos + 1 ) );

	game.add_tag_pair( tag, value );
}


/*******************************************
 * \brief
 *
 * Standard format of a move is moveno, a period and two plys, one white ply and one black ply separated by spaces.
 * if the moveno is followed by three periods, the white Ply is missing
 * A move can be followed by an optional comment. Comments are either between brackets or between ; and eol
 *
 *
 * \param movetext std::string
 * \return std::string::size_type
 *
 */
std::string::size_type  PNGParser::extract_move( std::string movetext )
{
	// First deliniator is a period
	std::string::size_type period = movetext.find( '.' );
	if( period == std::string::npos )
		return 0;	// cannot extract a move

	int moveno = std::atoi( movetext.substr( 0, period ).c_str() );

	// Do we have a white ply?
	if( movetext.substr(period,3) != "..." ) {
		std::string::size_type end_of_ply = movetext.find( ' ' );

	}

	return 0;
}

ChessGame PNGParser::do_parse( std::istream& is )
{
	std::string line;
	std::string movetext;

	ChessGame game;

	while( !is.eof() ) {

		getline( is, line );

		if( line[0] == '%' )		/* Standard chapter 6 */
			continue;

		if( line[0] == '[' ) {
			extract_tag_pair( game, line );
			continue;
		}

		movetext += line;

		std::string::size_type chars_processed;

		while( (chars_processed = extract_move( movetext ) ) != 0 )
			movetext.erase( 0, chars_processed );
	}

	return game;
}
