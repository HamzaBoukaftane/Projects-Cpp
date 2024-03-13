/**
* Programme qui lit, traite et affiche un liste de jeux 
* comprennant differents elements a partir du fichier jeux.bin.
* \fichier  main.cpp
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   2 october 2022
* Created 25 september 2022
*/
#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.
#include "Developpeur.hpp"
#include "ListeDeveloppeurs.hpp"

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

gsl::span<Jeu*> spanListeJeux(const ListeJeux& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}

gsl::span<Concepteur*> spanListeConcepteurs(const ListeConcepteurs& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}
#pragma endregion

#pragma region Fonctions principales TD2
Concepteur* trouverConcepteur(string& nomConcepteur, const ListeJeux& listeJeux)
{
	for (auto ptrJeu : spanListeJeux(listeJeux))
	{
		for (auto ptrConcepteur : spanListeConcepteurs(ptrJeu->concepteurs))
		{
			bool estPareil = (ptrConcepteur->nom == nomConcepteur);
			if (estPareil)
				return ptrConcepteur;
		}
	}
	return nullptr;
}

Concepteur* lireConcepteur(istream& fichier, const ListeJeux& listeJeux)
{
	Concepteur concepteur     = {}; 
	
	concepteur.nom            = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays           = lireString(fichier);
	
	bool estExistant          = (trouverConcepteur(concepteur.nom, listeJeux) != nullptr);
	
	if (!(estExistant))
	{
		//cout << "Creation du concepteur" << concepteur.nom << endl; // Uncomment si besoin debogage
		return new Concepteur(concepteur);
	}
	return trouverConcepteur(concepteur.nom, listeJeux);
}

void changerTailleTableau(size_t capacite, ListeJeux& listeJeux)
{
	Jeu** nouvelleListe = new Jeu* [capacite];
	int   index         = 0;

	for (auto jeu : spanListeJeux(listeJeux))
	{
		nouvelleListe[index] = jeu;
		index++;
	}
	delete[] listeJeux.elements;
	listeJeux.elements = nouvelleListe;
}

void ajouterJeu(Jeu* ptrJeu, ListeJeux& listeJeux)
{
	bool estPetit         = (listeJeux.capacite < listeJeux.nElements + 1);
	bool estCapaciteNulle = (listeJeux.capacite == 0);
	
	if (estPetit)
	{
		if (estCapaciteNulle)
			listeJeux.capacite = 1;
		changerTailleTableau((listeJeux.capacite) * 2, listeJeux);
		listeJeux.capacite *= 2;
	}
	listeJeux.elements[listeJeux.nElements ++] = ptrJeu;
}

void enleverJeu(const Jeu* ptrJeu, ListeJeux& liste)
{
	for (auto*& jeu : spanListeJeux(liste))
	{
		bool estPareil = (jeu->titre == ptrJeu->titre);

		if (estPareil)
		{
			if (liste.nElements > 1)
			{
				jeu = liste.elements[liste.nElements -1];
			}
			liste.nElements --;
		}
	}
}

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu                   = {}; 
	
	jeu.titre                 = lireString(fichier);
	jeu.anneeSortie           = int(lireUintTailleVariable(fichier));
	jeu.developpeur           = lireString(fichier);
	jeu.concepteurs.nElements = lireUintTailleVariable(fichier);
	jeu.concepteurs.capacite  = jeu.concepteurs.nElements;
	jeu.concepteurs.elements  = new Concepteur* [jeu.concepteurs.nElements];
	
	Jeu* ptrJeu               = new Jeu(jeu);
	//cout << "Lecture du jeu: " << jeu.titre << endl;  // Uncomment si besoin debogage
	for ([[maybe_unused]] size_t i : iter::range(jeu.concepteurs.nElements)) 
	{
		ptrJeu->concepteurs.elements[i] = lireConcepteur(fichier, listeJeux);
		ajouterJeu(ptrJeu, ptrJeu->concepteurs.elements[i]->jeuxConcus);
	}
	return ptrJeu;
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	size_t    nElements = lireUintTailleVariable(fichier);
	ListeJeux listeJeux = {};
	
	listeJeux.elements  = new Jeu * [nElements];
	listeJeux.capacite  = nElements;

	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		listeJeux.elements[n] = lireJeu(fichier, listeJeux); 
		listeJeux.nElements++;
	}
	return listeJeux; 
}

void detruireConcepteur(Concepteur* ptrConcepteur)
{
	delete[] ptrConcepteur->jeuxConcus.elements;
	ptrConcepteur->jeuxConcus.elements = nullptr;
	//cout << "Detruire concepteur: " << ptrConcepteur->nom << endl; // Uncomment si besoin debogage
	delete ptrConcepteur;
	ptrConcepteur = nullptr;
}

