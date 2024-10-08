/*
 * Copyright 2023 Alwin Leerling <dna.leerling@gmail.com>
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

#include "game.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <regex>

using namespace std;

Game::Game()
{
	tag_pairs = vector<pair<string,string>>{
		pair<string,string>( "Event", "Unknown" ),
		pair<string,string>( "Site", "Unknown" ),
		pair<string,string>( "Date", "Unknown" ),
		pair<string,string>( "Round", "Unknown" ),
		pair<string,string>( "White", "Unknown" ),
		pair<string,string>( "Black", "Unknown" ),
		pair<string,string>( "Result", "*" ),
	};
}

void Game::load(std::string pgn_string)
{
	regex re_tvpair( "\\[(.*) \"(.*)\"\\]\\n" );
	smatch tvpair_match;

	tag_pairs.clear();
	moves.clear();
    initial = Board();

	while( regex_search( pgn_string, tvpair_match, re_tvpair ) ) {
		add_tag_pair( tvpair_match[1], tvpair_match[2] );
		pgn_string = tvpair_match.suffix();
	}

	regex re_movetext( "(\\d*)\\.\\s(\\S*)\\s(\\S*)?" );
	smatch movetext_match;

	Board current;

	while( regex_search( pgn_string, movetext_match, re_movetext ) ) {

		if( movetext_match[2] != "..." ) {
			add_ply( eColor::white, movetext_match[2], current );
			current = current.make( moves.back() );
		}

		if( movetext_match[3] != "" ) {
			add_ply( eColor::black, movetext_match[3], current );
			current = current.make( moves.back() );
		}

		pgn_string = movetext_match.suffix();
	}
}

std::string Game::save()
{
	auto tag_pair_accumulator = []( string collector, pair<string,string>& tag_pair )
	{
		return move(collector) + "[" + tag_pair.first + " \"" + tag_pair.second + "\"]\n";
	};

	string result = accumulate( tag_pairs.begin(), tag_pairs.end(), string(""), tag_pair_accumulator );

	result += "\n";

	Board current( initial );
	string collected;

	for( unsigned int i = 0; i < moves.size(); ++i ) {

		if( !(i%2) ) {				// even is white move

			string moveno_rep( to_string( i/2 + 1 ) + ". " );

			if( collected.length() + moveno_rep.length() > 80 ) {
				collected.pop_back();		// remove last space
				result += collected + "\n";
				collected.clear();
			}

			collected += moveno_rep;
		}

		vector<Ply> plys = current.generate_legal_plys();
		string ply_rep( moves[i].print_SAN( plys ) + " " );

		if( collected.length() + ply_rep.length() > 80 ) {
			collected.pop_back();		// remove last space
			result += collected + "\n";
			collected.clear();
		}

		collected += ply_rep;

		current = current.make( moves[i] );
	}

	result += collected;

	auto result_it = find_if( tag_pairs.begin(), tag_pairs.end(), [](pair<string,string>& tp) { return tp.first == "Result"; } );

	if( result_it == tag_pairs.end() )
		throw( domain_error( "Cannot find game result") );

	result += (*result_it).second + "\n";

	return result;
}

void Game::add_tag_pair(std::string tag, std::string value)
{
	auto it( find_if( tag_pairs.begin(), tag_pairs.end(), [tag]( auto tag_pair ) { return tag_pair.first == tag;} ) );

	if( it != tag_pairs.end() )
		(*it).second = value;
	else
		tag_pairs.push_back( make_pair(tag, value) );

	// http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm chapter 9.7 Alternative starting positions
	if( (tag == "SetUp") || ( tag == "FEN" ) )
		set_alternate_starting_position();
}

void Game::set_alternate_starting_position()
{
	auto SetUp_it( find_if( tag_pairs.begin(), tag_pairs.end(), []( auto tag_pair ) { return tag_pair.first == "SetUp";} ) );
	auto FEN_it( find_if( tag_pairs.begin(), tag_pairs.end(), []( auto tag_pair ) { return tag_pair.first == "FEN";} ) );

	if( (SetUp_it == tag_pairs.end() ) || (FEN_it == tag_pairs.end() ) )
		return;

	if( (*SetUp_it).second != "1" )
		return;

	initial = Board( (*FEN_it).second );
}

#define REVERSE_RANK_MASK 0b00111000

void Game::add_ply(eColor color, std::string SAN, Board & current)
{
	// result
	if( SAN == "1-0" || SAN == "0-1" || SAN == "1/2-1/2" )
		return;

	vector<Ply> legal_plys = current.generate_legal_plys();

	if( (SAN == "O-O") || (SAN == "O-O-O") ) {

		uint16_t start_square = 4;
		uint16_t target_square = (SAN == "O-O") ? 6 : 2;

		if( color == eColor::black ) {
			start_square ^= REVERSE_RANK_MASK;
			target_square ^= REVERSE_RANK_MASK;
		}

		// Ply test_ply( Ply::make_castle_ply( start_square, target_square ) );

		auto ply_it( find_if( legal_plys.begin(), legal_plys.end(), [start_square, target_square]( Ply the_ply) { return the_ply.check_square_match(start_square, target_square); } ) );
		// auto ply_it( find_if( legal_plys.begin(), legal_plys.end(), [test_ply]( Ply the_ply) { return the_ply.check_square_match( test_ply ); } ) );

		if( ply_it == legal_plys.end() )
			throw( domain_error( "Cannot find matching ply") );

		moves.push_back( *ply_it );

		return;
	}

	uint16_t target_square;
	Piece::eType current_type = ( SAN[0] >= 'a' && SAN[0] <= 'h' ) ? Piece::pawn : Piece(SAN[0]).get_type();
	Piece::eType promo_type = Piece::none;

	size_t length = SAN.size();
	unsigned int pos;

	for( pos = length-1; pos != 0; --pos ) {

		if( string("QRBN").find( SAN[pos] ) != string::npos )
			promo_type = Piece( SAN[pos] ).get_type();

		if( SAN[pos] >= '1' && SAN[pos] <= '8' ) {
			target_square = (SAN[pos-1] - 'a') + (SAN[pos] - '1') * 8;
			pos -= 2;
			break;
		}
	}

	// Ply test_ply( Ply::make_test_ply( target_square, current_type, current.get_type_on_square(target_square), promo_type ) );

	vector<Ply> filtered_plys;

	copy_if( legal_plys.begin(), legal_plys.end(), back_inserter( filtered_plys), [target_square, current_type, promo_type]( Ply the_ply ) { return the_ply.check_san_match(target_square, current_type, promo_type); } );
	// copy_if( legal_plys.begin(), legal_plys.end(), back_inserter( filtered_plys), [test_ply]( Ply the_ply ) { return the_ply.check_san_match( test_ply ); } );

	if( filtered_plys.empty() )
		throw( domain_error( "Cannot find legal ply") );

	if( filtered_plys.size() == 1 ) {
		moves.push_back( filtered_plys[0] );
		return;
	}

	// find departing file and/or rank
	unsigned int file = (unsigned int)-1;
	unsigned int rank = (unsigned int)-1;

	for( ; pos != 0; --pos ) {
		if( SAN[pos] >= '1' && SAN[pos] <= '8' )
			rank = SAN[pos] - '1';

		if( SAN[pos] >= 'a' && SAN[pos] <= 'h' )
			file = SAN[pos] - 'a';
	}

	auto ply_it(
		find_if( filtered_plys.begin(), filtered_plys.end(),
			[file, rank]( Ply ply )
			{
				return( ((file == (unsigned int)-1) || (file == (ply.square_from() % 8) )) && ((rank == (unsigned int)-1) || (rank == (ply.square_from() / 8))) );
			}
		)
	);

	if( ply_it == filtered_plys.end() )
		throw( domain_error( "Cannot find matching ply") );

	moves.push_back( *ply_it );
}

