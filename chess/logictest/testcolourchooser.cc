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

#include "testcolourchooser.h"

#include "../logicsrc/colourchooser.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestColourChooser );


/*****************************************************************************
 * Helper classes to test the colour chooser logic
 */

class MockColourChooserOk : public ColourChooser
{
public:
    MockColourChooserOk( STColours& test_colours ) : ColourChooser(), new_colours(test_colours) {};

private:
    STColours new_colours;
    STColours the_colours;

    virtual void set_colours( STColours& colours )  { the_colours = colours; };
    virtual bool manipulate_colours( )              { the_colours = new_colours; return true; };
    virtual STColours colours( ) { return the_colours; };
};

class MockColourChooserCancel : public ColourChooser
{
public:
    MockColourChooserCancel( STColours& test_colours ) : ColourChooser(), new_colours(test_colours) {};

private:
    STColours new_colours;
    STColours the_colours;

    virtual void set_colours( STColours& colours )  { the_colours = colours; };
    virtual bool manipulate_colours( )              { return false; };
    virtual STColours colours( ) { return the_colours; };
};



/*****************************************************************************
 * TestColourChooser class implementation
 */

void TestColourChooser::no_change_exit_ok()
{
    STColours input_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };
    STColours test_colours( input_colours );
    STColours output_colours( input_colours );

    MockColourChooserOk test_object( input_colours );

    CPPUNIT_ASSERT( test_object.choose_colours( test_colours ) == true );
    CPPUNIT_ASSERT( test_colours == output_colours );
}

void TestColourChooser::no_change_exit_cancel()
{
    STColours input_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };
    STColours test_colours( input_colours );
    STColours output_colours( input_colours );

    MockColourChooserCancel test_object( input_colours );

    CPPUNIT_ASSERT( test_object.choose_colours( test_colours ) == false );
    CPPUNIT_ASSERT( test_colours == output_colours );
}

void TestColourChooser::change_exit_ok()
{
    STColours input_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };
    STColours test_colours = {
        .bg    = "rgb(192,128,0)",
        .fg    = "rgb(128,192,0)",
        .white = "rgb(128,0,192)",
        .black = "rgb(192,128,0)"
    };
    STColours output_colours( input_colours );

    MockColourChooserOk test_object( input_colours );

    CPPUNIT_ASSERT( test_object.choose_colours( test_colours ) == true );
    CPPUNIT_ASSERT( test_colours == output_colours );
}

void TestColourChooser::change_exit_cancel()
{
    STColours input_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };
    STColours test_colours = {
        .bg    = "rgb(192,128,0)",
        .fg    = "rgb(128,192,0)",
        .white = "rgb(128,0,192)",
        .black = "rgb(192,128,0)"
    };
    STColours output_colours( test_colours );

    MockColourChooserCancel test_object( input_colours );

    CPPUNIT_ASSERT( test_object.choose_colours( test_colours ) == false );
    CPPUNIT_ASSERT( test_colours == output_colours );
}

