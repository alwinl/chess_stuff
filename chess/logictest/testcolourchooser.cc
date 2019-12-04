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
    MockColourChooserOk( STColours& init_colours, STColours& test_colours ) : ColourChooser(init_colours), new_colours(test_colours) {};

private:
    STColours new_colours;

    virtual void setup( )  { };
    virtual bool manipulate( ) { return true; };
    virtual STColours result( ) { return new_colours; };
};

class MockColourChooserCancel : public ColourChooser
{
public:
    MockColourChooserCancel( STColours& init_colours ) : ColourChooser(init_colours) {};

private:
    virtual void setup( )  { };
    virtual bool manipulate( ) { return false; };
    virtual STColours result( ) { return STColours(); };	/* does not really matter what is returned, in this context it will never be called */
};



/*****************************************************************************
 * TestColourChooser class implementation
 */

void TestColourChooser::no_change_exit_ok()
{
    ColourChooser::STColours colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };

    MockColourChooserOk test_object( colours, colours );

    bool result = test_object.choose_colours( );

    CPPUNIT_ASSERT( result == true );
    CPPUNIT_ASSERT( test_object.is_colour(colours) );
}

void TestColourChooser::no_change_exit_cancel()
{
    ColourChooser::STColours colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };

    MockColourChooserCancel test_object( colours );

    bool result = test_object.choose_colours( );

    CPPUNIT_ASSERT( result == false );
}

void TestColourChooser::change_exit_ok()
{
    ColourChooser::STColours current_colours = {
        .bg    = "rgb(192,128,0)",
        .fg    = "rgb(128,192,0)",
        .white = "rgb(128,0,192)",
        .black = "rgb(192,128,0)"
    };
    ColourChooser::STColours new_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };

    MockColourChooserOk test_object( current_colours, new_colours );

    bool result = test_object.choose_colours( );

    CPPUNIT_ASSERT( result == true );
    CPPUNIT_ASSERT( test_object.is_colour(new_colours) );
}

void TestColourChooser::change_exit_cancel()
{
    ColourChooser::STColours current_colours = {
        .bg    = "rgb(192,128,0)",
        .fg    = "rgb(128,192,0)",
        .white = "rgb(128,0,192)",
        .black = "rgb(192,128,0)"
    };
    ColourChooser::STColours new_colours = {
        .bg    = "rgb(192,0,0)",
        .fg    = "rgb(0,192,0)",
        .white = "rgb(0,0,192)",
        .black = "rgb(0,128,0)"
    };

    MockColourChooserCancel test_object( current_colours );

    bool result = test_object.choose_colours( );

    CPPUNIT_ASSERT( result == false );
}

