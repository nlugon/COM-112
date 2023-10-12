/*
 File noeud.cc
 Author Chan Sol Park, Noah Lugon
 Date 23.05.2020
 Version 3.5
 Architecture: Fig 11 b1
*/
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "noeud.h"
#include "constantes.h"
#include "error.h"

#define THETA1 M_PI/4
#define THETA2 5*M_PI/4
#define PHI1 3*M_PI/4
#define PHI2 7*M_PI/4
#define X_RATIO 0.75
#define Y_RATIO 0.125

using namespace std;

static vector<Noeud*> collection;

bool verifier_modification(unsigned int uid, double x, double y, double rayon,
									string error){
	for (auto const& element : collection){
		Cercle position = element->get_position();
		double dist = distance({x,y}, position.centre);
		double somme_rayon = rayon + position.rayon;
		if (dist <= somme_rayon + dist_min and uid != element->get_uid()) {
			cout << "Error " << error 
			     << " : there is a collision between the nodes \n";
			return false;
			}
		}
	return true;
}

Ligne get_link(unsigned int id1, unsigned int id2){
	Noeud* const& n1 = uid_to_pointer(id1);
	Noeud* const& n2 = uid_to_pointer(id2);
	
	Cercle c1 = n1->get_position();
	Cercle c2 = n2->get_position();
	
	return {c1.centre, c2.centre};
}

bool verify_one_link(double x, double y, double rayon, unsigned int id1, 
                       unsigned int id2, string error){
	Ligne link(get_link(id1, id2));
	double dist(dist_segment_vecteur(link, {x, y}));
	if (dist <= rayon + dist_min){
		cout << "Error " << error << " : some links collide with some nodes \n";
		return false;
	}
	
	return true;
}

size_t get_collection_size(){
	return collection.size();
}

bool nbp_condition(unsigned int nbp){
	if (nbp < min_capacity){
		cout << error::too_little_capacity(nbp);
		return false;
	}
	
	if (nbp > max_capacity){
		cout << error::too_much_capacity(nbp);
		return false;
	}
	return true;
}

bool noeud_condition(unsigned int uid, double x, double y, unsigned int nbp){
	if (!nbp_condition(nbp)) return false;
	
	if (uid >= reserved){
		cout << error::reserved_uid();
		return false;
	}
	
	unsigned int occurence = 0;
	
	for (auto const& element : collection){
		if (element -> get_uid() == uid) ++occurence;
		if (occurence > 0) {
			cout << error::identical_uid(uid);
			return false;
		}
	}
	
	if (!verifier_noeud_collision(uid, x, y, nbp, true)) return false;
	
	return true;
}
 
bool verifier_noeud_collision(unsigned int uid, double x, double y, unsigned int nbp,
                              bool lecture){								
	for (auto const& element : collection){
		Cercle position = element->get_position();
		double dist = distance({x,y}, position.centre);
		double somme_rayon = sqrt(nbp) + position.rayon;
		
		if (lecture and dist <= somme_rayon){
			cout << error::node_node_superposition(uid, element->get_uid());
			return false;
		}
		
		if (!lecture and dist <= somme_rayon + dist_min){
			cout << "Error adding node : collision with existing node" << endl;
			return false;
		}
	}
	
	return true;
}

bool verifier_link_logic(unsigned int a, unsigned int b, bool ecriture){
	if (a == b){
		if (ecriture) cout << error::self_link_node(a);
		else cout << "Error creating link : link cannot connect to itself \n";
		return false;
	}
	
	return true;
}

bool verifier_existence(unsigned int n, unsigned int& indice){
	for (size_t i(0); i < collection.size(); ++i){
		if (collection[i] -> get_uid() == n){
			indice = i;
			return true;
		}
	}
	cout << error::link_vacuum(n);
	return false;
}

bool verifier_unique_link(const Noeud& n1, const Noeud& n2){
	for (auto const& el : n1.get_connexion()){
		if (el == &n2){
			cout << error::multiple_same_link(n1.get_uid(), n2.get_uid());
			return false;
		}
	}
	return true;
}

bool verifier_logement(const Noeud& n, bool ecriture){
	if (n.get_type() == LOGEMENT_ && n.get_nb_link() == max_link){
		if (ecriture) cout << error::max_link(n.get_uid());
		else cout << "Error creating link : " 
				  << "selected node is already at maximum link capacity \n";
		return false;
	}

	return true;
}

