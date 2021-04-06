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

#ifndef PNGTOKEN_H
#define PNGTOKEN_H

#include <map>
#include <string>

class PNGToken
{
public:
	enum eTokenType { PROCESSING, INVALID, SECTIONEND, FILEEND, STRING, INTEGER, MOVENOINDICATOR, GAMETERMINATOR,
						TAGSTART, TAGEND, RAVSTART, RAVEND, RESERVEDSTART, RESERVEDEND, NAG, SYMBOL, COMMENT, LINETERMINATOR };

public:
	PNGToken();

	void reset();

	static PNGToken EOFToken();
	static PNGToken EOSToken();

	bool add_character( char ch );

	eTokenType type() const { return kind; }
	std::string data() const { return collected; };

private:
	enum eState { COLLECTING_NONE, COLLECTING_STRING, COLLECTING_NAG, COLLECTING_SYMBOL, COLLECTING_INTEGER, COLLECTING_COMMENT, COLLECTING_LINECOMMENT };

	eTokenType kind = PROCESSING;
	eState state = COLLECTING_NONE;
	std::string collected = "";
	bool escaped = false;

	typedef void (PNGToken::*FN_SIG)(char);
	std::map<eState, FN_SIG> state_dispatchers;

	void state_none( char ch );
	void state_string( char ch );
	void state_nag( char ch );
	void state_symbol( char ch );
	void state_integer( char ch );
	void state_comment( char ch );
	void state_linecomment( char ch );
};

#endif // PNGTOKEN_H
