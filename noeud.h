#ifndef NOEUD_HEADER_H
#define NOEUD_HEADER_H

/*
 File noeud.h
 Author Chan Sol Park, Noah Lugon
 Date 23.05.2020
 Version 3.5
 Architecture: Fig 11 b1
*/

#include <vector>
#include <fstream>
#include <string>
#include "tools.h"

enum Type {LOGEMENT_, TRANSPORT_, PRODUCTION_};

constexpr unsigned int reserved = 4294967295;

class Noeud;

void print_radius();
Ligne get_link(unsigned int, unsigned int);
bool verify_one_link(double, double, double, unsigned int, unsigned int, std::string);
size_t get_collection_size();
bool nbp_condition(unsigned int);
bool verifier_modification(unsigned int, double, double, double, std::string);
bool verifier_noeud_collision(unsigned int, double, double, unsigned int, bool);
void make_noeud(unsigned int id, double x, double y, unsigned int n, Type t);
bool noeud_condition(unsigned int uid, double x, double y, unsigned int nbp);

bool verifier_link_logic(unsigned int, unsigned int, bool ecriture);
bool verifier_existence(unsigned int, unsigned int&);
bool verifier_unique_link(const Noeud&, const Noeud&);
bool verifier_logement(const Noeud&, bool ecriture);
bool verifier_espace(const Noeud&, const Noeud&, bool ecriture); 
bool link_creation_condition(unsigned int a, unsigned int b); 
bool verifier_liens_geometrie(unsigned int uid, double x, double y);

bool link_condition(unsigned int, unsigned int);
bool connect(unsigned int, unsigned int);
void dessiner_noeuds();
void dessiner_liens(Noeud* const& nptr1, Noeud* const& nptr2, Color c = BLACK);
void colorer_noeud(unsigned int uid, Color c);
void dessiner_shortest_path_to(Noeud* const&, Type);
bool add_noeud(unsigned int, double, double, unsigned int, Type);
void delete_noeuds();

unsigned int new_uid();
bool stop(unsigned int);
bool in_noeud(Vecteur, long int&);
void delete_one_noeud(unsigned int uid);

unsigned int get_index(Noeud* const&);  
bool compare(unsigned int, unsigned int);
unsigned int find_min_access();
double compute_access(Noeud* const&, Noeud* const&);
double mta();
unsigned int somme_nbp(Type);
Noeud* uid_to_pointer(unsigned int);
double vitesse_connexion(Noeud* const&, Noeud* const&);
double ci(unsigned int, unsigned int);
void ecriture_noeud(Type, std::ofstream&);
int nombre_noeud(Type);

class Noeud{
protected:
	unsigned int uid;
	unsigned int nbp;
	Type type;
	unsigned int nb_link;
	unsigned int indice;
	Cercle position;
	std::vector<Noeud*> connexion;
    
	bool in;
	double access;
	unsigned int parent;
	
public:
	Noeud(unsigned int, double, double, unsigned int);
	virtual ~Noeud();
	unsigned int get_uid() const;
	unsigned int get_nbp() const;
	Cercle get_position() const;
	Type get_type() const;
	void set_rayon(double);
	void set_centre(Vecteur);
	
	void remove_connexion(Noeud* const& nptr);
	
	unsigned int get_nb_link() const;
	std::vector<Noeud*> get_connexion() const;
	void get_noeud() const;
	void add_connexion(Noeud* const&); 
	virtual void dessiner(Color c = BLACK) const = 0; 
	
	double get_access() const;
	bool get_in() const;
	unsigned int get_parent() const;
	unsigned int closest(Type); 
};

class Logement : public Noeud{
public:
	Logement(unsigned int, double, double, unsigned int);
	~Logement() override;
	void dessiner(Color c = BLACK) const override;
};

class Transport : public Noeud{
public:
	Transport(unsigned int, double, double, unsigned int);
	~Transport() override;
	void dessiner(Color c = BLACK) const override;
};

class Production : public Noeud{
public:
	Production(unsigned int, double, double, unsigned int);
	~Production() override;
	void dessiner(Color c = BLACK) const override;
};

#endif
