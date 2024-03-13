#pragma once
#include <cstddef>
#include "Developpeur.hpp"
#include "gsl/span"

using namespace gsl;

class ListeDeveloppeurs
{
public:

	ListeDeveloppeurs();
	~ListeDeveloppeurs();
	span<Developpeur*> spanListeDeveloppeur();
	bool trouver(Developpeur&);
	void ajouter(Developpeur&);
	void retirer(Developpeur&);
	void afficher();
	
private:

	size_t nElements_ = NULL; 
	size_t capacite_  = NULL;
	Developpeur** elements_ = nullptr;
};
