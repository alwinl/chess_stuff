/*
 * gtest_parserengine.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtest/gtest.h>

#include "pgn_parser_engine.h"

TEST( PGNParser, parse_valid_pgn )
{
	PGNParserEngine engine;

	engine.open_file( "./game1.pgn" );

	std::string content;

	engine.visit_tag_pairs( [&content](const std::string& tag, const std::string& value)
		{ content += "Tag: " + tag + ", value: " + value + "\n"; }
	);

	engine.visit_movetext( [&content](const std::string& white_move, const std::string& black_move)
		{ content += "White: " + white_move + ", Black: " + black_move + "\n"; }
	);

	std::cout << content;

	ASSERT_TRUE( true );
}
