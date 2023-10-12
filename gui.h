/*
 File gui.h
 Author Chan Sol Park, Noah Lugon
 Date 23.05.2020
 Version 5.3
 Architecture: Fig 11 b1
*/

#ifndef GUI_HEADER_H
#define GUI_HEADER_H

#include <gtkmm.h>
#include <string>
#include "tools.h"
#include "noeud.h"

class GraphArea : public Gtk::DrawingArea{
public:
	GraphArea();
	virtual ~GraphArea();
	void clear();
	void draw();
	void set_shortest_path(bool);
	void set_noeud(Vecteur);
	void set_type(Type);
	void set_point(Vecteur);
	void move_point(Vecteur);
	void finalize_point(Vecteur);
	double get_zoom_factor() const;
	void set_zoom_factor(double);
	long int get_current_id() const;
	void set_edit_link(bool);
	void re_init(); 
	
protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
	bool empty;
	void refresh();
	bool shortest_path;  
	bool edit_link;
	Vecteur p1;
	Vecteur p2;
	Type type;
	bool selected;
	long int current_id; // equals to -1 if no node is currently selected
	double zoom_factor;
};

std::string to_text(double num);

class GUI : public Gtk::Window{
public:
	GUI();
	virtual ~GUI();
protected:
	GraphArea m_Area;
	Gtk::Box m_Box_All, m_Box_UI;
	Gtk::Box m_Box_General, m_Box_Display, m_Box_Editor, m_Box_Informations;
	Gtk::Frame m_Frame_General, m_Frame_Display, m_Frame_Editor;
	Gtk::Frame m_Frame_Informations, m_Frame_Graph;
	Gtk::Button m_Button_Exit, m_Button_New, m_Button_Open, m_Button_Save;
	Gtk::ToggleButton m_ToggleButton_Shortest_Path;    
	Gtk::Button m_Button_Zoom_In, m_Button_Zoom_Out, m_Button_Zoom_Reset;
	Gtk::ToggleButton m_ToggleButton_Edit_Link;      
	Gtk::Label m_Label_Zoom, m_Label_ENJ, m_Label_CI, m_Label_MTA;
	Gtk::RadioButton m_RadioButton_Housing, m_RadioButton_Transport;
	Gtk::RadioButton m_RadioButton_Production;
	void on_button_clicked_exit();
	void on_button_clicked_new();
	void on_button_clicked_open();
	void on_button_clicked_save();
	void on_button_clicked_shortest_path();
	void on_button_clicked_zoom_in();
	void on_button_clicked_zoom_out();
	void on_button_clicked_zoom_reset();
	void on_button_clicked_edit_link();
	void on_button_released_housing();
	void on_button_released_transport();
	void on_button_released_production();
	bool on_button_press_event(GdkEventButton* event);
	bool on_button_release_event(GdkEventButton* event);
	bool on_key_press_event(GdkEventKey* key_event);
private:
	void draw();
	void update_information_label(), update_zoom_label();
	void connect_button_signal(), connect_general_buttons();
	void connect_display_buttons(), connect_editor_buttons();
	void zoom_in(), zoom_out(), zoom_reset();
	void reinit();
};

#endif
