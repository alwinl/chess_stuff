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

#include "pnglexer.h"

#include <istream>

#include <cctype>
#include <cassert>


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








/**********************************************************
 * \brief
 *
 * Format of a tag value pair is ["<tag>" "<value"]
 *
 * Example:
 *	[Event "F/S Return Match"] : tag = Event, value = F/S Return Match
 *
 * However, the standard allows a tag value pair to span multiple lines
 * on import or even more than one pair per line.
 */
std::string::size_type PNGParser::extract_tag_pair( ChessGame& game, std::string line )
{
	// Lets find the start and end markers
	std::string::size_type start_marker = line.find_first_of("[");
	std::string::size_type end_marker = line.find_first_of("]");
	if( (start_marker == std::string::npos) || (end_marker == std::string::npos) )
		return 0;   // no markers, cannot extract a pair

	// Now that we have tag pair delimiters, lets extract tag and pair

	std::string::size_type start_tag = line.find_first_of( "\"", start_marker + 1 );
	if( start_tag == std::string::npos )
		return end_marker; // invalid sequence, consume the whole thing and discard

	std::string::size_type end_tag = line.find_first_of( "\"", start_tag + 1 );
	if( end_tag == std::string::npos )
		return end_marker; // invalid sequence, consume the whole thing and discard

	std::string::size_type start_value = line.find_first_of( "\"", end_tag + 1 );
	if( start_value == std::string::npos )
		return end_marker; // invalid sequence, consume the whole thing and discard

	std::string::size_type end_value = line.find_first_of( "\"", start_value + 1 );
	if( end_value == std::string::npos )
		return end_marker; // invalid sequence, consume the whole thing and discard

	std::string tag = line.substr( start_tag, end_tag );
	std::string value = line.substr( start_value, end_value );

	game.add_tag_pair( tag, value );

	return end_marker;
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
std::string::size_type PNGParser::extract_move( ChessGame& game, std::string movetext )
{
	// First deliniator is a period
	std::string::size_type period = movetext.find( '.' );
	if( period == std::string::npos )
		return 0;	// cannot extract a move

	//int moveno = std::atoi( movetext.substr( 0, period ).c_str() );

	// Do we have a white ply?
	if( movetext.substr(period,3) != "..." ) {
		//std::string::size_type end_of_ply = movetext.find( ' ' );

	}

	return 0;
}

ChessGame PNGParser::do_parse( std::istream& is )
{
	std::string line;
	std::string extracted_text;
	std::string::size_type chars_processed;
	bool processing_tagvalues = true;

	ChessGame game;

	while( !is.eof() ) {

		getline( is, line );

		if( line[0] == '%' )		/* Standard chapter 6 */
			continue;

		if( line.empty() ) {
			processing_tagvalues = false;
			extracted_text.clear();
			continue;
		}

		extracted_text += line;

		if( processing_tagvalues )
			while( (chars_processed = extract_tag_pair( game, extracted_text ) ) != 0 )
				extracted_text.erase( 0, chars_processed );
		else
			while( (chars_processed = extract_move( game, extracted_text ) ) != 0 )
				extracted_text.erase( 0, chars_processed );
	}

	return game;
}
