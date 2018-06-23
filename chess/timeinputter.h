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

class TimeInputter
{
public:
    TimeInputter() {};
    virtual ~TimeInputter() {};

    std::pair<bool, int> time_per_move()
        { return get_input( "Timed Level", "Seconds per move:" ); };

    std::pair<bool, int> total_game_time()
        { return get_input( "Total Time Level", "Minutes per game:" ); };

protected:
    virtual void set_up( const std::string& title, const std::string& prompt, int& value ) = 0;
    virtual bool manipulate_data( ) = 0;
    virtual int new_time( ) = 0;

private:
    std::pair<bool, int> get_input( const std::string& title, const std::string& prompt )
    {
        bool retval;
        int value = 120;

        set_up( title, prompt, value );
        if( (retval = manipulate_data( )) == true )
            value = new_time( );

        return std::pair<bool,int>(retval, value);
    };
};

#endif // TIMEINPUTTER_H