bool verifier_espace(const Noeud& n1, const Noeud& n2, bool ecriture){
	Ligne l1 = {n1.get_position().centre, n2.get_position().centre};
	
	for (auto const& noeud : collection){
		unsigned int uid = noeud->get_uid();
		
		if (uid != n1.get_uid() and uid != n2.get_uid()){
			Cercle position = noeud -> get_position();
			double d1 = dist_segment_vecteur(l1, position.centre);
			double r = position.rayon;
			
			if ((d1 <= r) && ecriture){
				cout << error::node_link_superposition(uid);
				return false;
			}
			
			if (!ecriture && (d1 <= r + dist_min)){
				cout << "Error creating link : " 
				     << error::node_link_superposition(uid);
				return false;
			}
		}
	}
	return true;
}

bool link_condition(unsigned int a, unsigned int b){	
	if (!verifier_link_logic(a, b, true)) return false;
	
	unsigned int indice1;
	unsigned int indice2;
	
	if (!verifier_existence(a, indice1)) return false; 
	if (!verifier_existence(b, indice2)) return false;
	
	Noeud& n1(*collection[indice1]);
	Noeud& n2(*collection[indice2]);
	
	if (!verifier_unique_link(n1, n2)) return false;
	
	if (!verifier_logement(n1, true)) return false;	
	if (!verifier_logement(n2, true)) return false;
	
	if (!verifier_espace(n1, n2, true)) return false;

	n1.add_connexion(&n2);
	n2.add_connexion(&n1);
	
	return true;
}

bool connect(unsigned int a, unsigned int b){
	if (!link_condition(a, b)){
		delete_noeuds();
		return false;
	}
	
	return true;
}

bool link_creation_condition(unsigned int a, unsigned int b){
	if (!verifier_link_logic(a, b, false)) return false;
	
	Noeud& n1 = *(uid_to_pointer(a));
	Noeud& n2 = *(uid_to_pointer(b));
	
	if (!verifier_logement(n1, false)) return false;
	if (!verifier_logement(n2, false)) return false;
	
	if (!verifier_espace(n1, n2, false)) return false;
	
	n1.add_connexion(&n2);
	n2.add_connexion(&n1);
	
	return true;
}

bool verifier_liens_geometrie(unsigned int uid, double x, double y){
	Noeud* nptr = uid_to_pointer(uid);
	for (auto connection : nptr->get_connexion()){
		for (auto n : collection){
			if (n->get_uid() != connection->get_uid() && n->get_uid() != uid){
				Ligne link = {{x,y}, connection->get_position().centre};
				double dist = dist_segment_vecteur(link, n->get_position().centre);
				if (dist <= (n->get_position().rayon + dist_min)){
					cout << "Error moving node : some links collide with some nodes";
					cout << endl;
					return false;
				}
			}
		}
	}
	return true;
}

Noeud::Noeud(unsigned int id, double x, double y, unsigned int n){	
	uid = id;
	nbp = n;
	position.centre = {x, y};
	position.rayon = sqrt(n);
	nb_link = 0;
	
	in = true;
	access = infinite_time;
	parent = no_link;
}

Noeud::~Noeud(){}

Logement::~Logement(){}

Transport::~Transport(){}

Production::~Production(){}

bool add_noeud(unsigned int id, double x, double y, unsigned int n, Type t){
	bool condition = noeud_condition(id, x, y, n);
	
	if (!condition){
		delete_noeuds();
		return false;
	}
	
	make_noeud(id, x, y, n, t);
	
	return true;
}

void make_noeud(unsigned int id, double x, double y, unsigned int n, Type t){
	if (t == LOGEMENT_){
		Noeud* ptr(new Logement(id, x, y, n));
		collection.push_back(ptr);
	}
	
	if (t == TRANSPORT_){
		Noeud* ptr(new Transport(id, x, y, n));
		collection.push_back(ptr);
	}
	
	if (t == PRODUCTION_){
		Noeud* ptr(new Production(id, x, y, n));
		collection.push_back(ptr);
	}
}

void delete_noeuds(){
	for (int i(collection.size() - 1); i >= 0; --i){
		delete collection[i];
		collection.pop_back();
	}
}

unsigned int Noeud::get_uid() const{
	return uid;
}

unsigned int Noeud::get_nbp() const{
	return nbp;
}

Type Noeud::get_type() const{
	return type;
}

Cercle Noeud::get_position() const{
	return position;
}

vector<Noeud*> Noeud::get_connexion() const{
	return connexion;
}

unsigned int Noeud::get_nb_link() const{
	return nb_link;
}

void Noeud::add_connexion(Noeud* const& n){
	connexion.push_back(n);
	++nb_link;
}

Logement::Logement(unsigned int id, double x, double y, unsigned int n)
: Noeud(id, x, y, n) {type = LOGEMENT_;}

