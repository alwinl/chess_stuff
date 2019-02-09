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

    std::pair<bool, int> time_per_move( int current_value )
        { return get_input( "Timed Level", "Seconds per move:", current_value ); };

    std::pair<bool, int> total_game_time( int current_value )
        { return get_input( "Total Time Level", "Minutes per game:", current_value ); };

protected:
    virtual void set_up( const std::string& title, const std::string& prompt, int& value ) = 0;
    virtual bool manipulate_data( ) = 0;
    virtual int new_time( ) = 0;

private:
    std::pair<bool, int> get_input( const std::string& title, const std::string& prompt, int current_value )
    {
        bool retval;
        int value = current_value;

        set_up( title, prompt, value );
        if( (retval = manipulate_data( )) == true )
            value = new_time( );

        return std::pair<bool,int>(retval, value);
    };
};

#endif // TIMEINPUTTER_H
