#include <iostream>

#include <vector>
#include <map>
#include <array>
#include <algorithm>

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <cctype>


#include "display.h"
#include "piece.h"
#include "board.h"
#include "move.h"

using namespace std;





class ChessGame
{
public:
	ChessGame() {};
	~ChessGame() {};

	bool setup();
	bool game_loop();

private:
	Display disp;
	eColor current_player = white;
	unsigned int gametype = -1;
	bool is_human[2];
	Board board;
	vector<Move> game_moves;

	bool input_move( eColor player, std::vector<Move> moves );
	bool make_move( std::vector<Move> moves );
	void apply_move( Move the_move );
};



bool ChessGame::input_move( eColor player, std::vector<Move> moves )
{
	Move new_move;
	unsigned int square;

	for(;;) {
		disp.print_input_header( player == white );

		if( (square = disp.get_square()) == uint16_t(-1) )
			return true;

		if( square == 64 )
			continue;

		new_move.from = (uint16_t)square;
		disp.print_bar();

		if( (square = disp.get_square()) == uint16_t(-1) )
			return true;

		if( square == 64 )
			continue;

		new_move.to = (uint16_t)square;

		auto square_match = [new_move]( Move the_move ) { return (new_move.from == the_move.from) && (new_move.to == the_move.to); };

		auto move_it = std::find_if( moves.begin(), moves.end(), square_match );
		if( move_it == moves.end() ) {
			disp.print_invalid_move();
			continue;
		}

		// If it is not a promotion, we have all relevant information. Process the move
		if( ! (*move_it).promotion ) {
			apply_move( *move_it );
			return false;
		}

		// we need to find all possible promotions and ask the player to select one
		vector<Move> promotion_moves;
		unsigned int index;

		while( move_it != moves.end() ) {
			promotion_moves.push_back( *(move_it++) ); // push first, then advance iterator
			move_it = std::find_if( move_it, moves.end(), square_match );
		}

		for(;;) {
			disp.promo_menu( false );

			if( (index = disp.select_promo_type()) != (unsigned int) -1 ) {
				disp.promo_menu( true );
				break;
			}
		}

		apply_move( promotion_moves[ index ] );
		return false;
	}
}

#include <experimental/random>

bool ChessGame::make_move( std::vector<Move> moves )
{
	int size = moves.size();
    int random_number = std::experimental::randint( 0, size - 1 );

    apply_move( moves[random_number] );

	return false;
}

void ChessGame::apply_move( Move the_move )
{
	if( the_move.promotion ) {
		disp.print_promotion_move( the_move.from, the_move.to, the_move.capture, the_move.promo_type );
	} else if( the_move.castling ) {
		disp.print_castling_move( the_move.from, the_move.to );
	} else {
		disp.print_regular_move( board.get_type_from_square( the_move.from ), the_move.from, the_move.to, the_move.capture );
	}

	sleep( 1 );

	game_moves.push_back( the_move );

	board.update_board( the_move );
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

	if( !game_moves.empty() && game_moves.back().ep_candidate )
		ep_square = game_moves.back().to + (( current_player == white ) ? 8 : -8);

	std::vector<Move> moves = board.generate_moves( current_player, ep_square );

	disp.print_total_possible_moves( moves.size() );

	if( is_human[ current_player ] )
		quit = input_move( current_player, moves );
	else
		quit = make_move( moves );

	current_player = eColor( current_player ^ 1 );

    return quit;
}

int main()
{
#if 0
	cout << "uint16_t = " << sizeof( uint16_t ) << endl;
	cout << "Piece = " << sizeof( Piece ) << endl;
	cout << "Move = " << sizeof( Move ) << endl;
#else
	ChessGame game;

	bool quit = game.setup();

    while( !quit ) {
		quit = game.game_loop();
    }
#endif
    return 0;
}
