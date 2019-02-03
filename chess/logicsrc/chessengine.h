/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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

#ifndef CHESSENGINE_H
#define CHESSENGINE_H

#include <string>

class AppModel;
class ChessAppBase;
class STSquare;

/** \brief
 */
class ChessEngine
{
public:
    /** Default constructor */
    ChessEngine();
    ~ChessEngine();

    void set_application_pointer( ChessAppBase* app );

    void start_move( STSquare square );
    void do_move( STSquare square );
    void cancel_move();

	void select_edit_piece( char piece );

	void arranging_start();
	void arranging_clear();
	void arranging_end( bool canceled );

//	void select_edit_mode();
//	void leave_edit_mode();

    void advance();

    void piece_value_changes();

    //int store_game( const std::string& file_name );
    //int load_game( const std::string& file_name );

    void new_game();
    void quit();

    void open_file();
    void save_file();
    void save_as();

protected:

private:
	AppModel * model;
	ChessAppBase * app;
    std::string filename;
};

#endif // CHESSENGINE_H
