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

#include "testdlginput.h"

#include "dlginput.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestDlgInput );


#include <gtkmm.h>

class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();

protected:
  //Signal handlers:
  void on_button_clicked();

  //Child widgets:
  Gtk::Button m_Button;
};



ExampleWindow::ExampleWindow() : m_Button("Show Dialog")
{
  set_title("Gtk::Dialog example");

  add( m_Button );
  m_Button.signal_clicked().connect(sigc::mem_fun(*this, &ExampleWindow::on_button_clicked) );

  show_all_children();
}

void ExampleWindow::on_button_clicked()
{
  Gtk::Dialog dialog;
  dialog.set_default_size(100,100);
  dialog.run();
}



TestDlgInput::TestDlgInput()
{
    //ctor
}

TestDlgInput::~TestDlgInput()
{
    //dtor
}

void TestDlgInput::testDialog()
{
//    auto app = Gtk::Application::create( "org.gtkmm.example" );

//    DlgInput dlg( "Test Dialog", "What is the value", "initial value" );
    //ExampleWindow window;

//    app->run(dlg);

/*
    DlgInput dlg( "Test Dialog", "What is the value", "initial value" );

    if( dlg.run() == Gtk::RESPONSE_ACCEPT ) {
        CPPUNIT_ASSERT(true);
        return;
    }
*/
    CPPUNIT_ASSERT(true);
}