bool estEncoreParticipant(Concepteur* ptrConcepteur)
{
	bool estNulle = ((ptrConcepteur->jeuxConcus.nElements) == 0);
	
	if (estNulle)
			return false;

	return true;
}

void detruireJeu(Jeu* ptrJeu)
{
	for (auto concepteur : spanListeConcepteurs(ptrJeu->concepteurs))
	{
		enleverJeu(ptrJeu, concepteur->jeuxConcus);
		if (!(estEncoreParticipant(concepteur)))
		{
			detruireConcepteur(concepteur);
		}
	}
	//cout << "Detruire jeu: " << ptrJeu->titre << endl; // Uncomment si besoin debogage 
	delete[] ptrJeu->concepteurs.elements;
	ptrJeu->concepteurs.elements = nullptr;
	delete ptrJeu;
	ptrJeu = nullptr;
}

void detruireTotaliteListeJeux(ListeJeux& liste)
{
	for (auto ptrJeu : spanListeJeux(liste))
	{
		detruireJeu(ptrJeu);
		liste.nElements--;
	}
	delete[] liste.elements;
}

void afficherConcepteur(const Concepteur& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays << endl;
}

void afficherInfoJeu(const Jeu& jeu)
{	
	cout << "\t" << "Titre du jeu: " << jeu.titre << endl
		 << "\t" << "Annee de sortie: " << jeu.anneeSortie << endl
		 << "\t" << "Developpeur: " << jeu.developpeur << endl
		 << "\t" << "Concepteur(s): " << endl;
	for (auto concepteur : spanListeConcepteurs(jeu.concepteurs))
		afficherConcepteur(*concepteur);
}

void afficherListeJeux(const ListeJeux& liste, const string separation)
{
	cout << "\t" << "Collection de jeux" << endl;
	
	int compteur = 1;
	for (auto jeu : spanListeJeux(liste))
	{
		cout << "Jeu # " << compteur << ": " << endl;
		afficherInfoJeu(*jeu);
		cout << separation << endl;
		compteur++;
	}
}
#pragma endregion

#pragma region classe Developpeur: Constructeur, Destructeur et methodes
Developpeur::Developpeur(const string& nom)
{
	paireNomJeux_.first = nom;
	paireNomJeux_.second = {};
}

Developpeur::~Developpeur()
{
	delete[] paireNomJeux_.second.elements;
	paireNomJeux_.second.elements = nullptr;
}

const string& Developpeur::obtenirNom()const
{
	return paireNomJeux_.first;
}

size_t& Developpeur::obtenirNombreJeux(const ListeJeux& listeJeux)
{
	for (auto ptrJeu : spanListeJeux(listeJeux))
	{
		bool estMemeDeveloppeur = (ptrJeu->developpeur == paireNomJeux_.first);

		if (estMemeDeveloppeur)
			paireNomJeux_.second.nElements++;
	}
	return paireNomJeux_.second.nElements;
}

void Developpeur::modifierListeJeux(const ListeJeux& listeJeux)
{
	paireNomJeux_.second.capacite = obtenirNombreJeux(listeJeux);
	paireNomJeux_.second.elements = new Jeu * [paireNomJeux_.second.nElements];
	int index = 0;
	for (auto ptrJeu : spanListeJeux(listeJeux))
	{
		bool estMemeDeveloppeur = (ptrJeu->developpeur == paireNomJeux_.first);
		if (estMemeDeveloppeur)
		{
			paireNomJeux_.second.elements[index] = ptrJeu;
			index++;
		}
	}
}

void Developpeur::afficherJeuxDeveloppes() const
{
	cout << "Nom du developpeur: " << paireNomJeux_.first << endl;
	cout << "Liste des jeux developpes: " << endl;
	for (auto ptrJeu : spanListeJeux(paireNomJeux_.second))
		cout << ptrJeu->titre << endl;
	cout << "*************************************************" << endl;
}
#pragma endregion

#pragma region classe ListeDeveloppeurs: Constructeur, Destructeur et methode
ListeDeveloppeurs::ListeDeveloppeurs()
{
	capacite_  = 0;
	nElements_ = 0;
	elements_ = nullptr;
}

ListeDeveloppeurs::~ListeDeveloppeurs()
{
	delete[] elements_;
	elements_ = nullptr;
}

span<Developpeur*> ListeDeveloppeurs::spanListeDeveloppeur()
{
	return span(elements_, nElements_);
}

