/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#include "testfilenamechooser.h"

#include <../logicsrc/filenamechooser.h>

CPPUNIT_TEST_SUITE_REGISTRATION( TestFilenameChooser );

class MockFilenameChooser : public FilenameChooser
{
public:
	MockFilenameChooser( std::string load_name, std::string save_name ) {
		this->load_name = load_name;
		this->save_name = save_name;
	};
	virtual ~MockFilenameChooser() {};

private:
	virtual std::string get_load_name() { return load_name; };
	virtual std::string get_save_name() { return save_name; };

	std::string load_name;
	std::string save_name;

};
/*
 * There are a number of scenarios where a filename is needed:
 * 1. When the client wants to create a new file
 *		if we have a filename defined, check if dirty and needs saving
 *		after previous step, ask implementation for the name of an existing data file
 * 2. When the client asks to load a file:
 *      If we have a filename defined, check if dirty and needs to be saved
 *      If no filename ask implementation for a filename, set filename to that name
 * 3. When we do a save and we already have a filename, we can supply the filename direct
 * 4. If we do a save without a filename set or we do a save as, ask implementation for a filename
 *
 * Currently the whole dirty check and saving bit is not implemented
 */

/**
 * When an object is created the filename should be undefined
 */
void TestFilenameChooser::test_new_file()
{
	MockFilenameChooser test_object( "Loader", "Saver" );

	CPPUNIT_ASSERT_EQUAL( true, test_object.new_file() );
}

void TestFilenameChooser::test_load_file()
{
	MockFilenameChooser test_object( "Loader", "Saver" );

	CPPUNIT_ASSERT_EQUAL( std::string("Loader"), test_object.load_file() );
}

void TestFilenameChooser::test_load_file_empty()
{
	MockFilenameChooser test_object( "", "" );

	CPPUNIT_ASSERT_EQUAL( std::string(""), test_object.load_file() );
}

void TestFilenameChooser::test_save_file()
{
	MockFilenameChooser test_object( "Loader", "Saver" );

	CPPUNIT_ASSERT_EQUAL( std::string("Saver.chess"), test_object.save_file() );
}

void TestFilenameChooser::test_save_file_as()
{
	MockFilenameChooser test_object( "Loader", "Saver" );

	CPPUNIT_ASSERT_EQUAL( std::string("Saver.chess"), test_object.save_file_as() );
}

void TestFilenameChooser::test_save_file_empty()
{
	MockFilenameChooser test_object( "Loader", "" );

	CPPUNIT_ASSERT_EQUAL( std::string(""), test_object.save_file() );
}

