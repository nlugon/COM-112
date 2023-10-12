#ifndef VILLE_HEADER_H
#define VILLE_HEADER_H

/*
 File ville.h
 Author Noah Lugon, Chan Sol Park
 Date 23.05.2020
 Version 6.2
 Architecture: Fig 11 b1
*/

#include <string>
#include <vector>
#include "noeud.h"  

void lecture(char * nom_fichier);
void decodage_ligne(std::string);
void erreur();
size_t get_nombre_noeud();
void dessiner_ville();
void reset_list();
void reset_ville();
void ecriture(char * nom_fichier);

void create_noeud(double x, double y, Type type);
void link_operation(unsigned int id1, unsigned int id2);
bool creation_condition(unsigned int uid, double x, double y, unsigned int nbp);
bool verifier_liens(unsigned int uid, double x, double y, double rayon, std::string);

void dessiner_shortest_path(unsigned int);     
void dessiner_selected(unsigned int);

void delete_indiv(unsigned int uid);
void modifier_rayon(unsigned int, Vecteur, Vecteur);
void modifier_centre(unsigned int, Vecteur);
bool noeud_selectionne(Vecteur p, long int& id);

double enj();
double ci_total();

class Ville{
private:
	std::vector<std::vector<unsigned int>> ville; 
	bool valid;
		
public:
	Ville(bool);
	bool addLogement(unsigned int, double, double, unsigned int);	
	bool addTransport(unsigned int, double, double, unsigned int);
	bool addProduction(unsigned int, double, double, unsigned int);
	bool addConnexion(unsigned int, unsigned int);
	void insertConnexion(unsigned int, unsigned int); 
	bool get_valid() const;
	void set_valid(bool);
	void enlever();
	std::vector<std::vector<unsigned int>> get_ville() const;
	void delete_connexion(size_t index);
};

#endif
