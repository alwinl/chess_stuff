/*
 * dialog_tester.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtkmm.h>
#include <memory>
#include <iostream>

static int show_dialog_from_ui(Gtk::Window& parent, const std::string& ui_file_path, const std::string& window_id)
{
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_file(ui_file_path);
    } catch (const Glib::Error& ex) {
        std::cerr << "Error loading UI file: " << ex.what() << std::endl;
        return -1;
    }

    auto dialog_window = builder->get_widget<Gtk::Window>(window_id);
    if (!dialog_window) {
        std::cerr << "Window with ID '" << window_id << "' not found in UI file." << std::endl;
        return -1;
    }

    dialog_window->set_transient_for(parent);
    dialog_window->set_visible(true);
    dialog_window->set_hide_on_close();

    int response = -1;
    auto loop = Glib::MainLoop::create();

    dialog_window->signal_close_request().connect(
        [&]() {
            response = Gtk::ResponseType::CLOSE;
            dialog_window->hide();
            loop->quit();
            return true;
        }, false
    );

    loop->run();

    // The window will be destroyed when the builder goes out of scope
    return response;
}

int main( int argc, char* argv[] )
{
    auto app = Gtk::Application::create("org.example.dialogwrapper");

    Gtk::Window main_window;
    main_window.set_default_size(400, 300);
    main_window.set_title("Main Window");

    // Connect to the activate signal
    app->signal_activate().connect([&]() {
        main_window.set_visible(true);

		auto css_provider = Gtk::CssProvider::create();
		css_provider->load_from_path("/home/alwin/Documents/Programming/chess_stuff/chess/guisrc/resources/app.css");
		Gtk::StyleContext::add_provider_for_display( Gdk::Display::get_default(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);


        // Use the wrapper to show the dialog loaded from a UI file
        const std::string ui_file_path = "/home/alwin/Documents/Programming/chess_stuff/chess/guisrc/resources/app.ui";

		std::vector<std::string> dialogs { "dlgLevel", "dlgColours", "dlgNewGame", "dlgInput", "dlgPieceValue" };
        // const std::string window_id = "dlgColours";

		std::for_each( dialogs.begin(), dialogs.end(), [&ui_file_path, &main_window]( std::string window_id )
			{
				int response = show_dialog_from_ui(main_window, ui_file_path, window_id);

				if (response == Gtk::ResponseType::OK) {
					g_print("OK pressed\n");
				} else if (response == Gtk::ResponseType::CLOSE) {
					g_print("Window closed\n");
				}
			}
		);
    });

    return app->run(argc, argv);
}
