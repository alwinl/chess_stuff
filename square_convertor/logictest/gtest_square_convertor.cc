/*
 * gtest_square_convertor.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
 */

#include <gtest/gtest.h>

#include "square_convertor_utils.h"

#include <string>

TEST( SquareConvertor, convert_from_SAN )
{
    for( char file = 'a'; file <= 'h'; ++file ) {
        for( char rank = '1'; rank <= '8'; ++rank ) {

            std::string input;
            input += file;
            input += rank;

            std::string result = SANtoUInt( input );

            unsigned int expected = file - 'a' + (rank - '1') * 8;

            EXPECT_EQ( result, std::to_string( expected ));

        }
    }
}

TEST( SquareConvertor, convert_to_SAN )
{
    for( unsigned int square = 0; square < 64; ++square ) {

        std::string out = UInttoSAN( std::to_string(square) );

        std::string result = SANtoUInt( out );

        unsigned int square_out = std::atoi( result.c_str() );

        EXPECT_EQ( square, square_out );
    }
}