bool ListeDeveloppeurs::trouver(Developpeur& developpeur)
{
	for (auto ptrDev : spanListeDeveloppeur())
	{
		bool estPareil = (ptrDev->obtenirNom() == developpeur.obtenirNom());
		if (estPareil)
			return true;
	}
	return false;
}

void ListeDeveloppeurs::ajouter(Developpeur& developpeur)
{
	bool estPetit = (capacite_ < nElements_ + 1);
	bool estCapaciteNulle = (capacite_ == 0);
	bool estDansListe = (trouver(developpeur));
	if (!(estDansListe))
	{
		if (estPetit)
		{
			if (estCapaciteNulle)
				capacite_ = 1;
			Developpeur** nouvelleListe = new Developpeur * [capacite_ * 2];
			int index = 0;
			for (auto ptrDeveloppeur : spanListeDeveloppeur())
			{
				nouvelleListe[index] = ptrDeveloppeur;
				index++;
			}
			delete[] elements_;
			elements_ = nullptr;
			elements_ = nouvelleListe;
			capacite_ *= 2;
		}
		elements_[nElements_++] = &developpeur;
	}
}

void ListeDeveloppeurs::retirer(Developpeur& developpeur)
{
	for (auto*& ptrDeveloppeur : spanListeDeveloppeur())
	{
		bool estPareil = (ptrDeveloppeur->obtenirNom() == developpeur.obtenirNom());
		if (estPareil)
		{
			if (nElements_ > 1)
			{
				ptrDeveloppeur = elements_[nElements_ - 1];
			}
			nElements_--;
		}
	}
}

void ListeDeveloppeurs :: afficher()
{
	cout << "\t" << "Liste de Developpeur" << endl << endl;
	for (auto ptrDeveloppeur : spanListeDeveloppeur())
		ptrDeveloppeur->afficherJeuxDeveloppes();
}

#pragma endregion

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi(); 
	#pragma endregion
	//int* fuite = new int;  //  Uncomment pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux listeJeux = creerListeJeux("jeux.bin");
	ListeDeveloppeurs listeDev;

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	afficherInfoJeu(*(listeJeux.elements[0])); // Affichage du jeu Chrono Trigger (Premier de la liste)
	cout << ligneSeparation << endl;
	
	afficherListeJeux(listeJeux, ligneSeparation);
	
	#pragma region Initialisatiom instances Developpeur + ajout a ListeDeveloppeur 
	Developpeur dev1{ listeJeux.elements[0]->developpeur };
	dev1.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev1);

	Developpeur dev2{ listeJeux.elements[1]->developpeur };
	dev2.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev2);

	Developpeur dev3{ listeJeux.elements[2]->developpeur }; // Dev 1,2 et 3 Square
	dev3.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev3);

	Developpeur dev4{ listeJeux.elements[3]->developpeur };
	dev4.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev4);

	Developpeur dev5{ listeJeux.elements[4]->developpeur };
	dev5.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev5);
	
	Developpeur dev6{ listeJeux.elements[5]->developpeur }; // Dev 4, 5, 6 Nintendo
	dev6.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev6);

	Developpeur dev7{ listeJeux.elements[6]->developpeur }; // Dev 7 GameFreak
	dev7.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev7);

	Developpeur dev8{ listeJeux.elements[7]->developpeur }; // Dev 8 Mojang
	dev8.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev8);

	Developpeur dev9{ listeJeux.elements[8]->developpeur }; // Dev 9 Blizzard
	dev9.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev9);

	Developpeur dev10{ listeJeux.elements[9]->developpeur }; // Dev 10, 13 Konami
	dev10.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev10);

	Developpeur dev11{ listeJeux.elements[10]->developpeur };
	dev11.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev11);

	Developpeur dev12{ listeJeux.elements[11]->developpeur }; // Dev 11, 12 Maxis
	dev12.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev12);

	Developpeur dev13{ listeJeux.elements[12]->developpeur }; // Dev 10, 13 Konami
	dev13.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev13);

	Developpeur dev14{ listeJeux.elements[16]->developpeur }; // Dev 14 FromSoftware (index 16 pour tester)
	dev14.modifierListeJeux(listeJeux);
	listeDev.ajouter(dev14);
	#pragma endregion
	listeDev.afficher();

	
	listeDev.retirer(dev1);
	cout << "#### Retirer Square ####" << endl << endl;
	listeDev.afficher();
	cout << "### Retirer FromSoftware ####" << endl << endl;
	listeDev.retirer(dev14);
	listeDev.afficher();
	cout << "### Retirer Mojang ###" << endl << endl;
	listeDev.retirer(dev8);
	listeDev.afficher();

	detruireTotaliteListeJeux(listeJeux);
}
