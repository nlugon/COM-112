/*
 File projet.cc
 Author Noah Lugon, Chan Sol Park
 Date 27.03.2020
 Version 6.2
 Architecture: Fig 11 b1
*/

#include <cstdlib>
#include <gtkmm.h>
#include <string>
#include <iostream>
#include <fstream>			
#include <sstream>
#include "ville.h"
#include "gui.h"

using namespace std;

int main (int argc, char * argv[]){
	if (argc == 2){
		lecture(argv[1]);
	}                 
	argc = 1;
	
	auto app = Gtk::Application::create(argc, argv, "org.gtkmm");
	
	GUI eventWindow;
	eventWindow.set_resizable(true);
    
	return app->run(eventWindow);
}
