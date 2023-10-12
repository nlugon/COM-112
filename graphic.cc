/*
 File graphic.cc
 Author Chan Sol Park, Noah Lugon
 Date 17.05.2020
 Version 1.2
 Architecture: Fig 11 b1
*/

#include <cmath>
#include <iostream>
#include "graphic_gui.h"

using namespace std;

static double dim_max;
static double dim_min;
static double default_drawing_size;
constexpr int default_line_width = 3;

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr){
	static bool init(false);

	if (!init)
	{
		ptcr = &cr;
		init = true;
	}
}

void graphic_set_default_drawing_size(const double c){
	default_drawing_size = c;
}

void graphic_set_dim_max(const double d){
	dim_max = d;
}

void graphic_set_dim_min(){
	dim_min = -dim_max;
}

// conversion de coordonnées de l'espace du Modèle à la fenêtre de dessin
double xm_to_xf(double xm){
	return default_drawing_size*(xm - dim_min)/(dim_max - dim_min);
}

double ym_to_yf(double ym){
	return default_drawing_size*(dim_max - ym)/(dim_max - dim_min);
}

// conversion de coordonnées de la fenêtre de dessin à l'espace du Modèle
double xf_to_xm(double xf){
	return xf*(dim_max - dim_min)/default_drawing_size + dim_min;
}

double yf_to_ym(double yf){
	return dim_max - yf*(dim_max - dim_min)/default_drawing_size;
}

double scaler(double c){
	return default_drawing_size*c/(dim_max - dim_min);
}

void set_color(const Cairo::RefPtr<Cairo::Context>& cr, Color c){
	switch(c){
		case BLACK:
			cr->set_source_rgb(0, 0, 0);
			break;
		case WHITE:
			cr->set_source_rgb(1, 1, 1);
			break;
		case RED:
			cr->set_source_rgb(0.8, 0, 0);
			break;
		case BLUE:
			cr->set_source_rgb(0, 0, 0.8);
			break;
		case GREEN:
			cr->set_source_rgb(0, 0.8, 0);
			break;
		case PURPLE:
			cr->set_source_rgb(0.8, 0, 0.8);
			break;
		default:
			cr->set_source_rgb(0, 0, 0);
	}
}

void graphic_draw_arc(double x, double y, double r, double phi, double theta, Color c){
	const Cairo::RefPtr<Cairo::Context>& cr = (*ptcr);
	
	double xf = xm_to_xf(x);
	double yf = ym_to_yf(y);
	double rf = scaler(r);
	
	cr->set_line_width(default_line_width);
	
	cr->arc(xf, yf, rf, phi, theta);
	set_color(cr, WHITE);
	cr->fill();
	
	set_color(cr, c);
	cr->arc(xf, yf, rf, phi, theta);

	cr->stroke();
}

void graphic_draw_line(double x1m, double y1m, double x2m, double y2m, Color c){
	const Cairo::RefPtr<Cairo::Context>& cr = (*ptcr);
	
	double x1f = xm_to_xf(x1m);
	double y1f = ym_to_yf(y1m);
	
	double x2f = xm_to_xf(x2m);
	double y2f = ym_to_yf(y2m);
	
	set_color(cr, c);
	cr->set_line_width(default_line_width);
	
	cr->move_to(x1f, y1f);
	cr->line_to(x2f, y2f);
	
	cr->stroke();
}

void graphic_draw_rectange(double x, double y, double deltaX, double deltaY, Color c){
	const Cairo::RefPtr<Cairo::Context>& cr = (*ptcr);
	
	double xf = xm_to_xf(x);
	double yf = ym_to_yf(y);
	
	double delta_xf = scaler(deltaX);
	double delta_yf = scaler(deltaY);
	
	set_color(cr, c);
	cr->set_line_width(default_line_width);
	
	cr->move_to(xf, yf);
	cr->line_to(xf + delta_xf, yf);
	cr->line_to(xf + delta_xf, yf + delta_yf);
	cr->line_to(xf, yf + delta_yf);
	cr->line_to(xf, yf);
	cr->line_to(xf + delta_xf, yf);
	
	cr->stroke();
}
