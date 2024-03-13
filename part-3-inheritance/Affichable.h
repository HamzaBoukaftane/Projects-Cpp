/**
* Ce fichier contient une description des attributs et
* methodes de la classe abstraite Affichable.
* \fichier  Vilain.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
#include "Couleur.h"

class Affichable
{
public:
	virtual void afficher(ostream& out) const = 0;
	virtual void changerCouleur(ostream& out, string colueur) const = 0;
};