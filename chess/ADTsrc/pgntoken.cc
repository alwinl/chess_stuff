/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#include "pgntoken.h"

#include <vector>
#include <iostream>

/*
Tokens:

	<string> ::= """ any printable character """
	<integer> ::= { <digit> }
	<movenoindicator> ::= "."
	<game_terminator> ::= "*"
	<tag_start> ::= "["
	<tag_end> ::= "]"
	<RAV_start> ::= "("
	<RAV_end> ::= ")"
	<reserved_start> ::= "<"
	<reserved_end> ::= ">"
	<NAG> ::= "$" { <digit> }
	<symbol> ::=  <letter> | <digit>  { <letter> | <digit> | <specials> }

	letter ::= [A-Za-z]
	digit ::= [0-9]
	specials ::= "_" | "+" | "#" | "=" | "-"
*/
PGNToken::PGNToken()
{
	state_dispatchers.insert( std::make_pair( COLLECTING_NONE, &PGNToken::state_none ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_STRING, &PGNToken::state_string ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_NAG, &PGNToken::state_nag ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_SYMBOL, &PGNToken::state_symbol ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_INTEGER, &PGNToken::state_integer ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_COMMENT, &PGNToken::state_comment ) );
	state_dispatchers.insert( std::make_pair( COLLECTING_LINECOMMENT, &PGNToken::state_linecomment ) );
}

void PGNToken::reset()
{
	kind = PROCESSING;
	state = COLLECTING_NONE;
	collected = "";
	escaped = false;
}

bool PGNToken::add_character( char ch )
{
	FN_SIG fp = state_dispatchers[state];
	(this->*fp)( ch );

	return kind != PROCESSING;
}

void PGNToken::state_none( char ch )
{
	switch( ch ) {
	// self terminating tokens
	case '[': kind = TAGSTART; break;
	case ']': kind = TAGEND; break;
	case '.': kind = MOVENOINDICATOR; break;
	case '*': kind = GAMETERMINATOR; break;
	case '(': kind = RAVSTART; break;
	case ')': kind = RAVEND; break;
	case '<': kind = RESERVEDSTART; break;
	case '>': kind = RESERVEDEND; break;
	case '\n': kind = LINETERMINATOR; break;

	// non self terminating tokens
	case '"': state = COLLECTING_STRING; break;
	case '$': state = COLLECTING_NAG; break;
	case '{': state = COLLECTING_COMMENT; break;
	case ';': state = COLLECTING_LINECOMMENT; break;
	default:
		if( std::isalpha(ch) ) {
			collected += ch;
			state = COLLECTING_SYMBOL;
			break;
		}
		if ( std::isdigit(ch) ) {
			collected += ch;
			state = COLLECTING_INTEGER;
			break;
		}
		if( ( ch == '\0' ) || std::isspace(ch) ) {
			break;
		}
		kind = INVALID;
		break;
	}
}

void PGNToken::state_string( char ch )
{
	if( escaped ) {
		collected += ch;
		escaped = false;
		return;
	}

	if( ch == '\\' ) {
		escaped = true;
		return;
	}

	if( ( ch == '\0' ) || (ch == '"') ) {
		kind = STRING;
		state = COLLECTING_NONE;
		return;
	}

	collected += ch;
}

void PGNToken::state_nag( char ch )
{
	if( ! std::isdigit(ch) ) {
		kind = NAG;
		state = COLLECTING_NONE;
		return;
	}

	collected += ch;
}

void PGNToken::state_symbol( char ch )
{
	if( ( ch == '\0' ) || std::isspace( ch ) ) {
		kind = SYMBOL;
		state = COLLECTING_NONE;
		return;
	}

	if( std::isalnum(ch) || ch == '_' || ch == '+' || ch == '#' || ch == '=' || ch == '-' ) {
		collected += ch;
		return;
	}

	kind = INVALID;
}

void PGNToken::state_integer( char ch )
{
	if( std::isdigit(ch)) {
		collected += ch;
		return;
	}
	if( ( ch == '\0' ) || std::isspace( ch ) ) {
		kind = INTEGER;
		state = COLLECTING_NONE;
		return;
	}

	 if( ch == '.' ) {
		kind = MOVENOINDICATOR;
		state = COLLECTING_NONE;
		return;
	 }

	if( std::isalpha(ch) ) {
		state = COLLECTING_SYMBOL;
		collected += ch;
		return;
	}

	kind = INVALID;
}

void PGNToken::state_comment( char ch )
{
	if( ( ch == '\0' ) || ( ch == '}' ) ) {
		kind = COMMENT;
		state = COLLECTING_NONE;
		return;
	}

	collected += ch;
}

void PGNToken::state_linecomment( char ch )
{
	if( ( ch == '\0' ) || ( ch == '\n' ) ) {
		kind = COMMENT;
		state = COLLECTING_NONE;
		return;
	}

	collected += ch;
}

std::ostream& operator<<( std::ostream& os, PGNToken token )
{
	os << "Token : " << token.type() << " '" << token.data() << "'\n";
	return os;
}

std::ostream& operator<<( std::ostream& os, PGNToken::eTokenType type )
{
	std::string type_rep = std::vector<std::string> { "PROCESSING", "INVALID", "SECTIONEND", "FILEEND", "STRING", "INTEGER", "MOVENOINDICATOR", "GAMETERMINATOR",
						"TAGSTART", "TAGEND", "RAVSTART", "RAVEND", "RESERVEDSTART", "RESERVEDEND", "NAG", "SYMBOL", "COMMENT", "LINETERMINATOR" }[ type ];

	os << type_rep;
	return os;
}
