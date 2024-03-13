#pragma once
#include <string>
#include "Concepteur.hpp"
#include "Liste.hpp"

class Jeu
{
public:
	string            titre;
	string            developpeur;
	int               anneeSortie;
	Liste<Concepteur> concepteurs;
	
	Jeu()
	: titre("n/a"),
	  developpeur("n/a"),
	  anneeSortie(0)
	{
	}

	Jeu(const Jeu& autre)
	{
		titre       = autre.titre;
		developpeur = autre.developpeur;
		anneeSortie = autre.anneeSortie;
		concepteurs = autre.concepteurs;
	}

	shared_ptr<Concepteur> chercher(const function<bool(shared_ptr<Concepteur>)>& critere)
	{
		return concepteurs.chercher(critere);
	}
};
