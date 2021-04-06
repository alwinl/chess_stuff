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

#ifndef PNGLEXER_H
#define PNGLEXER_H

#include "pngtoken.h"

#include "ply.h"

#include <istream>
#include <map>
#include <vector>
#include <string>

class ChessGame;

class PNGLexer
{
public:
	PNGLexer( std::istream& is_ );

	void ParseGrammar();

    std::map<std::string, std::string> parse_tag_pair_section(  );
    std::vector<Ply> parse_movetext_section(  );

private:
/*
	typedef bool (PNGLexer::*FN_SIG)( PNGToken& );
	std::map<PNGToken::eTokenType, FN_SIG> token_dispatchers;

	bool process_processing( PNGToken& token );
	bool process_invalid( PNGToken& token );
	bool process_string( PNGToken& token );
	bool process_integer( PNGToken& token );
	bool process_move_no_indicator( PNGToken& token );
	bool process_game_terminator( PNGToken& token );
	bool process_tag_start( PNGToken& token );
	bool process_tag_end( PNGToken& token );
	bool process_rav_start( PNGToken& token );
	bool process_rav_end( PNGToken& token );
	bool process_reserved_start( PNGToken& token );
	bool process_reserved_end( PNGToken& token );
	bool process_nag( PNGToken& token );
	bool process_symbol( PNGToken& token );
	bool process_comment( PNGToken& token );
*/

	PNGToken get_next_token( );
	void parse_error( std::string );


	//std::pair<std::string, std::string> ParseTagPair();

	std::istream& is;
	PNGToken::eTokenType expected_type;


};

#endif // PNGLEXER_H
