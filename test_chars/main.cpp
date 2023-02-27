#include <iostream>

#include <vector>
#include <map>
#include <array>

#include <unistd.h>
#include <stdio.h>

using namespace std;

#include <termios.h>
class TerminalSetup
{
public:
	TerminalSetup() { off(); clear_screen(); };
	~TerminalSetup() { on(); restore_screen(); };

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

	void clear_screen()
	{
		static const std::string CLS = "\x1B[2J";
		static const std::string HOME = "\x1B[0;0H";

		cout << CLS << HOME;
	}

	void restore_screen()
	{
		static const std::string BOTTOMLEFT = "\x1B[10;1H";
		cout << BOTTOMLEFT;
		flush( cout );
	}
};




enum eColor { white, black};
enum eType { none, pawn, knight, bishop, rook, queen, king };

union Piece {
	uint16_t piece;
	struct {
		uint16_t reserved:8;
		uint16_t has_moved:1;
		uint16_t sliding:1;
		uint16_t direction_count:2;
		uint16_t color:1;
		uint16_t type:3;
	};
};

bool operator<( const Piece lhs, const Piece rhs ) { return lhs.piece < rhs.piece; }

eColor get_color( Piece piece ) { return eColor( piece.color ); }
eType get_type( Piece piece ) { return eType( piece.type ); }
unsigned int ray_directions( Piece piece ) { return piece.direction_count << 2; };
bool is_sliding( Piece piece ) { return piece.sliding; }

Piece _none   = { .has_moved = false, .sliding = false, .direction_count = 0, .color = white, .type = none };
Piece wpawn   = { .has_moved = false, .sliding = false, .direction_count = 0, .color = white, .type = pawn };
Piece wknight = { .has_moved = false, .sliding = false, .direction_count = 2, .color = white, .type = knight };
Piece wbishop = { .has_moved = false, .sliding = true,  .direction_count = 1, .color = white, .type = bishop };
Piece wrook   = { .has_moved = false, .sliding = true,  .direction_count = 1, .color = white, .type = rook };
Piece wqueen  = { .has_moved = false, .sliding = true,  .direction_count = 2, .color = white, .type = queen };
Piece wking   = { .has_moved = false, .sliding = false, .direction_count = 2, .color = white, .type = king };
Piece bpawn   = { .has_moved = false, .sliding = false, .direction_count = 0, .color = black, .type = pawn };
Piece bknight = { .has_moved = false, .sliding = false, .direction_count = 2, .color = black, .type = knight };
Piece bbishop = { .has_moved = false, .sliding = true,  .direction_count = 1, .color = black, .type = bishop };
Piece brook   = { .has_moved = false, .sliding = true,  .direction_count = 1, .color = black, .type = rook };
Piece bqueen  = { .has_moved = false, .sliding = true,  .direction_count = 2, .color = black, .type = queen };
Piece bking   = { .has_moved = false, .sliding = false, .direction_count = 2, .color = black, .type = king };

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
	uint16_t move;
	struct {
		uint16_t from : 6;
		uint16_t to : 6;
		uint16_t promotion : 1;
		union {
			uint16_t promo_type : 3;
			struct {
				uint16_t capture : 1;
				uint16_t castling : 1;
				uint16_t flags : 1;
			};
		};
	};
};




void print_board( BoardType& board )
{
	static const std::string BLACKSQUARE = "\x1B[30;107m";
	static const std::string WHITESQUARE = "\x1B[97;40m";
	static const std::string RESTORE = "\x1B[39;49m";
	static const std::string INDENT = "\x1B[20x";
	static const std::string CLS = "\x1B[2J";
	static const std::string HOME = "\x1B[0;0H";

	array<string,14> rep = {
		" ", "♙", "♘", "♗", "♖", "♕", "♔",
		" ", "♟", "♞", "♝", "♜", "♛", "♚",
	};

	cout << CLS << HOME;

    for( int rank=8; rank; --rank ) {
		cout << INDENT;
		cout << rank;
		for( int file=0; file<8; ++file ) {
			int index = (rank - 1) * 8 + file;

			if( (file + rank) % 2 ) {
				cout << BLACKSQUARE << rep[ board[index].color * 7 + board[index].type ];
				//cout << BLACKSQUARE << rep_on_black.at( board[index] );
			} else {
				cout << WHITESQUARE << rep[(1 - board[index].color) * 7 + board[index].type];
				//cout << WHITESQUARE << rep_on_white.at( board[index] );
			}

		}
		cout << RESTORE << endl;
    }
    cout << " abcdefgh" << endl;

}

void print_white_input_header()
{
	static const std::string INPUT = "\x1B[2;12H";
    cout << INPUT << "White" << '?';
    flush( cout );
}

void print_black_input_header()
{
	static const std::string INPUT = "\x1B[2;12H";
    cout << INPUT << "Black" << '?';
    flush( cout );
}

void print_move( Move & the_move )
{
	cout << "\x1B[2;12H";

	if( the_move.promotion ) {

	} else if( the_move.castling ) {

		cout << (( the_move.to > the_move.from ) ? "O - O" : "O - O - O");

	} else {
		cout << string("E NBRQK")[board[the_move.from].type];
		cout << (char)('a' + (the_move.from % 8)) << (char)('1' + (the_move.from / 8) );
		cout << (the_move.capture ? " x " : " - " );
		cout << (char)('a' + (the_move.to % 8)) << (char)('1' + (the_move.to / 8) );
	}


	flush( cout );
	getchar();
}

