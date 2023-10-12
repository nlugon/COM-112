#ifndef GRAPHIC_GUI_HEADER_H
#define GRAPHIC_GUI_HEADER_H

/*
 File graphic_gui.h
 Author Chan Sol Park
 Date 15.05.2020
 Version 1.2
 Architecture: Fig 11 b1
*/

#include <gtkmm/drawingarea.h>
#include "graphic.h"

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);
void graphic_set_default_drawing_size(const double c);
void graphic_set_dim_max(const double d);
void graphic_set_dim_min();

#endif
