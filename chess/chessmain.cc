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


#include "logicsrc/chessengine.h"

#ifdef TESTING
#include "logictest/testapplication.h"
#else
#include "guisrc/chessappgui.h"
#endif

int main( int argc, char *argv[] )
{
#ifdef TESTING
    return TestApplication().run();
#else
    ChessEngine engine;

    ChessAppBase * application = new ChessAppGUI( &engine );

    engine.set_application_pointer( application );

    int result = application->run( argc, argv );

    delete application;

    return result;
#endif // TESTING
}
