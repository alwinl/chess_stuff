/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#ifndef TIMEINPUTTER_H
#define TIMEINPUTTER_H

#include <utility>
#include <string>

/** \brief
 */
class TimeInputter
{
public:
    TimeInputter() {};
    virtual ~TimeInputter() {};

    bool time_per_move( int current_value )
        { return get_input( "Timed Level", "Seconds per move:", current_value ); };

    bool total_game_time( int current_value )
        { return get_input( "Total Time Level", "Minutes per game:", current_value ); };

	int get_time() { return value; };

protected:
    virtual void setup( const std::string& title, const std::string& prompt, int& value ) = 0;
    virtual bool manipulate( ) = 0;
    virtual int result( ) = 0;

    int value;

private:
    bool get_input( const std::string& title, const std::string& prompt, int current_value )
    {
        bool retval;

        value = current_value;

        setup( title, prompt, value );
        if( (retval = manipulate( )) == true )
            value = result( );

        return retval;
    };
};

#endif // TIMEINPUTTER_H
