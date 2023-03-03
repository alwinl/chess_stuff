#include <iostream>

#include <vector>
#include <map>
#include <array>
#include <algorithm>

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <cctype>

using namespace std;





enum eColor { white, black};
enum eType { none, pawn, knight, bishop, rook, queen, king };

bool sliding_piece[] = { false, false, false, true, true, true, false };
unsigned int direction_count[] = { 0, 0, 8, 4, 4, 8, 8 };

union Piece {
	uint16_t piece;
	struct {
		uint16_t reserved:8;
		uint16_t has_moved:1;
		uint16_t reserved1:3;
		uint16_t color:1;
		uint16_t type:3;
	};
};

bool operator<( const Piece lhs, const Piece rhs ) { return lhs.piece < rhs.piece; }

eColor get_color( Piece piece ) { return eColor( piece.color ); }
eType get_type( Piece piece ) { return eType( piece.type ); }
unsigned int ray_directions( Piece piece ) { return direction_count[piece.type]; };
bool is_sliding( Piece piece ) { return sliding_piece[piece.type]; }

Piece _none   = { .has_moved = false, .color = white, .type = none };
Piece wpawn   = { .has_moved = false, .color = white, .type = pawn };
Piece wknight = { .has_moved = false, .color = white, .type = knight };
Piece wbishop = { .has_moved = false, .color = white, .type = bishop };
Piece wrook   = { .has_moved = false, .color = white, .type = rook };
Piece wqueen  = { .has_moved = false, .color = white, .type = queen };
Piece wking   = { .has_moved = false, .color = white, .type = king };
Piece bpawn   = { .has_moved = false, .color = black, .type = pawn };
Piece bknight = { .has_moved = false, .color = black, .type = knight };
Piece bbishop = { .has_moved = false, .color = black, .type = bishop };
Piece brook   = { .has_moved = false, .color = black, .type = rook };
Piece bqueen  = { .has_moved = false, .color = black, .type = queen };
Piece bking   = { .has_moved = false, .color = black, .type = king };

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



union Move {
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
		uint16_t flags : 1;
	};
};


class Display
{
public:
	Display() { off(); clear_screen(); };
	~Display() { on(); restore_screen(); };

	void print_input_header( bool is_white );
	void print_invalid_move();
	void promo_menu( bool clear);
	void new_game_menu();
	void print_bar();
	uint16_t get_square();
	unsigned int select_promo_type();
	unsigned int select_gametype();

	void print_promotion_move( uint16_t from, uint16_t to, bool is_capture, uint16_t promo_type );
	void print_castling_move( uint16_t from, uint16_t to );
	void print_regular_move( uint16_t type, uint16_t from, uint16_t to, bool is_capture );

	void print_board_header();
	void print_board_footer();
	void print_rank_header( unsigned int rank );
	void print_rank_footer( unsigned int rank );
	void print_square( unsigned int rank, unsigned int file, uint16_t type, bool is_white );

private:
	struct termios t;

	void off()
	{
		tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
		t.c_lflag &= ~ICANON; //Manipulate the flag bits to do what you want it to do
		tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
	}

	void on()
	{
		tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
		t.c_lflag |= ICANON; //Manipulate the flag bits to do what you want it to do
		tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
	}


	void ansi_cgi( std::string cgi_sequence )
		{ cout << "\x1B[" << cgi_sequence; }

	void char_color( unsigned int foreground, unsigned int background )
		{ ansi_cgi( std::to_string( foreground ) + ';' + std::to_string( background ) + "m" ); }

	void set_cursor( unsigned int row, unsigned int column )
		{ ansi_cgi( std::to_string( row ) + ';' + std::to_string( column ) + "H" ); }

	void restore() { char_color( 39, 49 ); }

	void erase_display() { ansi_cgi( "0J" ); }	/* clears from cursor to end of screen */
	void erase_line() {  ansi_cgi( "0K" ); }	/* clears from cursor to end of line */



	void clear_screen()
	{
		set_cursor( 1, 1 );
		erase_display();
	}

	void restore_screen() { set_cursor( 10, 1 ); }

};



vector<Move> game_moves;

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
		if( clear )
			erase_line();
		else
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



