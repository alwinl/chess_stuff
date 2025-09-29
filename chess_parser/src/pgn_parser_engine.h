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

#pragma once

#include <string>
#include <vector>
#include <utility>

class PGNParserEngine
{
public:
	bool open_file( std::string filename );

	template <typename Func> void visit_tag_pairs( Func&& func ) const { for( const auto& tag_pair : tag_pairs ) func( tag_pair.first, tag_pair.second ); }
	template <typename Func> void visit_movetext( Func&& func ) const { for( const auto& movetext : movetexts ) func( movetext.first, movetext.second ); }

private:
	std::vector< std::pair<std::string,std::string> > tag_pairs;
	std::vector< std::pair<std::string, std::string> > movetexts;

	void load( std::string input );
};
