/*
 File tools.cc
 Author Chan Sol Park, Noah Lugon
 Date 27.04.2020
 Version 3.2
 Architecture: Fig 11 b1
*/

#include <iostream>
#include <cmath>
#include "tools.h"
#include "graphic.h"

using namespace std;

double distance(Vecteur a, Vecteur b){
	double x1 = a.x;
	double y1 = a.y;
	double x2 = b.x;
	double y2 = b.y;
	
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

double norme(Vecteur v){
	return sqrt(v.x*v.x + v.y*v.y);
}

double dist_segment_vecteur(Ligne l, Vecteur p){
	Vecteur a = l.v1;
	Vecteur b = l.v2;
	
	Vecteur ab = b - a;
	Vecteur bp = p-b;
	Vecteur ap = p-a;
	
	if (produit_scalaire(ab,bp) >= 0.0) return distance(b,p);
	if (produit_scalaire(ab,ap) <= 0.0) return distance(a,p);
	
	else {
		Vecteur normale = {-(ab.y), ab.x};
		return abs(produit_scalaire(ap,normale) / norme(normale));	
	}
}

Vecteur operator-(Vecteur v1, Vecteur const& v2){
	v1.x -= v2.x;
	v1.y -= v2.y;
	
	return v1;
}

Vecteur operator+(Vecteur v1, Vecteur const& v2){
	v1.x += v2.x;
	v1.y += v2.y;
	
	return v1;
}

Vecteur operator*(double c, Vecteur v){
	v.x *= c;
	v.y *= c;
	
	return v;
}

double produit_scalaire(Vecteur v1, Vecteur v2){
	double x1 = v1.x;
	double y1 = v1.y;
	
	double x2 = v2.x;
	double y2 = v2.y;
	
	return (x1*x2 + y1*y2);
}

void dessiner_cercle(Cercle c, Color couleur){
	graphic_draw_arc(c.centre.x, c.centre.y, c.rayon, 0, 2*M_PI, couleur);
}

void dessiner_ligne(Vecteur l1, Vecteur l2, Color couleur){
	graphic_draw_line(l1.x, l1.y, l2.x, l2.y, couleur);
}

void dessiner_rectange(Vecteur v, double delta_x, double delta_y, Color couleur){
	graphic_draw_rectange(v.x, v.y, delta_x, delta_y, couleur);
}
