/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <array>
#include <vector>
#include <map>

#include "piece.h"
#include "ply.h"

class GameState
{
public:
	GameState( std::string FEN = "" );
	std::string FEN() const;

	GameState set_piece( uint16_t square, char code );
	GameState set_active_color( eColor color );
	GameState set_castle_rights( std::string castle_rights );
	GameState set_ep_square( uint16_t square );
	GameState set_halvemoves( uint16_t number );
	GameState set_fullmoves( uint16_t number );
	bool is_valid();

	std::vector<Ply> generate_legal_plys() const;

	int evaluate() const;
	GameState make( Ply a_ply ) const;

	std::array<Piece,64> get_pieces() { return position; }
	eColor get_current_colour() const { return current_player; };

private:
	std::array<Piece,64> position;
    eColor current_player;
    std::map<eColor,bool> can_castle_kingside;
    std::map<eColor,bool> can_castle_queenside;
    uint16_t en_passant_target = (uint16_t)-1;
    int halfmove_clock = 0;             // Number of half moves since last capture or pawn advance
    int fullmove_number = 1;             // The number of the move, start at one increment after black move

	void process_placement( std::string PiecePlacement );
	void process_active_color( std::string active_color );
	void process_castling( std::string castle_rights );
	void process_ep( std::string ep_input );
	void process_halfmoves( std::string halvemoves );
	void process_fullmoves( std::string fullmoves );


	std::string piece_placement() const;
	std::string active_color() const;
	std::string castle_rights() const;
	std::string ep_square() const;
	std::string halvemoves() const;
	std::string fullmoves() const;

	std::vector<Ply> generate_plys() const;
};

#endif // GAMESTATE_H
