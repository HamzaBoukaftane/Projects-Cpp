/**
* Ce fichier contient une description des attributs et
* methodes de la classe Heros.
* \fichier  Heros.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
# include "Personnage.h"
# include <vector>

class Heros : virtual public Personnage
{
public:
	
	explicit Heros(const string& nom, const string& parution, const string& ennemi, const vector<string>& allies)
	: Personnage(nom, parution),
	  ennemi_(ennemi),
	  allies_(allies) {}

	virtual ~Heros()=default;

	void afficher(ostream& out) const override
	{
		Personnage::changerCouleur(out, Couleur().bleu);
		Personnage::afficher(out);
		out << "Ennemi:    " << ennemi_ << endl
			<< "Allies:    "            << endl;
		for (auto allie : allies_)
			out << "\t   " << allie << endl;
	};

protected:
	
	void afficherSansCouleur(ostream& out) const
	{
		Personnage::afficher(out);
		out << "Ennemi:    " << ennemi_ << endl
			<< "Allies:    " << endl;
		for (auto allie : allies_)
			out << "\t   " << allie << endl;
	};

private:

	string         ennemi_;
	vector<string> allies_;
};