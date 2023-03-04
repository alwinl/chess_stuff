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


using namespace std;





#include "piece.h"


Piece _none   = Piece( white, Piece::none );
Piece wpawn   = Piece( white, Piece::pawn );
Piece wknight = Piece( white, Piece::knight );
Piece wbishop = Piece( white, Piece::bishop );
Piece wrook   = Piece( white, Piece::rook );
Piece wqueen  = Piece( white, Piece::queen );
Piece wking   = Piece( white, Piece::king );
Piece bpawn   = Piece( black, Piece::pawn );
Piece bknight = Piece( black, Piece::knight );
Piece bbishop = Piece( black, Piece::bishop );
Piece brook   = Piece( black, Piece::rook );
Piece bqueen  = Piece( black, Piece::queen );
Piece bking   = Piece( black, Piece::king );


typedef std::array<Piece, 64> BoardType;

BoardType board = {
	wrook, wknight, wbishop, wqueen, wking, wbishop, wknight, wrook,
	wpawn, wpawn, wpawn, wpawn, wpawn, wpawn, wpawn, wpawn,
	_none, _none, _none, _none, _none, _none, _none, _none,
	_none, _none, _none, _none, _none, _none, _none, _none,
	_none, _none, _none, _none, _none, _none, _none, _none,
	_none, _none, _none, _none, _none, _none, _none, _none,
	bpawn, bpawn, bpawn, bpawn, bpawn, bpawn, bpawn, bpawn,
	brook, bknight, bbishop, bqueen, bking, bbishop, bknight, brook,
};


class Move
{
public:
	union {
		uint32_t move;
		struct {
			uint16_t from : 6;
			uint16_t to : 6;
			uint16_t promotion : 1;
			uint16_t promo_type : 3;
			uint16_t capture : 1;
			uint16_t castling : 1;
			uint16_t ep_candidate : 1;
			uint16_t en_passant : 1;
			uint16_t flags : 12;
		};
	};

};




vector<Move> game_moves;

void print_board( BoardType& board, Display& display )
{
	display.print_board_header();

    for( int rank=8; rank; --rank ) {

		display.print_rank_header( rank );

		for( int file=0; file<8; ++file ) {

			int index = (rank - 1) * 8 + file;

			display.print_square( rank, file, board[index].get_type(), board[index].is_color( white ) );
		}

		display.print_rank_footer( rank );
    }

    display.print_board_footer();
}

void update_board( BoardType& board, Move the_move )
{
	if( the_move.en_passant ) {
		Piece attacking_piece = board[the_move.from];

		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		the_move.to += ( attacking_piece.is_color( white ) ? -8 : 8 );

		board[the_move.to] = _none;

	} else if( the_move.promotion ) {
		Piece promo_piece = board[the_move.from];

		promo_piece.promote_pawn( Piece::eType(the_move.promo_type) );

		board[the_move.to] = promo_piece;
		board[the_move.from] = _none;

	} else if( the_move.castling ) {

		// move the king
		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		board[the_move.to].moved();

		// adjust move structure to the rook
		if( the_move.to > the_move.from ) {	// king side
			the_move.from += 3;
			the_move.to -= 1;
		} else { 							// queen side
			the_move.from -= 4;
			the_move.to += 1;
		}

		// move the rook
		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		board[the_move.to].moved();

	} else {
		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		board[the_move.to].moved();
	}
}





