/**
* Ce fichier contient une description des attributs et
* methodes de la classe Personnage.
* \fichier  Personnage.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
#include "Affichable.h"
#include <iostream>

class Personnage : public Affichable
{
public:
	
	explicit Personnage(const string& nom, const string parution) 
	: nom_(nom),
	  parution_(parution) {}

	virtual ~Personnage() = default;

	void changerCouleur(ostream& out, string couleur) const override
	{
		out << couleur;
	}
	
	void afficher(ostream& out) const override
	{
		out << "Nom:       " << nom_      << endl
			<< "Parution:  " << parution_ << endl;
	};

	const string& obtenirNom() const
	{
		return nom_;
	}

	const string& obtenirJeu() const
	{
		return parution_;
	}

private:

	string nom_;
	string parution_;
};