#include "Jeu.hpp"
#include "Liste.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"

template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion

#pragma region "Fonction utile pour la struct Concepteur"

shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, const string nom)
{
	auto concepteurNom = [&](auto c) {return c->nom == nom;};
	for (auto&& jeu : listeJeux.enSpan()) 
	{
		shared_ptr<Concepteur> concepteur = jeu->chercher(concepteurNom);
		bool estExistant = (concepteur != nullptr);
		if (estExistant)
			return concepteur;
	}
	return nullptr;
}

shared_ptr<Concepteur> lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur;
	concepteur.nom            = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays           = lireString(fichier);
	bool estExistant          = (trouverConcepteur(listeJeux, concepteur.nom) != nullptr);
	if (estExistant)
		return trouverConcepteur(listeJeux, concepteur.nom);
	return make_shared<Concepteur>(concepteur);
}
#pragma endregion

#pragma region "Fonction utile pour la classe Jeu"

shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu;
	jeu.titre                 = lireString(fichier);
	jeu.anneeSortie           = int(lireUintTailleVariable(fichier));
	jeu.developpeur           = lireString(fichier);
	
	shared_ptr<Jeu> ptrJeu    = make_shared<Jeu>(jeu);
	size_t          nElements = lireUintTailleVariable(fichier);
	
	for ([[maybe_unused]] size_t n : range(nElements))
	{
		ptrJeu->concepteurs.ajouter(lireConcepteur(fichier, listeJeux));
	}
	return ptrJeu; 
}

Liste<Jeu> creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	size_t     nElements = lireUintTailleVariable(fichier);
	Liste<Jeu> listeJeux;
	
	for ([[maybe_unused]] size_t n : iter::range(nElements))
	{
		listeJeux.ajouter(lireJeu(fichier,listeJeux)); 
	}
	return listeJeux;
}
#pragma endregion

#pragma region "Surcharge operateur<< pour affichage"

ostream& operator<<(ostream& sortant, const Concepteur& concepteur)
{
	sortant << concepteur.nom            << ", "
			<< concepteur.anneeNaissance << ", "
		    << concepteur.pays;
	return sortant;
}

ostream& operator<<(ostream& sortant, const Liste<Concepteur>& liste)
{
	int numero = 1;

	for (auto concepteur : liste.enSpan())
	{
		sortant << "\n\t" << numero << "- " << *concepteur;
		numero++;
	}
	return sortant;
}

ostream& operator<<(ostream& sortant, const Jeu& jeu)
{
	sortant << "\t" << "Titre du jeu: "    << jeu.titre       << endl
		    << "\t" << "Annee de sortie: " << jeu.anneeSortie << endl
		    << "\t" << "Developpeur: "     << jeu.developpeur << endl
		    << "\t" << "Concepteur(s): "   << jeu.concepteurs << endl;	                                   
	return sortant;
}

ostream& operator<<(ostream& sortant, const Liste<Jeu>& liste)
{
	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	int                          numero = 1;
	
	sortant << "Affichage de la liste de Jeux" << endl 
		    << ligneSeparation                 << endl;
	
	for (auto jeu : liste.enSpan())
	{
		sortant << "\t\t Jeu #" << numero          << endl
				                << *jeu            << endl
		                        << ligneSeparation << endl;
		numero++;
	}
	
	sortant << "\nTest de l'ajout dans la liste\n"
		    << "\nSpecification de la liste de jeux:"                    << endl
		    << "1- Nombre d'elements = " << liste.obtenirNombreElement() << endl
	        << "2- Capacite          = " << liste.obtenirCapacite()      << endl;

	return sortant;
}

ostream& operator<<(ostream& sortant, const Liste<int>& liste)
{
	sortant << "Affichage de la liste de nombre : ";
	int index = 0;
	for (auto nombre : liste.enSpan())
	{
		bool estNul = (index == 0);
		if (estNul)
			sortant << *nombre;
		else
			sortant << ", " << *nombre;
		index++;
	}
	sortant << endl;
	return sortant;
}
#pragma endregion

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	//En executant le main, tout les tests effectue sont affiche
#pragma region Bibliothèque du cours
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
#pragma endregion
	//int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.
#pragma region Test de la classe generique Liste avec type int
	Liste<int> test1;
	
	shared_ptr ptr1 = make_shared<int>(0);
	shared_ptr ptr2 = make_shared<int>(1);
	shared_ptr ptr3 = make_shared<int>(2);
	shared_ptr ptr4 = make_shared<int>(3);
	shared_ptr ptr5 = make_shared<int>(4);
	
	test1.ajouter(ptr1);
	test1.ajouter(ptr2);
	test1.ajouter(ptr3);
	test1.ajouter(ptr4);
	test1.ajouter(ptr5);
	
	cout << test1 << endl;
#pragma endregion

	Liste<Jeu> lj = creerListeJeux("jeux.bin");
	
	cout << "Affichage du deuxieme concepteur du jeux Secret of Mana: " 
		 << lj[2]->concepteurs[1]->nom << endl <<endl;

	cout << lj << endl;           // Affiche aussi la capacite et le nombre element de la liste
	ofstream("sortie.txt") << lj; // Creation fichier txt de sortie avec la liste

	cout << "\nTest de la surcharge de l'operateur []\n" << endl;
	shared_ptr<Jeu>        secretOfMana = lj[2];
	shared_ptr<Concepteur> hiromichi    = secretOfMana->concepteurs[1];
	cout << "lj[2]->titre donne:               " << secretOfMana->titre << endl
	     << "lj[2]->concepteurs[1]->nom donne: " << hiromichi->nom      << endl;

	cout << "\nTest de la fonction lambda\n" << endl;
	auto kitase = [&](shared_ptr<Concepteur> concepteur) {return concepteur->nom == "Yoshinori Kitase"; };
	
	for (int i = 0; i < 2; i++)
		cout << "Addresse " << i + 1 << " de Kitase est " << lj[i]->chercher(kitase) 
		     << " et date de naissance est " << lj[i]->chercher(kitase)->anneeNaissance << endl;

	Jeu copie = *lj[2];
	cout << "\nTest de copie de jeu \n\nOriginal:\n\n"          << *lj[2]                << endl
		 << "\n\nCopie du jeu:\n\n "                            << copie                 << endl
		 << "Addresse de l'original:                          " << lj[2]                 << endl
		 << "Addresse de la copie:                            " << &copie                << endl
		 << "Addresse du concepteur Tanaka l'original:        " << lj[2]->concepteurs[1] << endl
		 << "Addresse du concepteur Tanaka de la copie:       " << copie.concepteurs[1]  << endl;
	
	copie.concepteurs[1] = lj[7]->concepteurs[0];
	cout << "\nCopie du jeu ou l'on remplace Tanaka par Alex:\n\n " << copie                 << endl;
	cout << "\nOriginal:\n\n"                                       << *lj[2]                << endl;
	cout << "Addresse du concepteur Alexej l'original:      "       << lj[7]->concepteurs[0] << endl
		 << "Addresse du concepteur Alexej de la copie:     "       << copie.concepteurs[1]  << endl
		 << "Addresse du concepteur Tanaka de l'original:   "       << lj[2]->concepteurs[1] << endl;
}
