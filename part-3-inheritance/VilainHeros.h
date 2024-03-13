/**
* Ce fichier contient une description des attributs et
* methodes de la classe VilainHeros.
* \fichier  VilainHeros.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
#include "Vilain.h"
#include "Heros.h"

class VilainHeros: public Heros, public Vilain
{
public:
	
	explicit VilainHeros(const shared_ptr<Heros>& h, const shared_ptr <Vilain>& v)
	:Personnage(
		v->obtenirNom() + "-" + h->obtenirNom(),
		v->obtenirJeu() + "-" + h->obtenirJeu()),
	 Heros(*h),
	 Vilain(*v),
	 missionSpeciale_(
		 v->obtenirObjectif()  + 
		 " dans le monde de "  + 
		 h->obtenirJeu()) {}

	virtual ~VilainHeros() = default;

	void afficher(ostream& out) const override
	{
		Personnage::changerCouleur(out, Couleur().vert);
		Heros::afficherSansCouleur(out);
		out << "Mission speciale: " << missionSpeciale_ << endl;
	};

private:

	string missionSpeciale_;
};