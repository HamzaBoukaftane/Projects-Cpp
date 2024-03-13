/**
* Ce fichier contient une description des attributs et
* methodes de la classe Vilain.
* \fichier  Vilain.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
#include "Personnage.h"

class Vilain : virtual public Personnage
{
public:
	
	explicit Vilain(const string& nom, const string& parution, const string& objectif)
	: Personnage(nom, parution),
	  objectif_(objectif) {};

	virtual ~Vilain()=default;

	void afficher(ostream& out) const override
	{
		Personnage::changerCouleur(out, Couleur().rouge);
		Personnage::afficher(out);
		out << "Objectif:  " << objectif_ << endl;
	};

	const string& obtenirObjectif() const
	{
		return objectif_;
	}

private:

	string objectif_;
};