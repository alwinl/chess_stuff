// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "externs.h"
#include "wcdefs.h"

CLOCKTYPE   ChessClock;
CLOCKTYPE ChessTime[2];

static void CGetTime( int *hour, int *minute, int *second, int *sec100 )
{
    _AH = 0x2C;
    geninterrupt( 0x21 );
    *hour = _CH;
    *minute = _CL;
    *second = _DH;
    *sec100 = _DL;
}

void InitTime( CLOCKTYPE *clock )
{
    memset( clock, 0, sizeof( CLOCKTYPE ) );
}

void StartTime( CLOCKTYPE *clock )
{
    CGetTime( &clock->hour, &clock->minute, &clock->second, &clock->sec100 );
}

char * DisplayTime( CLOCKTYPE *clock, char * buffer, int max_length )
{
    static bool Colon = true;
    int hour1, min1, sec1, s100;
    double CurTime = clock->totaltime;

    CGetTime( &hour1, &min1, &sec1, &s100 );

    CurTime += ( ( s100 - clock->sec100 ) * 0.01 +
                 ( sec1 - clock->second ) +
                 ( min1 - clock->minute ) * 60.0 +
                 ( hour1 - clock->hour ) * 3600.0 );

    min1 = int( CurTime / 60. );
    snprintf( buffer, max_length, "%2.2d%s%02d", min1, ( Colon ) ? ":" : " ", int( CurTime - min1 * 60.0 ) );

    Colon = !Colon;

    return buffer;
}

void StopTime( CLOCKTYPE *clock )
{
    int hour1, min1, sec1, s100;

    CGetTime( &hour1, &min1, &sec1, &s100 );

    if( hour1 < clock->hour )
        hour1 += 24;

    clock->totalhours += ( hour1 - clock->hour );
    clock->totmin     += ( min1 - clock->minute );
    clock->totalsecs  += ( sec1 - clock->second );
    clock->tots100    += ( s100 - clock->sec100 );

    clock->number++;

    clock->totaltime +=
        ( s100  - clock->sec100 ) * 0.01 +
        ( sec1  - clock->second )        +
        ( min1  - clock->minute ) * 60.0 +
        ( hour1 - clock->hour ) * 3600.0;

    clock->hour   = hour1;
    clock->minute = min1;
    clock->second = sec1;
    clock->sec100 = s100;
}

double getTotalTime(  CLOCKTYPE *clock )
{
    return clock->totaltime;
}

void setTotalTime( CLOCKTYPE *clock, double new_time );

