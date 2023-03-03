/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <string>
#include <termios.h>

class Display
{
public:
	Display();
	~Display();

	uint16_t get_square();
	unsigned int select_promo_type();
	unsigned int select_gametype();

	void print_board_header();
	void print_board_footer();
	void print_rank_header( unsigned int rank );
	void print_rank_footer( unsigned int rank );
	void print_square( unsigned int rank, unsigned int file, uint16_t type, bool is_white );

	void print_promotion_move( uint16_t from, uint16_t to, bool is_capture, uint16_t promo_type );
	void print_castling_move( uint16_t from, uint16_t to );
	void print_regular_move( uint16_t type, uint16_t from, uint16_t to, bool is_capture );
	void print_invalid_move();

	void print_input_header( bool is_white );
	void promo_menu( bool clear);
	void new_game_menu();
	void print_bar();

private:
	struct termios t;

	void off();
	void on();

	void ansi_cgi( std::string cgi_sequence );
	void char_color( unsigned int foreground, unsigned int background );
	void set_cursor( unsigned int row, unsigned int column );
	void restore();

	void erase_display();	/* clears from cursor to end of screen */
	void erase_line();	/* clears from cursor to end of line */
	void clear_screen();
	void restore_screen();
};

#endif // DISPLAY_H
