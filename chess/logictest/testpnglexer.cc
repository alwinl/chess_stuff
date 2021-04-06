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

#include "testpnglexer.h"

#include <sstream>

#include "../ADTsrc/pnglexer.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGLexer );

TestPNGLexer::TestPNGLexer()
{
}

TestPNGLexer::~TestPNGLexer()
{
}


void TestPNGLexer::create_test()
{

    std::string input_data( "[Event \"F/S Return Match\"]\n"
                            "[Site \"Belgrade, Serbia JUG\"]\n"
                            "[Date \"1992.11.04\"]\n"
                            "[Round \"29\"]\n"
                            "[White \"Fischer, Robert J.\"]\n"
                            "[Black \"Spassky, Boris V.\"]\n"
                            "[Result \"1/2-1/2\"]\n"
                            "\n" );

    std::stringstream is( input_data );
    PNGLexer lexer( is );

    std::map<std::string, std::string> tagpairs;

    tagpairs = lexer.parse_tag_pair_section(  );

    CPPUNIT_ASSERT( tagpairs.size() == 7 );
}

