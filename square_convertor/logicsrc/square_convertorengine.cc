/*
 * square_convertor Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#include "square_convertorengine.h"

std::string square_convertorEngine::SANtoUInt( std::string SAN)
{
	if( SAN.size() != 2 )
		return std::string();

	if( SAN[1] < '1' || SAN[1] > '8' )
		return std::string();

	if( SAN[0] >= 'a' && SAN[0] <= 'h') {
		unsigned int square = SAN[0] - 'a' + (SAN[1] - '1') * 8;
		return std::to_string( square );
	}

	if( SAN[0] >= 'A' && SAN[0] <= 'H') {
		unsigned int square = SAN[0] - 'A' + (SAN[1] - '1') * 8;
		return std::to_string( square );
	}

	return std::string();
}

std::string square_convertorEngine::UInttoSAN( std::string input )
{
	if( input.empty() )
		return std::string();

	unsigned int square = std::atoi( input.c_str() );

	if( square > 63 )
		return std::string();

	std::string SAN;

	SAN.push_back( (square % 8) + 'a' );
	SAN.push_back( (square / 8) + '1' );

	return SAN;
}
