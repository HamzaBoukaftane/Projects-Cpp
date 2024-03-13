/*
* Ce programme permet de manipuler une liste d'elements non 
* contigu (ListeLiee<Heros>) a travers l'utilisation de noeud et 
* d'iterateur. De plus, ce programme explore l'utilisation d'un
* conteneur (map).
* \fichier  main.cpp
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date    22 novembre 2022
* \Created 15 novembre 2022
*/

#include "Personnage.hpp"
#include "Heros.hpp"
#include "Vilain.hpp"
#include "VilainHeros.hpp"
#include "lireVectorDuFichier.hpp"
#include "lectureBinaire.hpp"
#include "ListeLiee.hpp"
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "cppitertools/range.hpp"
#include "bibliotheque_cours.hpp"
using namespace std;
using namespace iter;

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


// versReferenceAffichable permet d'avoir une référence à l'affichable, que le paramètre soit un unique_ptr ou une référence.  On aurait pu faire un template plus générique pour const/non const, et même utiliser un concept, mais ce n'est pas nécessaire pour que ça fonctionne.
// lireFichier aurait aussi pu retourner un vector de unique_ptr pour ne pas avoir besoin de versReferenceAffichable pour supporter à la fois les références et les unique_ptr.
const Affichable& versReferenceAffichable(const Affichable& p) { return p; }
template <typename T>
const Affichable& versReferenceAffichable(const unique_ptr<T>& p) { return *p; }


// Trait de separation
static const string trait =
	"═════════════════════════════════════════════════════════════════════════";

static const string separateurDeListe = string(60, '=');

static const string separateurElement = string(60, '*');

// On ne demande pas particulièrement la surcharge de << dans ce TD.
template <typename T>
void afficherAffichables(const vector<T>& affichables)
{
	static const string separateurElements = "\033[33m" + trait + "\033[0m\n";
	for (auto&& a : affichables) {
		cout << separateurElements;
		auto& x = versReferenceAffichable(a);
		x.changerCouleur(cout, 0);
		x.afficher(cout);
	}
}

// Permet d'avoir une référence non-const à un objet temporaire.
template <typename T> T& temporaireModifiable(T&& objet) { return objet; }

template <typename T>
vector<T> lireVectorDuFichier(const string& nomFichier)
{
	return lireVectorDuFichier<T>(temporaireModifiable(ouvrirFichierBinaire(nomFichier)));
}

