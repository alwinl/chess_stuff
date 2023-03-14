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

#include "chessgame.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include <experimental/random>

int ChessGame::run( int argc, char *argv[] )
{
	bool quit = setup();

    while( !quit )
		quit = game_loop();

    return 0;
}

bool ChessGame::setup()
{
	while( gametype == (unsigned int )-1 ) {
		disp.new_game_menu();
		gametype = disp.select_gametype();
	}

	bool quit = ( gametype == 5 );
	is_human[0] = ((gametype == 1) || (gametype == 4));
	is_human[1] = ((gametype == 2) || (gametype == 4));

	return quit;
}

bool ChessGame::game_loop()
{
	bool quit;

	board.print_board( disp );

	uint16_t ep_square = (uint16_t)-1;

	if( !game_moves.empty() && game_moves.back().is_ep_candidate() )
		ep_square = game_moves.back().get_ep_square( current_player == white );

	std::vector<Ply> moves = board.generate_legal_plys( current_player, ep_square );	// grabs all legal moves
	if( moves.empty() ) {
		// checkmate
		return true;
	}

	disp.print_total_possible_moves( moves.size() );

	if( is_human[ current_player ] )
		quit = input_move( current_player, moves );
	else
		quit = make_move( moves );

	current_player = eColor( current_player ^ 1 );

    return quit;
}



bool ChessGame::input_move( eColor player, std::vector<Ply> moves )
{
	unsigned int square_from;
	unsigned int square_to;

	for(;;) {
		disp.print_input_header( player == white );

		if( (square_from = disp.get_square()) == uint16_t(-1) )
			return true;

		if( square_from == 64 )
			continue;

		disp.print_bar();

		if( (square_to = disp.get_square()) == uint16_t(-1) )
			return true;

		if( square_to == 64 )
			continue;

		Ply new_move = Ply( square_from, square_to );

		auto move_it = std::find_if( moves.begin(), moves.end(), [new_move]( Ply the_move ) { return the_move.check_square_match( new_move ); } );
		if( move_it == moves.end() ) {
			disp.print_invalid_move();
			continue;
		}

		// If it is not a promotion, we have all relevant information. Process the move
		if( (*move_it).check_promo_match( Piece::none ) ) {
			apply_move( *move_it );
			return false;
		}

		// we need to find all possible promotions and ask the player to select one
		unsigned int index;

		for(;;) {
			disp.promo_menu( false );

			if( (index = disp.select_promo_type()) != (unsigned int) -1 ) {
				disp.promo_menu( true );
				break;
			}
		}

		Piece::eType promo_type = ((Piece::eType[]){ Piece::knight, Piece::bishop, Piece::rook, Piece::queen})[index];

		new_move = Ply( square_from, square_to, Piece::none, Piece::none, promo_type );

		move_it = std::find_if( moves.begin(), moves.end(), [new_move]( Ply the_move ) { return the_move.check_promo_match( new_move ); } );

		apply_move( *move_it );

		return false;
	}
}

bool ChessGame::make_move( std::vector<Ply> moves )
{
	int size = moves.size();
    int random_number = std::experimental::randint( 0, size - 1 );

    apply_move( moves[random_number] );

	return false;
}

void ChessGame::apply_move( Ply the_move )
{
	disp.print_move( the_move.print_LAN() );

	std::this_thread::sleep_for(std::chrono::seconds(1));

	game_moves.push_back( the_move );

	board.update_board( the_move );
}