void print_board( BoardType& board, Display& display )
{
	display.print_board_header();

    for( int rank=8; rank; --rank ) {

		display.print_rank_header( rank );

		for( int file=0; file<8; ++file ) {

			int index = (rank - 1) * 8 + file;

			display.print_square( rank, file, board[index].type, board[index].color == white );
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

		the_move.to += ( (attacking_piece.color == white) ? -8 : 8 );

		board[the_move.to] = _none;

	} else if( the_move.promotion ) {
		Piece promo_piece = board[the_move.from];

		promo_piece.type = the_move.promo_type;

		board[the_move.to] = promo_piece;
		board[the_move.from] = _none;

	} else if( the_move.castling ) {

		// move the king
		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		board[the_move.to].has_moved = true;

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

		board[the_move.to].has_moved = true;

	} else {
		board[the_move.to] = board[the_move.from];
		board[the_move.from] = _none;

		board[the_move.to].has_moved = true;
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

	static int offset[7][8] = {
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* none */
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* pawn */
		{ -21, -19,-12, -8, 8, 12, 19, 21 }, /* knight */
		{ -11,  -9,  9, 11, 0,  0,  0,  0 }, /* bishop */
		{ -10,  -1,  1, 10, 0,  0,  0,  0 }, /* rook */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }, /* queen */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }  /* king */
	};

	for( uint16_t square = 0; square < 64; ++square) { /* loop over all squares (no piece list) */

		Piece piece = board[square];

		if( piece.color != side )
			continue;

		if( piece.type != pawn ) {

			for( unsigned int ray = 0; ray < ray_directions(piece); ++ray ) {

				uint16_t target_square = square;

				do {
					target_square = mailbox[ mailbox64[target_square] + offset[piece.type][ray] ];	/* next square in this direction */

					if( target_square == (uint16_t)-1 )	/* outside of board */
						break;

					if( board[target_square].type == none )	/* quiet move */
						moves.push_back( {.from = square, .to = target_square } );
					else {
						if( board[target_square].color != side )
							moves.push_back( {.from = square, .to = target_square, .capture = true } );
						break;
					}

				} while( is_sliding(piece) );
			}
		}

		if( piece.type == pawn ) {

			uint16_t target_square = square;

			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[target_square] + (piece.color == white ? 10 : -10) ];	/* next square in this direction */

				if( board[target_square].type != none )	/* cannot_move */
					break;

				if( target_square / 8 == (piece.color == white ? 7: 0) ) {		// promotion ranks
					for( eType type = knight; type < king; type = eType(type + 1) )
						moves.push_back( {.from = square, .to = target_square, .promotion = true, .promo_type = type } );	// generate a promotion moves
					break;
				}

				moves.push_back( {.from = square, .to = target_square, .ep_candidate = (counter == 1) } );	// generate a quiet move

				if( piece.has_moved )
					break;
			}

			// capture moves
			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[square] + ((int[2][2]){{9,11},{-9,-11}})[piece.color][counter] ];

				if( target_square != (uint16_t)-1 )	{				/* on the board... */

					if( ( board[target_square].type != none ) && ( board[target_square].color != side ) ) {	/* something is there, and its their piece */

						if( target_square / 8 == (piece.color == white ? 7: 0) ) {		// promotion ranks
							for( eType type = knight; type < king; type = eType(type + 1) )
								moves.push_back( {.from = square, .to = target_square, .promotion = true, .promo_type = type, .capture = true } );	// generate a promotion moves
						} else
							moves.push_back( {.from = square, .to = target_square, .capture = true } );
					}

					if( !game_moves.empty() && game_moves.back().ep_candidate ) {		// check en-passant

						Move last_move = game_moves.back();
						uint16_t skipped_square = last_move.to + ( (piece.color == white) ? 8 : -8 );	// To calculate the skipped square we subtract 8 for white and add 8 for black

						if( skipped_square == target_square )
							moves.push_back( {.from = square, .to = target_square, .capture = true, .en_passant = true } );
					}
				} /*  target_square != (uint16_t)-1 */

			}

		}

		if( piece.type == king && !piece.has_moved ) {

			// Check king side castle
			if( (board[square + 3].type == rook) && !board[square + 3].has_moved
				&& ( board[square + 1].type == none )
				&& ( board[square + 2].type == none )
			)
				moves.push_back( {.from = square, .to = uint16_t(square + 2), .castling = true } );

			// Check queen side castle
			if( (board[square - 4].type == rook) && !board[square - 4].has_moved
				&& ( board[square - 1].type == none )
				&& ( board[square - 2].type == none )
				&& ( board[square - 3].type == none )
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
		disp.print_regular_move( board[the_move.from].type, the_move.from, the_move.to, the_move.capture );
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

	if( is_human[ current_player ] )
		quit = input_move( board, current_player, generate_moves( board, current_player ) );
	else
		quit = make_move( board, generate_moves( board, current_player ) );

	current_player = eColor( current_player ^ 1 );

    return quit;
}

int main()
{
#if 0
	Display bf;
	eColor current_player = white;

	unsigned int gametype = -1;

	while( gametype == (unsigned int )-1 ) {
		new_game_menu();
		gametype = select_gametype();
	}

	bool quit = ( gametype == 5 );
	bool is_human[2] = { ((gametype == 1) || (gametype == 4)), ((gametype == 2) || (gametype == 4)) };

    while( !quit ) {

		print_board( board );

		if( is_human[ current_player ] )
			quit = input_move( board, current_player, generate_moves( board, current_player ) );
		else
			quit = make_move( board, generate_moves( board, current_player ) );

		current_player = eColor( current_player ^ 1 );
    }
#else
	ChessGame game;

	bool quit = game.setup();

    while( !quit ) {
		quit = game.game_loop();
    }
#endif
    return 0;
}
