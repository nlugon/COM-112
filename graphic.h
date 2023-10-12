#ifndef GRAPHIC_HEADER_H
#define GRAPHIC_HEADER_H

/*
 File graphic.h
 Author Chan Sol Park, Noah Lugon
 Date 17.05.2020
 Version 1.2
 Architecture: Fig 11 b1
*/

#include "graphic_color.h"

void graphic_draw_arc(double x, double y, double r, double u, double v, Color c);
void graphic_draw_line(double x, double y, double a, double b, Color c);
void graphic_draw_rectange(double x, double y, double d_x, double d_y, Color c);

double xf_to_xm(double xf);
double yf_to_ym(double yf);

#endif
