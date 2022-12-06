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

#ifndef PNGPARSER_H
#define PNGPARSER_H

#include <string>
#include <vector>
#include <iosfwd>

class PGNToken;

class PGNParserCollector
{
public:
	virtual ~PGNParserCollector() {};

	virtual void add_tag_pair( std::string tag, std::string value ) = 0;
	virtual void add_move_text( unsigned int moveno, std::string white_move, std::string black_move ) = 0;

	virtual void debug_token_list( std::vector<PGNToken> tokens ) {};

protected:
	PGNParserCollector() {};
};

class PGNParser
{
public:
    bool do_parse( std::istream& is, PGNParserCollector* collector );

private:
	void tokenise( std::vector<PGNToken>& tokens, std::istream& is );
	void replace_lineterminators_with_sectionend( std::vector<PGNToken>& tokens );
	void remove_lineends( std::vector<PGNToken>& tokens );
	void replace_three_moveindicators_with_movesubstitute( std::vector<PGNToken>& tokens );
	void process_tokens( std::vector<PGNToken> tokens, PGNParserCollector* collector );
};

#endif // PNGPARSER_H
