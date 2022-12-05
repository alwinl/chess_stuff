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
#include <map>
#include <vector>
#include "chessgame.h"

#include <pgntoken.h>

class PGNParser
{
public:
    bool do_parse( std::istream& is, ChessGame& game );

	std::vector<PGNToken> Tokenise( std::istream& is );

private:
	static std::map<std::string, std::string> tags;

	std::string::size_type extract_tag_pair( ChessGame& game, std::string line );
	std::string::size_type extract_move( ChessGame& game, std::string movetext );
};

#endif // PNGPARSER_H
