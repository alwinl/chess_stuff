/*
 * [program] Copyright 2014 Alwin Leerling <alwin@jambo>
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

#ifndef APPTESTER_H
#define APPTESTER_H

#include "../logicsrc/chessappbase.h"

class ChessController;

/** \brief
 */
class TestApplication : public ChessAppBase
{
public:
	TestApplication( ChessEngine* engine_init );
	virtual ~TestApplication() {};

	/* called from main function */;
	virtual int run(  int argc, char *argv[] );

	/* Called from logic */
    virtual void set_piece_positions( std::string FEN_string, STInfo& info );
    virtual void push_statusbar_text( std::string message );
    virtual void message_dialog( std::string message );
    virtual STPieceValues edit_piecevalues( STPieceValues& current );

    virtual void start_arranging();
    virtual void end_arranging();

    virtual std::string open_filename( std::string filename, std::string working_dir );
    virtual std::string save_filename( std::string filename, std::string working_dir );

    virtual void quit();

private:
	ChessEngine* engine;
};

#endif // APPTESTER_H
