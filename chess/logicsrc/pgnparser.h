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

#include <vector>
#include <iosfwd>

#include "chessgame.h"

class PGNToken;

class PGNParser
{
public:
    ChessGame load( std::istream& is );
    void save( std::ostream& is, ChessGame& game );

private:
	std::vector<PGNToken> tokenise( std::istream& is );

	void replace_lineterminators_with_sectionend( std::vector<PGNToken>& tokens );
	void remove_lineends( std::vector<PGNToken>& tokens );
	void replace_three_moveindicators_with_movesubstitute( std::vector<PGNToken>& tokens );

	ChessGame build_game( std::vector<PGNToken>& tokens );

	enum class CollectionState { COLLECTED_NONE, COLLECTED_TAG, COLLECTED_TAGPAIRS, COLLECTED_MOVENO, COLLECTED_WHITE };
};

#endif // PNGPARSER_H