Transport::Transport(unsigned int id, double x, double y, unsigned int n)
: Noeud(id, x, y, n) {type = TRANSPORT_;}

Production::Production(unsigned int id, double x, double y, unsigned int n)
: Noeud(id, x, y, n) {type = PRODUCTION_;}

// Dessin ----------------------------------------------------------------------------

void Logement::dessiner(Color c) const{
	dessiner_cercle(position, c);
}	
	
void Transport::dessiner(Color c) const{                                         
	dessiner_cercle(position, c);
	double xc = position.centre.x;
	double yc = position.centre.y;
	double r = position.rayon;
	
	dessiner_ligne({xc + r, yc}, {xc - r, yc}, c);
	dessiner_ligne({xc, yc + r}, {xc, yc - r}, c);
	dessiner_ligne({xc + r*cos(THETA1), yc + r*sin(THETA1)}, 
						{xc + r*cos(THETA2), yc + r*sin(THETA2)}, c);
	dessiner_ligne({xc + r*cos(PHI1), yc + r*sin(PHI1)}, 
						{xc + r*cos(PHI2), yc + r*sin(PHI2)}, c);		
}

void Production::dessiner(Color c) const{                                          
	dessiner_cercle(position, c);
	double xc = position.centre.x;
	double yc = position.centre.y;
	double r = position.rayon;
	
	dessiner_rectange({xc - X_RATIO*r, yc + Y_RATIO*r},
							2.0*X_RATIO*r, 2.0*Y_RATIO*r, c);
}

void dessiner_noeuds(){
	for (auto const& nptr : collection){
		nptr->dessiner();
	}
}

void colorer_noeud(unsigned int uid, Color c){
	Noeud* nptr = uid_to_pointer(uid);
	nptr->dessiner(c);
}

void dessiner_liens(Noeud* const& nptr1, Noeud* const& nptr2, Color c){          
	Cercle a = nptr1->get_position();
	Cercle b = nptr2->get_position();
	
	dessiner_ligne({a.centre.x, a.centre.y}, {b.centre.x, b.centre.y}, c);
}

void dessiner_shortest_path_to(Noeud* const& nptr, Type type){ 
	unsigned int start(get_index(nptr));
	unsigned int finish(nptr->closest(type));
	
	if (finish != no_link and nptr->get_type() == LOGEMENT_){
		while(finish != start){
			unsigned int parent(collection[finish]->get_parent());
			dessiner_liens(collection[finish], collection[parent], GREEN);
			collection[finish]->dessiner(GREEN);
			finish = parent;
		}
	}
	nptr->dessiner(RED);
}

unsigned int new_uid(){
	unsigned int uid(0);
	do {
		++uid;
	} while(!stop(uid));
	return uid;
}

bool stop(unsigned int uid){
	for (auto element : collection){
		if (uid == element->get_uid()) return false;
	}
	return true;	
}

bool in_noeud(Vecteur p, long int& uid){
	uid = -1;
	for (auto element : collection){
		double dist(distance(p, element->get_position().centre));
		if (dist <= element->get_position().rayon){
			uid = element->get_uid();	
			return true;
		}	
	}
	return false;
}

void delete_one_noeud(unsigned int uid){              
	Noeud* nptr(nullptr);
	nptr = uid_to_pointer(uid);
	for (auto const& element : nptr->get_connexion()){
		element->remove_connexion(nptr);
	}
	
	size_t taille(collection.size());
	unsigned int index(get_index(nptr));
	Noeud* temp(nullptr);
	temp = collection[index];
	collection[index] = collection[taille-1];
	collection[taille-1] = temp;
	temp = nullptr;
	
	collection.pop_back();
	delete nptr;	
}

void Noeud::remove_connexion(Noeud* const& nptr){
	size_t taille(connexion.size());
	bool popback(false);
	for (size_t i(0); i<taille; ++i){
		if (connexion[i] == nptr){
			Noeud* temp(nullptr);
			temp = connexion[i];
			connexion[i] = connexion[taille-1];
			connexion[taille-1] = temp; 
			temp = nullptr;
			
			popback = true;
		}
	}
	if (popback){
		nb_link--;
		connexion.pop_back();
	}
}

void Noeud::set_rayon(double r){
	position.rayon = r;
	nbp = r * r;
}

void Noeud::set_centre(Vecteur c){
	position.centre = c;
}


// Fonctions pour les 3 critères------------------------------------------------------

double Noeud::get_access() const{
	return access;	
}

bool Noeud::get_in() const{
	return in;
}

