#pragma once
# include <string>
# include "ListeJeux.hpp"

using namespace std;

class Developpeur
{
public:

	Developpeur(const string&);
	~Developpeur();
	const string& obtenirNom() const;
	size_t& obtenirNombreJeux(const ListeJeux&);
	void modifierListeJeux(const ListeJeux&);
	void afficherJeuxDeveloppes() const;

private:

	pair<string, ListeJeux> paireNomJeux_;
	
};
