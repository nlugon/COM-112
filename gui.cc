/*
 File gui.cc
 Author Chan Sol Park, Noah Lugon
 Date 23.05.2020
 Version 5.3
 Architecture: Fig 11 b1
*/

#include "gui.h"
#include "graphic_gui.h"
#include "constantes.h"
#include "tools.h"
#include "ville.h"
#include <iostream>
#include <cairomm/context.h>
#include <sstream>

#define DEFAULT_ZOOM 1

using namespace std;

GraphArea::GraphArea(): 
	empty(false), shortest_path(false), edit_link(false), p1({0,0}), p2({0,0}), 
	type(LOGEMENT_), selected(false), current_id(-1), zoom_factor(DEFAULT_ZOOM)
	{}
	
GraphArea::~GraphArea() {}

void GraphArea::clear(){
	empty = true;
	refresh();
}

void GraphArea::draw(){
	empty = false;
	refresh();
}

long int GraphArea::get_current_id() const{
	return current_id;
}

void GraphArea::set_edit_link(bool in){
	edit_link = in;
	refresh();
}

void GraphArea::set_shortest_path(bool in){  
	shortest_path = in;
	refresh();
}

void GraphArea::set_type(Type t){
	type = t;
}

double GraphArea::get_zoom_factor() const{
	return zoom_factor;
}

void GraphArea::set_zoom_factor(double zoom){
	zoom_factor = zoom;
}

void GraphArea::re_init(){
	current_id = -1;
	selected = false;
	edit_link = false;
	shortest_path = false;
	zoom_factor = DEFAULT_ZOOM;
	type = LOGEMENT_;	
}

void GraphArea::set_point(Vecteur point){
	p1 = point;
	refresh();
}

void GraphArea::finalize_point(Vecteur point){
	p2 = point;

	long int id(-1);
	selected = noeud_selectionne(p1, id);
	
	if (edit_link){
		if (selected){
			link_operation(current_id, id);
		}
	}
	
	else {
		if (current_id == -1){
			if (id == -1) create_noeud(p1.x, p1.y, type);
			else if (id != -1){
				current_id = id;
			}
		}
		
		else {
			if (id == current_id){
				delete_indiv(id);
				current_id = -1;
			}
			else if (id == -1){
				if (p1.x == p2.x and p1.y == p2.y) current_id = -1;
				else modifier_rayon(current_id, p1, p2);
				 
			 }
			else if (id != current_id) current_id = id;
		}
	}
	refresh();
}

void GraphArea::move_point(Vecteur point){
	if (current_id != -1){
		modifier_centre(current_id, point);
	}
	refresh();
}

void GraphArea::refresh(){
	auto win = get_window();
	if (win){
		Gdk::Rectangle r(0,0, get_allocation().get_width(),     
		                     get_allocation().get_height()); 
		win->invalidate_rect(r,false);
	}
}

bool GraphArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	cr->set_source_rgb(1, 1, 1); // white
	cr->paint();
	if (not empty){
		graphic_set_context(cr);
		graphic_set_default_drawing_size(default_drawing_size);
		graphic_set_dim_max(dim_max/zoom_factor);
		graphic_set_dim_min();
		
		dessiner_ville(); 
		
		if (current_id != -1){
			 dessiner_selected(current_id);
			 if (shortest_path) dessiner_shortest_path(current_id);
		 }
	}
	return true;
}

string to_text(double num){
	ostringstream streamObj;
	streamObj << num;
	string strObj = streamObj.str();
	return strObj;
}

