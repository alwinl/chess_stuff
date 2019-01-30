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

#ifndef TIMELIB_H_INCLUDED
#define TIMELIB_H_INCLUDED

struct CLOCKTYPE {
    int hour;
    int minute;
    int second;
    int sec100;
    int totalhours;
    int totmin;
    int totalsecs;
    int tots100;
    int number;
    double totaltime;
};

extern CLOCKTYPE     ChessTime[];
extern CLOCKTYPE     ChessClock;

void InitTime( CLOCKTYPE *clock );
void StartTime( CLOCKTYPE *clock );
char * DisplayTime( CLOCKTYPE *clock, char * buffer, int max_length );
void StopTime( CLOCKTYPE *clock );

double getTotalTime(  CLOCKTYPE *clock );
void setTotalTime( CLOCKTYPE *clock, double new_time );

#endif // TIMELIB_H_INCLUDED
