/**
* Ce programme permet d'effectuer la lecture et l'affichage
* de differents types de Personnages contenu dans des fichiers
* bin. Il permet de les placer dans des vecteurs et d'afficher
* la liste tout en modifiant la couleur et la composition 
*  d'affichage en fonction du type de personnages.
* \fichier  main.cpp
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#include "lectureBinaire.hpp"
#include <fstream>
#include <sstream>
#include <cassert>
#include "bibliotheque_cours.hpp"
#include "VilainHeros.h"

ifstream ouvrirFichierBinaire(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	return fichier;
}

void testsPourCouvertureLectureBinaire()
{
	istringstream iss("\xA0\x12\xA1\x21\x43\xA2\x98\xBA\xDC\xFE");
	assert(lireUintTailleVariable(iss) == 0x12);
	assert(lireUintTailleVariable(iss) == 0x4321);
	assert(lireUintTailleVariable(iss) == 0xFEDCBA98);
}

shared_ptr<Heros> lireHero(istream& fichier)
{
	string         nom      = lireString(fichier);
	string         jeu      = lireString(fichier);
	string         ennemi   = lireString(fichier);
	size_t         nAlllies = lireUintTailleVariable(fichier);
	vector<string> allies   = {};
	
	for (int i = 0; i < nAlllies; i++ )
	{
		allies.push_back(lireString(fichier));
	}
	return make_shared<Heros>(Heros(nom, jeu, ennemi, allies));
}

shared_ptr<Vilain> lireVilain(istream& fichier)
{
	string nom      = lireString(fichier);
	string jeu      = lireString(fichier);
	string objectif = lireString(fichier);
	return make_shared<Vilain> (Vilain(nom, jeu, objectif));
}

vector<shared_ptr<Heros>> creerListeHeros(istream& fichier)
{
	size_t nHeros = lireUintTailleVariable(fichier);
	vector<shared_ptr<Heros>> listeHeros = {};

	for (size_t i = 0; i < nHeros; i++)
	{
		listeHeros.push_back(lireHero(fichier));
	}
	return listeHeros;
}

vector<shared_ptr<Vilain>> creerListeVilains(istream& fichier)
{
	size_t nVilains = lireUintTailleVariable(fichier);
	vector<shared_ptr<Vilain>> listeVilains = {};

	for (size_t i = 0; i < nVilains; i++)
	{
		listeVilains.push_back(lireVilain(fichier));
	}
	return listeVilains;
}

// Cette fonction generique fonctionne seulement avec des listes sources
// de classe: Personnage, Heros, Vilain et VilainHeros.
template<typename T>
void chargerElements(vector<shared_ptr<Personnage>>& listeCible, const vector<shared_ptr<T>>& listeSource)
{
	for (auto&& element: listeSource)
		listeCible.push_back(element);
}

// Cette fonction generique fonctionne seulement pour les classes:
// Personnage, Heros, Vilain et VilainHeros.
template<typename T>
void afficherListe(ostream& out, const vector<shared_ptr<T>>& liste, const string& trait)
{
	size_t indexDernier = liste.size() - 1;
	for (auto&& element : liste)
	{
		bool estDernier = (element->obtenirNom() == liste[indexDernier]->obtenirNom());
		element->afficher(out);
		out << trait << endl;
		if (estDernier)
			element->changerCouleur(out, Couleur().blanc);
	}
}

int main()
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
	#pragma endregion
	
	testsPourCouvertureLectureBinaire();

	// Trait de separation
	static const string trait =
		"═════════════════════════════════════════════════════════════════════════";

	// Ouverture des fichiers binaires
	ifstream fichierHeros = ouvrirFichierBinaire("heros.bin");
	ifstream fichierVilains = ouvrirFichierBinaire("vilains.bin");

	// Creation de la liste de Heros
	vector<shared_ptr<Heros>>  listeHeros(creerListeHeros(fichierHeros));
	cout << "\t\tAffichage de la liste de Heros\n\n";
	afficherListe(cout, listeHeros, trait);

	// Creation de la liste de Vilains
	vector<shared_ptr<Vilain>> listeVilains(creerListeVilains(fichierVilains));
	cout << "\n\t\tAffichage de la liste de Vilains\n\n";
	afficherListe(cout, listeVilains, trait);
	
	// Creation de la liste de personnage
	vector<shared_ptr<Personnage>> listePersonnages;
	chargerElements(listePersonnages, listeHeros);
	chargerElements(listePersonnages, listeVilains);
	cout << "\n\t\tAffichage de la liste de Personnages\n\n";
	afficherListe(cout, listePersonnages, trait);

	// Creation d'un VilainHeros avec le Hero Crono et le Vilains Kekfa
	VilainHeros v(listeHeros[0], listeVilains[2]);
	cout << "\n\t\tAffichage du VilainHeros\n\n";
	v.afficher(cout);
	v.changerCouleur(cout, Couleur().blanc);

	// Ajout du VilainHeros dans la liste de Personnage
	listePersonnages.push_back(make_shared<VilainHeros>(v));
	cout << "\n\t\tAffichage de la liste de Personnages modifiee\n\n";
	afficherListe(cout, listePersonnages, trait);
	
	// Test pour le plaisir!!!
	ofstream file("sortie.txt");
	afficherListe(file, listePersonnages, trait);
}