GUI::GUI():
	m_Box_All(Gtk::ORIENTATION_HORIZONTAL, 10), 
	m_Box_UI(Gtk::ORIENTATION_VERTICAL, 10),
	m_Box_General(Gtk::ORIENTATION_VERTICAL, 10),
	m_Box_Display(Gtk::ORIENTATION_VERTICAL, 10),
	m_Box_Editor(Gtk::ORIENTATION_VERTICAL, 10),
	m_Box_Informations(Gtk::ORIENTATION_VERTICAL, 10),
	m_Button_Exit("exit"), m_Button_New("new"), m_Button_Open("open"),
	m_Button_Save("save"), m_ToggleButton_Shortest_Path("shortest path"),
	m_Button_Zoom_In("zoom in"), m_Button_Zoom_Out("zoom out"),
	m_Button_Zoom_Reset("zoom reset"), m_ToggleButton_Edit_Link("edit link"), 
	m_Label_Zoom("zoom: x1.00"), m_Label_ENJ("ENJ: " + to_text(enj())), 
	m_Label_CI("CI: " + to_text(ci_total())), m_Label_MTA("MTA: " + to_text(mta())),
	m_RadioButton_Housing("housing"), m_RadioButton_Transport("transport"),
	m_RadioButton_Production("production")
{
	set_title("GUI : Archipelago");
	add(m_Box_All);
	m_Box_All.pack_start(m_Box_UI, false, false);
	m_Box_All.pack_start(m_Frame_Graph, true, true);
	m_Box_UI.set_border_width(10);
	m_Frame_Graph.set_border_width(10);
	m_Area.set_size_request(default_drawing_size, default_drawing_size); 
	m_Frame_Graph.add(m_Area);
	
	m_Frame_General.set_label("General");
	m_Frame_Display.set_label("Display");
	m_Frame_Editor.set_label("Editor");
	m_Frame_Informations.set_label("Informations");
	
	m_Box_UI.pack_start(m_Frame_General, false, false);
	m_Frame_General.add(m_Box_General);
	m_Box_General.pack_start(m_Button_Exit, false, false);
	m_Box_General.pack_start(m_Button_New, false, false);
	m_Box_General.pack_start(m_Button_Open, false, false);
	m_Box_General.pack_start(m_Button_Save, false, false);
	
	m_Box_UI.pack_start(m_Frame_Display, false, false);
	m_Frame_Display.add(m_Box_Display);
	m_Box_Display.pack_start(m_ToggleButton_Shortest_Path, false, false);
	m_Box_Display.pack_start(m_Button_Zoom_In, false, false);
	m_Box_Display.pack_start(m_Button_Zoom_Out, false, false);
	m_Box_Display.pack_start(m_Button_Zoom_Reset, false, false);
	m_Box_Display.pack_start(m_Label_Zoom, false, false);
	
	m_Box_UI.pack_start(m_Frame_Editor, false, false);
	m_Frame_Editor.add(m_Box_Editor);
	m_Box_Editor.pack_start(m_ToggleButton_Edit_Link);          
	m_RadioButton_Transport.join_group(m_RadioButton_Housing);
	m_RadioButton_Production.join_group(m_RadioButton_Housing);
	m_Box_Editor.pack_start(m_RadioButton_Housing);
	m_Box_Editor.pack_start(m_RadioButton_Transport);
	m_Box_Editor.pack_start(m_RadioButton_Production);
	
	m_Box_UI.pack_start(m_Frame_Informations, false, false);
	m_Frame_Informations.add(m_Box_Informations);
	m_Box_Informations.pack_start(m_Label_ENJ, false, false);
	m_Box_Informations.pack_start(m_Label_CI, false, false);
	m_Box_Informations.pack_start(m_Label_MTA, false, false);
	
	connect_button_signal();
                         
	show_all_children();
}

GUI::~GUI() {}

void GUI::connect_button_signal(){
	connect_general_buttons();
	connect_display_buttons();
	connect_editor_buttons();
}

void GUI::connect_general_buttons(){
	m_Button_Exit.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_exit)
				);
              
	m_Button_New.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_new)
				);
              
	m_Button_Open.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_open)
				);
              
	m_Button_Save.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_save)
				);
}

void GUI::connect_display_buttons(){
	m_ToggleButton_Shortest_Path.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_shortest_path)
				);    
               
	m_Button_Zoom_In.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_zoom_in)
				);
              
	m_Button_Zoom_Out.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_zoom_out)
				);
              
	m_Button_Zoom_Reset.signal_clicked().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_clicked_zoom_reset)
				);
}

void GUI::connect_editor_buttons(){
	m_ToggleButton_Edit_Link.signal_clicked().connect(
			sigc::mem_fun(*this,  
							  &GUI::on_button_clicked_edit_link)
				);
              
	m_RadioButton_Housing.signal_released().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_released_housing)
				);    
                     
	m_RadioButton_Transport.signal_released().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_released_transport)
				);
              
	m_RadioButton_Production.signal_released().connect(
			sigc::mem_fun(*this,
							  &GUI::on_button_released_production)
				);
}

void GUI::update_information_label(){
	m_Label_ENJ.set_label("ENJ: " + to_text(enj()));
	m_Label_CI.set_label("CI: " + to_text(ci_total()));
	m_Label_MTA.set_label("MTA: " + to_text(mta()));
}

void GUI::update_zoom_label(){
	m_Label_Zoom.set_label("zoom: x" +  to_text(m_Area.get_zoom_factor()));
}

void GUI::on_button_clicked_exit(){
	exit(EXIT_SUCCESS);
}

void GUI::reinit(){
	update_zoom_label();
	m_ToggleButton_Edit_Link.set_active(false);
	m_ToggleButton_Shortest_Path.set_active(false);
	m_RadioButton_Housing.set_active(true);
}

void GUI::on_button_clicked_new(){
	m_Label_ENJ.set_text("ENJ: 0");
	m_Label_CI.set_text("CI: 0");
	m_Label_MTA.set_text("MTA: 0");
	reset_ville();
	m_Area.re_init();
	m_Area.clear();
	m_Area.draw();
	reinit();
	
}

