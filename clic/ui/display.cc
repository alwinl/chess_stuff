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

#include <array>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace std;

Display::Display()
{
	set_cannonical( false );
	set_cursor( cout, 1, 1 ); // initialise the screen
	erase_display( cout );
}

Display::~Display()
{
	set_cursor( cout, 11, 1 ); // restore the screen
	set_cannonical( true );
}

void Display::print_board_header()
{
	set_cursor( cout, 1, 1 );
	erase_display( cout );

	cout << " abcdefgh";
}

void Display::print_board_footer()
{
	set_cursor( cout, 10, 1 );
	cout << " abcdefgh";

	flush( cout );
}

void Display::print_rank_header( unsigned int rank )
{
	set_cursor( cout, 10 - rank, 1 );
	cout << rank;
}

void Display::print_rank_footer( unsigned int rank )
{
	restore( cout );
	set_cursor( cout, 10 - rank, 10 );
	cout << rank;
}

void Display::print_square( unsigned int rank, unsigned int file, uint16_t type, bool is_white )
{
	static array<string, 14> rep = {
		// clang-format off
		" ", "♙", "♘", "♗", "♖", "♕", "♔",
		" ", "♟", "♞", "♝", "♜", "♛", "♚",
		// clang-format on
	};

	bool odd = ( file + rank ) % 2;

	set_cursor( cout, 10 - rank, file + 2 );

	odd ? char_color( cout, 97, 40 ) : char_color( cout, 30, 107 );

	cout << rep[( odd == is_white ) ? type + 7 : type];
}

void Display::print_input_header( bool is_white )
{
	set_cursor( cout, 2, 12 );
	erase_line( cout );

	cout << ( is_white ? "White" : "Black" ) << '?';
	flush( cout );
}

void Display::print_move( std::string the_move )
{
	set_cursor( cout, 3, 12 );
	erase_line( cout );

	cout << the_move;

	flush( cout );
}

void Display::print_promotion_move( uint16_t from, uint16_t to, bool is_capture, uint16_t promo_type )
{
	set_cursor( cout, 3, 12 );
	erase_line( cout );

	cout << (char)( 'a' + ( from % 8 ) ) << (char)( '1' + ( from / 8 ) );
	cout << ( is_capture ? " x " : " - " );
	cout << (char)( 'a' + ( to % 8 ) ) << (char)( '1' + ( to / 8 ) );
	cout << string( "E NBRQK" )[promo_type];

	flush( cout );
}

void Display::print_castling_move( uint16_t from, uint16_t to )
{
	set_cursor( cout, 3, 12 );
	erase_line( cout );

	cout << ( ( to > from ) ? "O - O" : "O - O - O" );

	flush( cout );
}

void Display::print_regular_move( uint16_t type, uint16_t from, uint16_t to, bool is_capture )
{
	set_cursor( cout, 3, 12 );
	erase_line( cout );

	cout << string( "E NBRQK" )[type];
	cout << (char)( 'a' + ( from % 8 ) ) << (char)( '1' + ( from / 8 ) );
	cout << ( is_capture ? " x " : " - " );
	cout << (char)( 'a' + ( to % 8 ) ) << (char)( '1' + ( to / 8 ) );

	flush( cout );
}

void Display::print_invalid_move()
{
	set_cursor( cout, 3, 12 );
	erase_line( cout );

	cout << "Invalid move";
	flush( cout );
}

void Display::promo_menu( bool clear )
{
	std::string output[] = { "1. N", "2. B", "3. R", "4. Q", "? " };

	for( int row = 0; row < 5; ++row ) {
		set_cursor( cout, row + 2, 12 );
		erase_line( cout );
		if( !clear )
			cout << output[row];
	}

	set_cursor( cout, 7, 12 );

	flush( cout );
}

void Display::new_game_menu()
{
	set_cursor( cout, 1, 1 );
	erase_display( cout );

	cout << "1. Human vs Computer\n"
		 << "2. Computer vs Human\n"
		 << "3. Computer vs Computer\n"
		 << "4. Human vs Human\n"
		 << "5. Quit\n"
		 << "\n"
		 << "Selection? ";

	set_cursor( cout, 7, 12 );

	flush( cout );
}

void Display::print_bar()
{
	cout << '-';
	flush( cout );
}

void Display::print_total_possible_moves( unsigned int no_of_moves )
{
	set_cursor( cout, 5, 12 );
	erase_line( cout );

	cout << "Generated moves " << no_of_moves;

	flush( cout );
}

uint16_t Display::select_square()
{
	char file_char;
	char rank_char;

	cin >> file_char;
	if( ( file_char == 'Q' ) || ( file_char == 'q' ) )
		return uint16_t( -1 );

	if( ( file_char >= 'A' ) && ( file_char <= 'H' ) )
		file_char += ( 'a' - 'A' );

	if( ( file_char < 'a' ) || ( file_char > 'h' ) )
		return uint16_t( 64 );

	cin >> rank_char;
	if( ( rank_char == 'Q' ) || ( rank_char == 'q' ) )
		return uint16_t( -1 );

	if( ( rank_char < '1' ) || ( rank_char > '8' ) )
		return uint16_t( 64 );

	return uint16_t( ( rank_char - '1' ) * 8 + file_char - 'a' );
}

unsigned int Display::select_promo_type()
{
	char answer;

	cin >> answer;

	return ( ( answer >= '1' ) && ( answer <= '4' ) ) ? answer - '1' : (unsigned int)-1;
}

unsigned int Display::select_gametype()
{
	char answer;

	cin >> answer;

	return ( ( answer >= '1' ) && ( answer <= '5' ) ) ? answer - '0' : (unsigned int)-1;
}

ostream &Display::ansi_cgi( ostream &os, std::string cgi_sequence )
{
	os << "\x1B[" << cgi_sequence;
	return os;
}

ostream &Display::erase_display( ostream &os )
{
	return ansi_cgi( os, "0J" ); // clears from cursor to end of screen
}

ostream &Display::erase_line( ostream &os )
{
	return ansi_cgi( os, "0K" ); // clears from cursor to end of line
}

ostream &Display::char_color( ostream &os, unsigned int foreground, unsigned int background )
{
	return ansi_cgi( os, std::to_string( foreground ) + ';' + std::to_string( background ) + "m" );
}

ostream &Display::set_cursor( ostream &os, unsigned int row, unsigned int column )
{
	return ansi_cgi( os, std::to_string( row ) + ';' + std::to_string( column ) + "H" );
}

ostream &Display::restore( ostream &os )
{
	return char_color( os, 39, 49 );
}

#ifdef __linux__
#include <termios.h>
#endif

/**
 * @brief Turn input timeout on or off
 *
 * Normal terminal input requires to be terminated with a newline.
 * Setting the ICANNON flag turns input timeout on (amongst other things), thus
 * we do not need to press enter to finalise our input.
 *
 * @param on Turn timeout on (or off)
 */
void Display::set_cannonical( bool on )
{
#ifdef __linux__
	termios t;

	tcgetattr( STDIN_FILENO, &t ); // get the current terminal I/O structure

	if( on )
		t.c_lflag |= ICANON; // Manipulate the flag bits to do what you want it to do
	else
		t.c_lflag &= ~ICANON; // Manipulate the flag bits to do what you want it to do

	tcsetattr( STDIN_FILENO, TCSANOW, &t ); // Apply the new settings
#endif
}