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
	MockFilenameChooser() {};
	virtual ~MockFilenameChooser() {};

private:
	virtual void set_working_dir( std::string working_dir ) {};
	virtual void set_filename( std::string aname ) {};
	virtual bool query_file() { return false; };
	virtual std::string	result( ) { return ""; };
};

void TestFilenameChooser::get_new_filename()
{
	MockFilenameChooser test_object;

	CPPUNIT_ASSERT_EQUAL( std::string(""), test_object.get_filename() );
}