void GUI::on_button_clicked_open(){
	Gtk::FileChooserDialog dialog("Please choose a file to open",
								         Gtk::FILE_CHOOSER_ACTION_OPEN);     
	dialog.set_transient_for(*this);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);

	int result = dialog.run();

	switch (result){
		case (Gtk::RESPONSE_OK) :{
			reset_ville();
			string filename = dialog.get_filename();
			char *name = new char[filename.size() + 1];
			filename.copy(name, filename.size() + 1);
			name[filename.size()] = '\0';
			lecture(name);
			update_information_label();
			m_Area.re_init();
			reinit();

			break;
		}
		case (Gtk::RESPONSE_CANCEL) :{
			break;
		}
		default :{
			cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
	m_Area.draw();
}

void GUI::on_button_clicked_save(){
	Gtk::FileChooserDialog dialog("Please choose a file to save",
											Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	int result = dialog.run();

	switch (result){
		case (Gtk::RESPONSE_OK) :{
			string filename = dialog.get_filename();
			char *name = new char[filename.size() + 1];
			filename.copy(name, filename.size() + 1);
			name[filename.size()] = '\0';
			ecriture(name);
			break;
		}
		case (Gtk::RESPONSE_CANCEL) :{
			break;
		}
		default :{
			cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
}

void GUI::on_button_clicked_shortest_path(){ 
	if (m_ToggleButton_Shortest_Path.get_active() == true){
		m_Area.set_shortest_path(true);
	}
	else {
		m_Area.set_shortest_path(false);
	}                  
}

void GUI::zoom_in(){
	if (m_Area.get_zoom_factor() < max_zoom){
		m_Area.set_zoom_factor(m_Area.get_zoom_factor() + delta_zoom);
	}
	update_zoom_label();
	m_Area.draw();
}

void GUI::zoom_out(){
	if (m_Area.get_zoom_factor() > min_zoom + epsil_zero){
		m_Area.set_zoom_factor(m_Area.get_zoom_factor() - delta_zoom);
	}
	update_zoom_label();
	m_Area.draw();
}

void GUI::zoom_reset(){
	m_Area.set_zoom_factor(DEFAULT_ZOOM);
	update_zoom_label();
	m_Area.draw();
}

void GUI::on_button_clicked_zoom_in(){
	zoom_in();
}

void GUI::on_button_clicked_zoom_out(){
	zoom_out();
}

void GUI::on_button_clicked_zoom_reset(){
	zoom_reset();
}

void GUI::on_button_clicked_edit_link(){                        
	if (m_ToggleButton_Edit_Link.get_active() == true){
		if (get_nombre_noeud() < 2){
			cout << "There is nothing to edit : not enough nodes" << endl;
			m_ToggleButton_Edit_Link.set_active(false);
		}
		else if (m_Area.get_current_id() == -1){
			cout << "Please select first a node to edit" << endl;
			m_ToggleButton_Edit_Link.set_active(false);
		}
		else {
			m_Area.set_edit_link(true);
			m_ToggleButton_Edit_Link.set_active();
		}
	}
	else m_Area.set_edit_link(false);
}

void GUI::on_button_released_housing(){
	m_Area.set_type(LOGEMENT_);
}

void GUI::on_button_released_transport(){
	m_Area.set_type(TRANSPORT_);
}

void GUI::on_button_released_production(){
	m_Area.set_type(PRODUCTION_);
}

// Mouse event signal handlers:

bool GUI::on_button_press_event(GdkEventButton* event){
	if (event->type == GDK_BUTTON_PRESS){
		// raw mouse coordinates in the window frame
		double clic_x = event->x;
		double clic_y = event->y;
		
		// origin of the drawing area
		double origin_x = m_Area.get_allocation().get_x();
		double origin_y = m_Area.get_allocation().get_y();
		
		double width = m_Area.get_allocation().get_width();
		double height= m_Area.get_allocation().get_height();
		
		// retain only mouse events located within the drawing area
		if (clic_x >= origin_x and clic_x <= origin_x + width and
		    clic_y >= origin_y and clic_y <= origin_y + height){ 
			
			Vecteur p({clic_x - origin_x, clic_y - origin_y});
			
			if (event->button == 1){ // Left mouse button
				m_Area.set_point(p); 
			}
			else if (event->button == 3){ // Right mouse button
				m_Area.move_point(p);
				update_information_label();
			}
		}
	}
	return true;
}

bool GUI::on_button_release_event(GdkEventButton* event){
	if (event->type == GDK_BUTTON_RELEASE){
		// raw mouse coordinates in the window frame
		double clic_x = event->x ;
		double clic_y = event->y ;

		// origin of the drawing area
		double origin_x = m_Area.get_allocation().get_x();
		double origin_y = m_Area.get_allocation().get_y();
		
		double width = m_Area.get_allocation().get_width();
		double height= m_Area.get_allocation().get_height();
		
		// retain only mouse events located within the drawing area
		if (clic_x >= origin_x and clic_x <= origin_x + width and
		    clic_y >= origin_y and clic_y <= origin_y + height){ 
			
			Vecteur p({clic_x - origin_x, clic_y -origin_y});
			if (event->button == 1){ // Left mouse button
				m_Area.finalize_point(p);
				update_information_label();
			}
		}
	}
	return true;
}

// Keyboard signal handler:
bool GUI::on_key_press_event(GdkEventKey* key_event){
	if(key_event->type == GDK_KEY_PRESS){
		switch(gdk_keyval_to_unicode(key_event->keyval)){
			case 'r':
				zoom_reset();
				break;
			case 'i':
				zoom_in();
				break;
			case 'o':
				zoom_out();
				break;
		}
	}
	return Gtk::Window::on_key_press_event(key_event);
}