std::vector<Move> generate_moves( BoardType& board, eColor side )
{
	std::vector<Move> moves;

	static int mailbox[120] = {
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
		 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
		 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
		 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
		 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
		 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
		 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
		 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	static int mailbox64[64] = {
		21, 22, 23, 24, 25, 26, 27, 28,
		31, 32, 33, 34, 35, 36, 37, 38,
		41, 42, 43, 44, 45, 46, 47, 48,
		51, 52, 53, 54, 55, 56, 57, 58,
		61, 62, 63, 64, 65, 66, 67, 68,
		71, 72, 73, 74, 75, 76, 77, 78,
		81, 82, 83, 84, 85, 86, 87, 88,
		91, 92, 93, 94, 95, 96, 97, 98
	};

//	static int offset[7][8] = {
//		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* none */
//		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* pawn */
//		{ -21, -19,-12, -8, 8, 12, 19, 21 }, /* knight */
//		{ -11,  -9,  9, 11, 0,  0,  0,  0 }, /* bishop */
//		{ -10,  -1,  1, 10, 0,  0,  0,  0 }, /* rook */
//		{ -11, -10, -9, -1, 1,  9, 10, 11 }, /* queen */
//		{ -11, -10, -9, -1, 1,  9, 10, 11 }  /* king */
//	};

	for( uint16_t square = 0; square < 64; ++square) { /* loop over all squares (no piece list) */

		Piece piece = board[square];

		if( ! piece.is_color( side ) )
			continue;

		if( ! piece.is_of_type( Piece::pawn ) ) {

			for( unsigned int ray = 0; ray < piece.ray_directions(); ++ray ) {

				uint16_t target_square = square;

				do {
					target_square = mailbox[ mailbox64[target_square] + piece.get_ray_offset( ray ) ];	/* next square in this direction */

					if( target_square == (uint16_t)-1 )	/* outside of board */
						break;

					if( board[target_square].is_of_type( Piece::none ) )	/* quiet move */
						moves.push_back( {.from = square, .to = target_square } );
					else {
						if( ! board[target_square].is_color( side ) )
							moves.push_back( {.from = square, .to = target_square, .capture = true } );
						break;
					}

				} while( piece.is_sliding() );
			}
		}

		if( piece.is_of_type( Piece::pawn ) ) {

			uint16_t target_square = square;

			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[target_square] + (piece.is_color( white ) ? 10 : -10) ];	/* next square in this direction */

				if( ! board[target_square].is_of_type( Piece::none ) )	/* cannot_move */
					break;

				if( target_square / 8 == (piece.is_color( white ) ? 7: 0) ) {		// promotion ranks
					for( Piece::eType type = Piece::knight; type < Piece::Piece::king; type = Piece::eType(type + 1) )
						moves.push_back( {.from = square, .to = target_square, .promotion = true, .promo_type = type } );	// generate a promotion moves
					break;
				}

				moves.push_back( {.from = square, .to = target_square, .ep_candidate = (counter == 1) } );	// generate a quiet move

				if( piece.has_moved() )
					break;
			}

			// capture moves
			for( int counter = 0; counter < 2; ++counter ) {

				unsigned int offset = ((counter == 0) ? 9 : 11);

				if( piece.is_color( black ) )
					offset *= -1;

				target_square = mailbox[ mailbox64[square] + offset ];

				if( target_square == (uint16_t)-1 )				/* off the board... */
					continue;

				if( !board[target_square].is_of_type( Piece::none ) && ! board[target_square].is_color( side ) ) {	/* something is there, and its their piece */

					if( target_square / 8 == (piece.is_color( white ) ? 7: 0) ) {		// promotion ranks
						for( Piece::eType type = Piece::knight; type < Piece::king; type = Piece::eType(type + 1) )
							moves.push_back( {.from = square, .to = target_square, .promotion = true, .promo_type = type, .capture = true } );	// generate a promotion moves
					} else
						moves.push_back( {.from = square, .to = target_square, .capture = true } );
				}

				if( !game_moves.empty() && game_moves.back().ep_candidate ) {		// check en-passant

					Move last_move = game_moves.back();
					uint16_t skipped_square = last_move.to + ( piece.is_color( white ) ? 8 : -8 );	// To calculate the skipped square we subtract 8 for white and add 8 for black

					if( skipped_square == target_square )
						moves.push_back( {.from = square, .to = target_square, .capture = true, .en_passant = true } );
				}
			}

		}

		if( piece.is_of_type( Piece::king ) && !piece.has_moved() ) {

			// Check king side castle
			if(   !board[square + 3].has_moved()
				&& board[square + 3].is_of_type(Piece::rook)
				&& board[square + 1].is_of_type(Piece::none)
				&& board[square + 2].is_of_type(Piece::none)
			)
				moves.push_back( {.from = square, .to = uint16_t(square + 2), .castling = true } );

			// Check queen side castle
			if(   !board[square - 4].has_moved()
				&& board[square - 4].is_of_type(Piece::rook)
				&& board[square - 1].is_of_type(Piece::none)
				&& board[square - 2].is_of_type(Piece::none)
				&& board[square - 3].is_of_type(Piece::none)
			)
				moves.push_back( {.from = square, .to = uint16_t(square - 2), .castling = true } );
		}
	}

	return moves;
}

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

	bool input_move( BoardType& board, eColor player, std::vector<Move> moves );
	bool make_move( BoardType& board, std::vector<Move> moves );
	void apply_move( BoardType& board, Move the_move );
};



bool ChessGame::input_move( BoardType& board, eColor player, std::vector<Move> moves )
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
			apply_move( board, *move_it );
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

		apply_move( board, promotion_moves[ index ] );
		return false;
	}
}

#include <experimental/random>

bool ChessGame::make_move( BoardType& board, std::vector<Move> moves )
{
	int size = moves.size();
    int random_number = std::experimental::randint( 0, size - 1 );

    apply_move( board, moves[random_number] );

	return false;
}

void ChessGame::apply_move( BoardType& board, Move the_move )
{
	if( the_move.promotion ) {
		disp.print_promotion_move( the_move.from, the_move.to, the_move.capture, the_move.promo_type );
	} else if( the_move.castling ) {
		disp.print_castling_move( the_move.from, the_move.to );
	} else {
		disp.print_regular_move( board[the_move.from].get_type(), the_move.from, the_move.to, the_move.capture );
	}

	sleep( 1 );

	game_moves.push_back( the_move );

	update_board( board, the_move );

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

	print_board( board, disp );

	std::vector<Move> moves = generate_moves( board, current_player );

	disp.print_total_possible_moves( moves.size() );

	if( is_human[ current_player ] )
		quit = input_move( board, current_player, moves );
	else
		quit = make_move( board, moves );

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
