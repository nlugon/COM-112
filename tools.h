#ifndef TOOLS_HEADER_H
#define TOOLS_HEADER_H

/*
 File tools.h
 Author Chan Sol Park, Noah Lugon
 Date 27.04.2020
 Version 3.2
 Architecture: Fig 11 b1
*/

#include "graphic_color.h"

struct Vecteur{
	double x;
	double y;
};

Vecteur operator-(Vecteur, const Vecteur&);
Vecteur operator+(Vecteur, const Vecteur&);
Vecteur operator*(double, Vecteur);

struct Ligne{
	Vecteur v1;
	Vecteur v2;
};

struct Cercle{
	Vecteur centre;
	double rayon;
};

double distance(Vecteur, Vecteur);
double norme(Vecteur);
double dist_segment_vecteur(Ligne, Vecteur);
double produit_scalaire(Vecteur, Vecteur);

void dessiner_cercle(Cercle, Color couleur);
void dessiner_ligne(Vecteur l1, Vecteur l2, Color couleur);
void dessiner_rectange(Vecteur v, double a, double b, Color couleur);

#endif
