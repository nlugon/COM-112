/*
 File ville.cc
 Author Noah Lugon, Chan Sol Park
 Date 23.05.2020
 Version 6.2
 Architecture: Fig 11 b1
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>			
#include <sstream>
#include <vector>
#include <cmath>
#include "ville.h"
#include "error.h"
#include "graphic.h"
#include "constantes.h"

using namespace std;

enum Etat_lecture {NB0, LOGEMENT, NB1, TRANSPORT, 
				       NB2, PRODUCTION, NB3, CONNEXION,
};

static Ville ma_ville(true);
 
void lecture(char * nom_fichier){
   string line;
   ifstream fichier(nom_fichier);
     		
   if (!fichier.fail()){
		while (getline(fichier >> ws,line) && ma_ville.get_valid()){
			if (line[0] == '#')  continue;
			if (line[0] == '\\' and (line[1] == 'n' or line[1] == 'r'))  continue;        
				decodage_ligne(line);
		}
	}
	
	else cout << "error opening file" << endl;			
}
                                                                           
void decodage_ligne(string line){
	istringstream data(line);		
	static int etat(NB0);                         
	static int i(0), total(0);	
	unsigned int uid(0), uid1(0), uid2(0);
	double x(0.0), y(0.0);
	unsigned int nbp(0); 

	switch (etat){
		case NB0 : 
			if (!(data >> total)) erreur();    
			else i=0 ;
			if (total==0) etat = NB1; 
			else etat=LOGEMENT ;  
			break;
		case LOGEMENT : 
			if (!(data >> uid >> x >> y >> nbp)) erreur();
			else ++i;
			if (i == total) etat = NB1 ;
			if (!ma_ville.addLogement(uid, x, y, nbp)) etat = NB0;
			break;
			
		case NB1 : 
			if (!(data >> total)) erreur(); 
			else i=0 ;
			if (total == 0) etat = NB2; 
			else etat = TRANSPORT ;  
			break;
		case TRANSPORT : 
			if(!(data >> uid >>  x >> y >> nbp)) erreur();
			else ++i;
			if(i == total) etat = NB2 ;
			if (!ma_ville.addTransport(uid, x, y, nbp)) etat = NB0;
			break;
			
		case NB2 : 
			if (!(data >> total)) erreur(); 
			else i = 0;
			if (total == 0) etat = NB3; 
			else etat = PRODUCTION ;  
			break;
		case PRODUCTION : 
			if (!(data >> uid >> x >> y >> nbp)) erreur();
			else ++i;
			if (i == total) etat = NB3 ; 
			if (!ma_ville.addProduction(uid, x, y, nbp)) etat = NB0;
			break;
			
		case NB3 : 
			if (!(data >> total)) erreur(); 
			else i = 0;
			if (total == 0) etat = NB0; 
			else etat = CONNEXION ;  
			break;
		case CONNEXION : 
			if (!(data >> uid1 >> uid2)) erreur();
			else ++i;
			if (i == total) etat = NB0; 
			if (!ma_ville.addConnexion(uid1,uid2)) etat = NB0;
			break;
		default :
			erreur();
	}	
}

void erreur(){
	cout << "Format error" << endl;
}

size_t get_nombre_noeud(){
	return get_collection_size();
}

Ville::Ville(bool etat_initial) : valid(etat_initial) {}

bool Ville::addLogement(unsigned int uid, double x, double y, unsigned nbp){
	bool valid = add_noeud(uid, x, y, nbp, LOGEMENT_);
	if (!valid) ma_ville.set_valid(false);
	return valid;
}
		
bool Ville::addTransport(unsigned int uid, double x, double y, unsigned nbp){
	bool valid = add_noeud(uid, x, y, nbp, TRANSPORT_);
	if (!valid) ma_ville.set_valid(false);
	return valid;
}

bool Ville::addProduction(unsigned int uid, double x, double y, unsigned nbp){
	bool valid = add_noeud(uid, x, y, nbp, PRODUCTION_);
	if (!valid) ma_ville.set_valid(false);
	return valid;
}

bool Ville::addConnexion(unsigned int a, unsigned int b){ 
	bool valid = connect(a, b);
	if (!valid){
		ma_ville.set_valid(false);
		reset_list();
	}
	else insertConnexion(a, b);
	return valid;
}

void Ville::insertConnexion(unsigned int a, unsigned int b){
	ville.push_back({a, b});
}

void create_noeud(double x, double y, Type type){
	double xm(xf_to_xm(x));
	double ym(yf_to_ym(y));
	
	unsigned int id = new_uid();
	
	bool valid = creation_condition(id, xm, ym, min_capacity);
	
	if (valid) make_noeud(id, xm, ym, min_capacity, type);
}

void link_operation(unsigned int id1, unsigned int id2){
	bool creation = true;
	for (size_t i(0); i < ma_ville.get_ville().size(); i++){
		if (ma_ville.get_ville()[i][0] == id1 && ma_ville.get_ville()[i][1] == id2){
			Noeud* const& n1 = uid_to_pointer(id1);
			Noeud* const& n2 = uid_to_pointer(id2);
			n1->remove_connexion(n2);
			n2->remove_connexion(n1);
			ma_ville.delete_connexion(i);
			creation = false;
			break;
		}
		
		if (ma_ville.get_ville()[i][0] == id2 and ma_ville.get_ville()[i][1] == id1){
			Noeud* const& n1 = uid_to_pointer(id1);
			Noeud* const& n2 = uid_to_pointer(id2);
			n1->remove_connexion(n2);
			n2->remove_connexion(n1);
			ma_ville.delete_connexion(i);
			creation = false;
			break;
		}
	}
	
	if (creation){
		bool link_valid = link_creation_condition(id1, id2);
		if (link_valid) ma_ville.insertConnexion(id1, id2);
	}
}

bool creation_condition(unsigned int uid, double x, double y, unsigned int nbp){
	if (uid >= reserved){
		cout << error::reserved_uid();
		return false;
	}
	
	if (!verifier_noeud_collision(uid, x, y, nbp, false)) return false;
	
	string error_type = "adding node";
	
	for (auto connection : ma_ville.get_ville()){            
		if (!verify_one_link(x, y, sqrt(nbp), connection[0], 
		                     connection[1], error_type)){
			 return false;
		}
	}
	
	return true;
}

bool Ville::get_valid() const{
	return valid;
}

void Ville::set_valid(bool v){
	valid = v;
}

void Ville::enlever(){
	ville.pop_back();
}

vector<vector<unsigned int>> Ville::get_ville() const{
	return ville;
}

void dessiner_ville(){
	for (auto connexion : ma_ville.get_ville()){
		dessiner_liens(uid_to_pointer(connexion[0]), uid_to_pointer(connexion[1]));
	}
	dessiner_noeuds();
}

void dessiner_shortest_path(unsigned int uid){ 
	dessiner_shortest_path_to(uid_to_pointer(uid),PRODUCTION_);
	dessiner_shortest_path_to(uid_to_pointer(uid),TRANSPORT_);
}

void dessiner_selected(unsigned int a){
	uid_to_pointer(a)->dessiner(RED);
}

bool verifier_liens(unsigned int uid, double x, double y, double rayon, string error){
	for (auto link : ma_ville.get_ville()){
		if (link[0] != uid && link[1] != uid){
			if (!verify_one_link(x, y, rayon, link[0], link[1], error)){
				return false;
			}
		}
	}
	return true;
}

void modifier_rayon(unsigned int uid, Vecteur p1, Vecteur p2){
	Cercle cercle(uid_to_pointer(uid)->get_position());
	double x(cercle.centre.x);
	double y(cercle.centre.y);
	
	double x1(xf_to_xm(p1.x));
	double x2(xf_to_xm(p2.x));
	double y1(yf_to_ym(p1.y));
	double y2(yf_to_ym(p2.y));
	
	string error_type = "changing size";
	
	double rayon_debut(distance({x, y}, {x1, y1}));
	double rayon_fin(distance({x, y}, {x2, y2}));
	double new_rayon(rayon_fin - rayon_debut);
	double rayon_tot(new_rayon + cercle.rayon);
	
	if (rayon_tot < sqrt(min_capacity)){
		rayon_tot = sqrt(min_capacity);
	}
	
	if (rayon_tot > sqrt(max_capacity)){
		rayon_tot = sqrt(max_capacity);
	}

	if (verifier_modification(uid, x, y, rayon_tot, error_type)
	    and verifier_liens(uid, x, y, rayon_tot, error_type)){
		uid_to_pointer(uid)->set_rayon(rayon_tot); 
	} 
}

void modifier_centre(unsigned int uid, Vecteur p){
	double x(xf_to_xm(p.x));
	double y(yf_to_ym(p.y));
	double rayon(uid_to_pointer(uid)->get_position().rayon);
	
	string error_type = "moving node";
	
	if (verifier_modification(uid, x, y, rayon, error_type) 
		 and verifier_liens(uid, x, y, rayon, error_type)
		 and verifier_liens_geometrie(uid, x, y)){
		uid_to_pointer(uid)->set_centre({x, y});
	}
}

bool noeud_selectionne(Vecteur p, long int& id){
	double xm(xf_to_xm(p.x));
	double ym(yf_to_ym(p.y));
	
	return in_noeud({xm, ym}, id);
}

void delete_indiv(unsigned int uid){
	for (size_t i(0); i < ma_ville.get_ville().size(); ++i){
		unsigned int uid1(ma_ville.get_ville()[i][0]);
		unsigned int uid2(ma_ville.get_ville()[i][1]);
		
		if (uid == uid1 or uid == uid2){
			ma_ville.delete_connexion(i);
			--i;
		}
	}
	delete_one_noeud(uid);
}

void Ville::delete_connexion(size_t index){
	size_t last(ville.size() - 1);
	swap(ville[index], ville[last]);
	ville.pop_back();
}

void reset_list(){
	while (!ma_ville.get_ville().empty()){
		ma_ville.enlever();
	}
}

void reset_ville(){
	delete_noeuds();
	reset_list();
	ma_ville.set_valid(true);
}

void ecriture(char * nom_fichier){
	ofstream fsortie;    
	fsortie.open(nom_fichier);  
	  
	if (!fsortie.fail()){
		fsortie << "#this file is proudly sponsored by KFC's spicy ckicken wings";      
		fsortie << endl << endl << endl;
	
		fsortie << "#nb housing" << endl << nombre_noeud(LOGEMENT_) << endl;
		fsortie << "#housing:" << endl;
		ecriture_noeud(LOGEMENT_ , fsortie);
	
		fsortie << "#nb transport" << endl << nombre_noeud(TRANSPORT_) << endl;
		fsortie << "#transport:" << endl;
		ecriture_noeud(TRANSPORT_, fsortie);
	
		fsortie << "#nb production" << endl << nombre_noeud(PRODUCTION_) << endl;
		fsortie << "#production:" << endl; 
		ecriture_noeud(PRODUCTION_, fsortie);
	
		fsortie << "#nb link" << endl;
		fsortie << ma_ville.get_ville().size() << endl;
		for (auto element : ma_ville.get_ville()){
			fsortie << "\t" << element[0] << " " << element[1] << endl;
		}
	}
	else cout << "Impossible d'ouvrir le fichier: " << nom_fichier << endl;
	fsortie.close(); 	
}

double enj(){
	int somme_l(somme_nbp(LOGEMENT_));
	int somme_t(somme_nbp(TRANSPORT_));
	int somme_p(somme_nbp(PRODUCTION_));
	double denominateur(somme_l + somme_t + somme_p);
	double enj(0.);
	if (denominateur != 0.) enj = (somme_l - somme_t - somme_p)/denominateur;
	return enj;
}
 
double ci_total(){
	double ci_total(0.);
	for (size_t i(0); i < (ma_ville.get_ville()).size(); ++i){
		ci_total = ci_total+ ci(ma_ville.get_ville()[i][0], 
		                        ma_ville.get_ville()[i][1]); 
	}
	return ci_total;
}
