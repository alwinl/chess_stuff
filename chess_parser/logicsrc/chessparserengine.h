/*
 * ChessParser Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#ifndef CHESSPARSERENGINE_H
#define CHESSPARSERENGINE_H

#include <string>
#include <vector>
#include <utility>

class ParserVisitorBase
{
public:
	virtual void process_tag_pair( std::string tag, std::string value ) = 0;
	virtual void process_movetext( std::string white_move, std::string black_move ) = 0;
};

/** \brief
 */
class ChessParserEngine
{
public:
	bool open_file( std::string filename );

	void visit_tag_pairs( ParserVisitorBase* processor );
	void visit_movetext( ParserVisitorBase* processor );

private:
	std::vector< std::pair<std::string,std::string> > tag_pairs;
	std::vector< std::pair<std::string, std::string> > movetexts;

//	std::string parse_tag_pairs( std::string input );
//	void parse_movetext( std::string input );

	void load( std::string input );

};

#endif // CHESSPARSERENGINE_H