int mailbox[120] = {
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

int mailbox64[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};

std::vector<Move> generate_moves( BoardType& board, eColor side )
{
	std::vector<Move> moves;

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

		Piece p = board[square];

		if( p.color != side )
			continue;

		if( p.type != pawn ) {

			for( unsigned int ray = 0; ray < ray_directions(p); ++ray ) {
				uint16_t target_square = square;
				do {
					target_square = mailbox[ mailbox64[target_square] + offset[p.type][ray] ];	/* next square in this direction */

					if( target_square == (uint16_t)-1 )	/* outside of board */
						break;

					if( board[target_square].type == none )	/* quiet move */
						moves.push_back( {.from = square, .to = target_square } );
					else {
						if( board[target_square].color != side )
							moves.push_back( {.from = square, .to = target_square, .capture = true } );
						break;
					}

				} while( is_sliding(p) );
			}
		}

		if( p.type == pawn ) {
			// generate pawn moves
			uint16_t target_square = square;

			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[target_square] + ((int[]){ 10, -10 })[p.color] ];	/* next square in this direction */

				if( board[target_square].type != none )	/* cannot_move */
					break;

				if( target_square / 8 == ((int[]){ 7, 0 })[p.color] ) {		// promotion ranks
					for( eType type = knight; type < king; type = eType(type + 1) )
						moves.push_back( {.from = square, .to = target_square, .promotion = true, .promo_type = type } );	// generate a promotion moves
					break;
				}

				moves.push_back( {.from = square, .to = target_square } );	// generate a quiet move

				if( p.has_moved )
					break;
			}

			// capture moves
			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[square] + ((int[2][2]){{9,11},{-9,-11}})[p.color][counter] ];

				if(    ( target_square != (uint16_t)-1 )					/* on the board... */
					&& ( board[target_square].type != none )	/* ...something is there... */
					&& ( board[target_square].color != side )	/* ...and its their piece */
				)
					moves.push_back( {.from = square, .to = target_square, .capture = true } );
			}

			// en passant moves

		}

		if( p.type == king && !p.has_moved ) {
			uint16_t rook_squares[2][2] = { {7,0}, {63, 56} };

			// Check king side castle
			if( !board[rook_squares[p.color][0]].has_moved
				&& ( board[rook_squares[p.color][0] - 1].type == none )
				&& ( board[rook_squares[p.color][0] - 2].type == none )
			)
				moves.push_back( {.from = square, .to = uint16_t(rook_squares[p.color][0] - 1), .castling = true } );

			// Check queen side castle
			if( !board[rook_squares[p.color][1]].has_moved
				&& ( board[rook_squares[p.color][1] + 1].type == none )
				&& ( board[rook_squares[p.color][1] + 2].type == none )
				&& ( board[rook_squares[p.color][1] + 3].type == none )
			)
				moves.push_back( {.from = square, .to = uint16_t(rook_squares[p.color][1] + 2), .castling = true } );
		}
	}

	return moves;
}


void apply_move( BoardType& board, Move& the_move )
{
	print_move( the_move );

	board[the_move.to] = board[the_move.from];
	board[the_move.from] = _none;

	board[the_move.to].has_moved = true;
}

bool input_move( BoardType& board, std::vector<Move> moves )
{
    char file_char;
    char rank_char;
	Move new_move;

    cin >> file_char;
	if( std::string("qQ").find( file_char ) != std::string::npos )
		return true;

    cin >> rank_char;

    new_move.from = (rank_char - '1') * 8 + file_char - 'a';

    cout << '-';
    flush( cout );

    cin >> file_char >> rank_char;
    new_move.to = (rank_char - '1') * 8 + file_char - 'a';

    apply_move( board, new_move );

    return false;
}

#include <experimental/random>

bool make_move( BoardType& board, std::vector<Move> moves )
{
	int size = moves.size();
    int random_number = std::experimental::randint( 0, size - 1 );

    apply_move( board, moves[random_number] );

	return false;
}

int get_gametype()
{
	static const std::string CLS = "\x1B[2J";
	static const std::string HOME = "\x1B[0;0H";
	char answer;

	for( ;; ) {
		cout << CLS << HOME;
		cout << "1. Human vs Computer\n"
			 << "2. Computer vs Human\n"
			 << "3. Computer vs Computer\n"
			 << "4. Human vs Human\n"
			 << "q. Quit\n"
			 << "\n"
			 << "Selection? ";
		flush( cout );

		cin >> answer;

		if( std::string("qQ").find( answer ) != std::string::npos )
			return 0;

		if( std::string("1234").find( answer ) != std::string::npos )
			return answer - '0';
	}
}

int main()
{
	TerminalSetup bf;

	int gametype = get_gametype();

	if( gametype == 0 )
		return 0;

	bool white_is_human = (gametype == 1) || (gametype == 4);
	bool black_is_human = (gametype == 2) || (gametype == 4);
	bool is_white_turn = true;
	bool quit = false;

    while( !quit ) {

		print_board( board );

		if( is_white_turn ) {
			if( white_is_human ) {
				print_white_input_header();
				quit = input_move( board, generate_moves( board, white ) );
			} else {
				sleep( 2 );
				quit = make_move( board, generate_moves( board, white ) );
			}
		} else {

			if( black_is_human ) {
				print_black_input_header();
				quit = input_move( board, generate_moves( board, black ) );
			} else {
				sleep( 2 );
				quit = make_move( board, generate_moves( board, black ) );
			}
		}

		is_white_turn = ! is_white_turn;
    }

    return 0;
}