unsigned int Noeud::get_parent() const{
	return parent;
}

bool non_vide(){
	for (auto const& element : collection){
		if (element->get_in() == true) return true; 
	}
	return false;
}
//retourne l'indice dans le tableau collection du type de noeud désiré le plus proche 
unsigned int Noeud::closest(Type to){ 
	// initialisation du tableau ta  
	vector<unsigned int> ta;
	ta.push_back(get_index(this));
	for (auto const& element : collection){
		if (element != this) ta.push_back(get_index(element));
	}
	// initialisation des attributs in, access et parent
	for (auto const& element : collection){
		if (to == TRANSPORT_ and element->type == PRODUCTION_) element->in = false;
		else element->in = true;
		element->access = infinite_time;
		element->parent = no_link;
		}
	this->access = 0.;  
	
   while(non_vide()){
		unsigned int n(find_min_access());
		if (n == no_link) return n;
		if (collection[n]->type == to) return n;
		
		collection[n]->in = false;
		for (auto const& lv : (collection[n])->connexion){
			unsigned int indice_lv(get_index(lv));
			if (collection[indice_lv]-> in == true){
				double alt(collection[n]->access + compute_access(collection[n], lv)); 
				if (collection[indice_lv]->access > alt){
					collection[indice_lv]->access = alt;
					collection[indice_lv]->parent = n;
					sort(ta.begin(), ta.end(), compare);
				}
			}
		}
	}
	return no_link;
}

double vitesse_connexion(Noeud* const& a, Noeud* const& b){
	if (a->get_type() == TRANSPORT_ and b->get_type() == TRANSPORT_) return fast_speed;
	else return default_speed;
}

double compute_access(Noeud* const& a, Noeud* const& b){
	double dist = distance(a->get_position().centre, b->get_position().centre); 
	return dist/vitesse_connexion(a, b);                                                
}

unsigned int find_min_access(){
	double min_access(infinite_time);
	unsigned int n(no_link);
	for (auto const& element : collection){
		if (element->get_access() < min_access and element->get_in() == true){
			 min_access = element->get_access();
			 n = get_index(element);
		} 
	}
	return n;
}

// renvoie l'indice dans le tableau collection
unsigned int get_index(Noeud* const& a){ 
	if (a == nullptr){ 
		return 0;
	}
	
	for (unsigned int i(0); i < collection.size(); ++i){
		if (collection[i] == a) return i;
	}
	return 0; 										
}

bool compare(unsigned int a, unsigned int b){
	return (collection[a]->get_access()) < (collection[b]->get_access());
}

double mta(){                            
	double mta(0.);
	int compteur(0);
	for (auto const& element : collection){
		if (element->get_type() == LOGEMENT_){
			++compteur;
			unsigned int closest_production(element->closest(PRODUCTION_));
			if (closest_production == no_link) mta += infinite_time;
			else mta += collection[closest_production]->get_access(); 
			
			unsigned int closest_transport(element->closest(TRANSPORT_));
			if (closest_transport == no_link) mta += infinite_time;
			else mta += collection[closest_transport]->get_access();
		}
	}
	if (compteur != 0) mta /= compteur;
	return mta;
}

unsigned int somme_nbp(Type type){
	unsigned int somme(0);
	for (auto const& element : collection){
		if (element->get_type() == type) somme += element->get_nbp();
	}
	return somme;
}

Noeud* uid_to_pointer(unsigned int uid){
	for (auto const& element : collection){
		if (element->get_uid() == uid and element != nullptr) return element;
	} 
	return nullptr;
}

double ci(unsigned int uid1, unsigned int uid2){
	Noeud* const& a(uid_to_pointer(uid1));
	Noeud* const& b(uid_to_pointer(uid2));
	double dist( distance(a->get_position().centre, b->get_position().centre) );
	double capacity( min(a->get_nbp(), b->get_nbp()) );
	double vitesse(vitesse_connexion(a,b));
	
	return dist*capacity*vitesse;
}

int nombre_noeud(Type type){
	int compteur(0);
	for (auto const& el : collection){
		if (el->get_type() == type) ++compteur;
	}
	return compteur;
}

// -----------------------------------------------------------------------------------

void ecriture_noeud(Type type, ofstream& fsortie){                    
	fsortie << setprecision(6) << fixed;
	for (auto const& el : collection){
		if (el->get_type() == type){
			fsortie << "\t" << el->get_uid() << " ";
			fsortie << el->get_position().centre.x << " ";
			fsortie << el->get_position().centre.y << " ";
			fsortie << el->get_nbp() << endl;  
		}
	}
	fsortie << endl;
}
