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

class AppModel;
class ChessApplication;

#include "pods.h"

class ChessEngine
{
public:
    /** Default constructor */
    ChessEngine();
    ~ChessEngine();

    void set_application_pointer( ChessApplication* app );

    void start_move( ChessApplication* app, STSquare square );
    void do_move( ChessApplication* app, STSquare square );
    void cancel_move( ChessApplication* app );

    void advance( ChessApplication* app );

    void piece_value_changes( ChessApplication * app );

    //int store_game( const std::string& file_name );
    //int load_game( const std::string& file_name );

    void new_game( ChessApplication* app );
    void quit( ChessApplication* app );

    void open_file( ChessApplication* app );
    void save_file( ChessApplication* app );
    void save_as( ChessApplication* app );

protected:

private:
	AppModel * model;
	ChessApplication * app;
    std::string filename;
};

#endif // CHESSENGINE_H
