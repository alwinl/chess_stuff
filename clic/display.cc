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

#include "display.h"

#include <iostream>
#include <array>
#include <stdio.h>
#include <unistd.h>

using namespace std;

void Display::print_board_header()
{
	set_cursor( 1, 1 );
	erase_display();

    cout << " abcdefgh";
}

void Display::print_board_footer()
{
	set_cursor( 10, 1 );
    cout << " abcdefgh";

    flush( cout );
}

void Display::print_rank_header( unsigned int rank )
{
	set_cursor( 10 - rank, 1 );
	cout << rank;
}

void Display::print_rank_footer( unsigned int rank )
{
	restore();
	set_cursor( 10 - rank, 10 );
	cout << rank;
}

void Display::print_square( unsigned int rank, unsigned int file, uint16_t type, bool is_white )
{
	static array<string,14> rep = {
		" ", "♙", "♘", "♗", "♖", "♕", "♔",
		" ", "♟", "♞", "♝", "♜", "♛", "♚",
	};

	bool odd = (file + rank) % 2;

	set_cursor( 10 - rank, file + 2 );

	odd ? char_color( 30, 107 ) : char_color( 97, 40 );

	cout << rep[ ( odd == is_white ) ? type : type + 7 ];
}

void Display::print_input_header( bool is_white )
{
	set_cursor( 2, 12 );
	erase_line();

    cout << (is_white ? "White" :"Black") << '?';
    flush( cout );
}

void Display::print_promotion_move( uint16_t from, uint16_t to, bool is_capture, uint16_t promo_type )
{
	set_cursor( 3, 12 );
	erase_line();

	cout << (char)('a' + (from % 8)) << (char)('1' + (from / 8) );
	cout << (is_capture ? " x " : " - " );
	cout << (char)('a' + (to % 8)) << (char)('1' + (to / 8) );
	cout << string("E NBRQK")[promo_type];

	flush( cout );
}

void Display::print_castling_move( uint16_t from, uint16_t to )
{
	set_cursor( 3, 12 );
	erase_line();

	cout << (( to > from ) ? "O - O" : "O - O - O");

	flush( cout );
}

void Display::print_regular_move( uint16_t type, uint16_t from, uint16_t to, bool is_capture )
{
	set_cursor( 3, 12 );
	erase_line();

	cout << string("E NBRQK")[type];
	cout << (char)('a' + (from % 8)) << (char)('1' + (from / 8) );
	cout << (is_capture ? " x " : " - " );
	cout << (char)('a' + (to % 8)) << (char)('1' + (to / 8) );

	flush( cout );
}

void Display::print_invalid_move()
{
	set_cursor( 3, 12 );
	erase_line();

	cout << "Invalid move";
	flush( cout );
}

void Display::promo_menu( bool clear)
{
	std::string output[] = { "1. N", "2. B", "3. R", "4. Q", "? " };

	for( int row = 0; row < 5; ++row ) {
		set_cursor( row + 2, 12 );
		erase_line();
		if( !clear )
			cout << output[row];
	}

	flush( cout );
}

void Display::new_game_menu()
{
	set_cursor( 1, 1 );
	erase_display();

	cout << "1. Human vs Computer\n"
		 << "2. Computer vs Human\n"
		 << "3. Computer vs Computer\n"
		 << "4. Human vs Human\n"
		 << "5. Quit\n"
		 << "\n"
		 << "Selection? ";
	flush( cout );
}

void Display::print_bar()
{
	cout << '-';
	flush( cout );
}

void Display::print_total_possible_moves( unsigned int no_of_moves )
{
	set_cursor( 5, 12 );
	erase_line();

	cout << "Generated moves " << no_of_moves;

	flush( cout );
}


uint16_t Display::get_square()
{
    char file_char;
    char rank_char;

	cin >> file_char;
	if( (file_char == 'Q') || (file_char == 'q') )
		return uint16_t(-1);

	if( (file_char >= 'A') && (file_char <= 'H') )
		file_char += ('a' - 'A');

	if( (file_char < 'a') || (file_char > 'h') )
		return uint16_t(64);

	cin >> rank_char;
	if( (rank_char == 'Q') || (rank_char == 'q') )
		return uint16_t(-1);

	if( (rank_char < '1') || (rank_char > '8') )
		return uint16_t(64);

	return uint16_t((rank_char - '1') * 8 + file_char - 'a');
}

unsigned int Display::select_promo_type()
{
	char answer;

	set_cursor( 7, 12 );
	cin >> answer;

	return ( (answer >= '1') && (answer <= '4') ) ?  answer -'1' : (unsigned int)-1;
}

unsigned int Display::select_gametype()
{
	char answer;

	set_cursor( 7, 12 );
	cin >> answer;

	return ( (answer >= '1') && (answer <= '5') ) ?  answer -'0' : (unsigned int)-1;
}


void Display::off()
{
	tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
	t.c_lflag &= ~ICANON; //Manipulate the flag bits to do what you want it to do
	tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
}

void Display::on()
{
	tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
	t.c_lflag |= ICANON; //Manipulate the flag bits to do what you want it to do
	tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
}

Display::Display()
{
	off();
	clear_screen();
}

Display::~Display()
{
	on();
	restore_screen();
}

void Display::ansi_cgi( std::string cgi_sequence )
	{ cout << "\x1B[" << cgi_sequence; }

void Display::char_color( unsigned int foreground, unsigned int background )
	{ ansi_cgi( std::to_string( foreground ) + ';' + std::to_string( background ) + "m" ); }

void Display::set_cursor( unsigned int row, unsigned int column )
	{ ansi_cgi( std::to_string( row ) + ';' + std::to_string( column ) + "H" ); }

void Display::restore() { char_color( 39, 49 ); }

void Display::erase_display() { ansi_cgi( "0J" ); }	/* clears from cursor to end of screen */
void Display::erase_line() {  ansi_cgi( "0K" ); }	/* clears from cursor to end of line */



void Display::clear_screen()
{
	set_cursor( 1, 1 );
	erase_display();
}

void Display::restore_screen() { set_cursor( 10, 1 ); }