template <typename T>
ListeLiee<T>::iterator trouverParNom(ListeLiee<T>& liste, const string& nom)
{
	//? Quand on accède à un membre dans un type qui n'est pas encore connu (ici T n'est pas connu donc ListeLiee<T> n'est pas connu non plus) lors de l'analyse syntaxique du programme, dans le cas général il ne peut pas savoir si ce membre est un type.  Pour cette raison on doit ajouter le mot typename devant pour lui dire.
	typename ListeLiee<T>::iterator fin = liste.end();
	for (typename ListeLiee<T>::iterator pos = liste.begin(); pos != fin; pos.avancer()) {
		if ((*pos).getNom() == nom)
			return pos;
	}
	return fin;
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
	
	static const string separateurSections = "\033[95m" + trait + "\033[0m\n";

	vector<Heros> heros = lireVectorDuFichier<Heros>("heros.bin");
	vector<Vilain> vilains = lireVectorDuFichier<Vilain>("vilains.bin");
	vector<unique_ptr<Personnage>> peronnages;  

	cout << "Section 1: Transfert des heros de vector vers ListeLiee" << endl
		 << "\n\t\t Affichage de la ListeLiee \n"                     << endl;
	
	ListeLiee<Heros> lH;
	for (auto element : heros)
		lH.push_back(element);

	for (auto element : lH)
	{
		element.afficher(cout);
		cout << separateurDeListe << endl;
	}
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
	     << separateurSections                                         << endl;

	cout << "Section 2: Creation iterateur a la position Alucard" << endl
		 << "\n\t Affichage du contenu de l'iterateur \n"         << endl;
	
	Iterateur it = trouverParNom(lH, "Alucard");
	(*it).afficher(cout);
	cout << separateurSections << endl;

	cout << "Section 3: Deplacement 2 fois de l'iterateur pour trouver Aya Brea " << endl
		 << "\n\t Affichage du contenu de l'iterateur apres deplacement\n"        << endl;
	
	it.avancer(2);
	(*it).afficher(cout);
	cout << separateurSections << endl;

	cout << "Section 4: Ajout du Heros Khabib avant Aya Brea" << endl
		 << "\n\t\t Affichage de la ListeLiee modifiee\n"     << endl;
	
	it = lH.insert(it, Heros("Khabib Nurmagomedov", "UFC GOAT", "Connor McGreggor"));
	for (auto element : lH)
	{
		element.afficher(cout);
		cout << separateurDeListe << endl;
	}
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
	     << separateurSections                                         << endl;

	cout << "Section 5a): Reculer iterateur jusqu'a Mario"                 << endl
		 << "\n\t Affichage du contenu de l'iterateur apres deplacement\n" << endl;
	
	it.reculer(4);
	(*it).afficher(cout);
	
	cout << "\n" << separateurElement                                  << endl
		 << "\nSection 5b):  Effacer Mario"                            << endl
		 << "\n\t Affichage du contenu de l'iterateur suivant Mario\n" << endl;
	
	it = lH.erase(it);
	(*it).afficher(cout);
	
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
		 << separateurSections                                         << endl;

	cout << "Section 6: Effacer le premier element de la ListeLiee"                 << endl
		 << "\n\t Affichage du contenu de l'iterateur suivant apres l'effacement\n" << endl;
	
	it = lH.erase(lH.begin());
	(*it).afficher(cout);
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
		 << separateurSections                                         << endl;

	cout << "Section 7: Affichage par intermediaire d'un iterateur" << endl
		 << "\n\t\t Affichage de la ListeLiee modifiee\n"           << endl;
	
	for (Iterateur position = lH.begin(); position != lH.end(); position.avancer())
	{
		(*position).afficher(cout);
		cout << separateurDeListe << endl;
	}
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
		 << separateurSections << endl;
	
	cout << "Section 8: Affichage par simple boucle for sur intervalle" << endl
		 << "\n\t\t Affichage de la ListeLiee modifiee\n" << endl;
	
	for (auto element : lH)
	{
		element.afficher(cout);
		cout << separateurDeListe << endl;
	}
	cout << "\nLa taille de la liste est de " << lH.size() << " Heros" << endl
		 << separateurSections                                         << endl;
	
	cout << "Section 9: Utilisation d'un conteneur (map) pour avoir les heros en ordre alphabetique" << endl
		<< "\n\t\t Affichage des valeurs de map de Heros\n" << endl;
	
	map<string,Heros> conteneurHeros;
	
	for (auto element : lH)
		conteneurHeros.insert({element.getNom(), element});

	for (auto [cle, h] : conteneurHeros)
	{
		h.afficher(cout);
		cout <<separateurDeListe << endl;
	}
	cout << separateurSections << endl;

	cout << "Section 10: Chercher le heros Khabib dans la map" << endl
		<< "\n\t\t Affichage de la valeur trouvee\n" << endl;
	
	conteneurHeros["Khabib Nurmagomedov"].afficher(cout);
	cout << separateurSections << endl;

	/*
	* Reponse aux questions de la section 2:
	*********************************************************************** 
	*2.1 Complexite de la recherche conteneurHeros["Khabib Nurmagomedov"]
	* 
	* Le conteneurHeros est de type map, ainsi c'est une structure ordonnee
	* dont chaque element de type Heros est associe a une cle de type string
	* qui est le nom du Heros en question. Par consequent, lors de la recherche
	* du Heros, tant qu'il y a plus d'un element dans la map consideree, on test
	* en O(1) quelle moitie de la map on doit continuer a considerer. Enfin,
	* de ce raisonnement nous pouvons deduire que la complexite algorithmique
	* d'une telle recherche est de O (log n).
	* 
	* Preuve:
	* Prenons notre map en exemple
	* Chaque cle pointe vers un element de type Heros
	* Cle:   Al - Ay - K - L - N - Ra - Ro - T
	* Index: 0  - 1  - 2 - 3 - 4 - 5  - 6  - 7
	* 
	* Premiere verification: 8*0.5 = 4
	* Cle:   Al - Ay - K - L - N - Ra - Ro - T
	* Index: 0  - 1  - 2 - 3 - 4 - 5  - 6  - 7
	*                      ^
	* Est-ce que K > L (Faux) ou K < L (Vrai)
	*
	* Deuxieme verification: 4*0.5 = 2
	* Cle:   Al - Ay - K - L
	* Index: 0  - 1  - 2 - 3
	*             ^
	* Est-ce que K > Ay (Vrai) ou K < Ay (Faux)
	* 
	* Troisieme verification: 2*0.5 = 1
	* Cle:   K - L
	* Index: 2 - 3
	*            ^
	* Est-ce que K > L (Faux) ou K < L (Vrai)
	* 
	* Quatrieme etape on la cle de la valeur recherchee
	* Cle:   K
	* Index: 2
	* 
	* Ainsi, on peut dire 8*0.5^3 = 1 => n*0.5^k = 1 => En generalisant, log n!
	* 
	* *********************************************************************** 
	* 2.2 Rapidite de rechercher par nom entre ListeLiee et conteneur utilise
	* 
	* Evaluons d'abord la complexite algorithmique de la fonction trouverParNom
	* typename ListeLiee<T>::iterator fin = liste.end(); => O(1)
	* for (typename ListeLiee<T>::iterator pos = liste.begin(); pos != fin; pos.avancer()) => O(n)
	* {
	*	if ((*pos).getNom() == nom) => O(1)
	* 		return pos; => O(1)
	* }
	* return fin; => O(1)
	* 
	* 1 + n(1 + 1) + 1 => 1 + n + n + 1 => n + n => 2n => n
	* Ainsi, pour la fonction trouverParNom => O(n)
	* 
	* Dans cette optique, pour rechercher un element par nom avec la fonction 
	* TrouverParNom, il faut parcourir la ListeLiee depuis le debut d'ou la
	* complexite lineaire demontrer precedemment, alors que pour effectuer 
	* la recherche par nom dans un conteneur de type map, il suffit de 
	* fragmenter consecutivement la map jusqu'a l'obtention de l'element 
	* recherche (O(log n)). Par consequent, le conteneur de type map permet une 
	* recherche plus rapide,car cette technique effectue moins d'operations que 
	* la fonction TrouverParNom utilisant une ListeLiee pour donner un resultat.
	* 
	* *********************************************************************** 
	*/
}
