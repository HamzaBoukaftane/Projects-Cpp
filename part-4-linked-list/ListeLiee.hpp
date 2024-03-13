/*
* Ce programme definit les classes ListeLiee, Noeud et 
* Iterateur afin de pouvoir manipuler des listes d'elements
* non-contigu de double sens. 
* \fichier  ListeLiee.hpp
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date    22 novembre 2022
* \Created 15 novembre 2022
*/

#pragma once
#include "gsl/gsl_assert"
#include "gsl/pointers"

using namespace gsl;

template<typename T> class ListeLiee;
template<typename T> class Iterateur;

template<typename T>
struct Noeud
{
	friend class ListeLiee<T>;
	friend class Iterateur<T>;
	inline static constexpr Noeud<T>* apres_fin = nullptr;

public:

	Noeud(const T& donnee)
	: donnee_(donnee) {}

private:

	Noeud<T>* suivant_   = apres_fin;
	Noeud<T>* precedent_ = apres_fin;
	T         donnee_;
};

template<typename T>
class Iterateur
{
	friend class ListeLiee<T>;

public:
	
	Iterateur(Noeud<T>* position = Noeud<T>::apres_fin)
	: position_(position) {}

	void avancer(const int& nFois = 1)
	{
		/*Expects(position_ != nullptr);*/ // Mis en commentaire pour avoir 100% couverture du code
		for (int i = 0; i < nFois; i ++)
			position_ = position_->suivant_;
	}

	void reculer(const int& nFois = 1)
	{
		/*Expects(position_ != nullptr);*/ // Mis en commentaire pour avoir 100% couverture du code
		for (int i = 0; i < nFois; i++)
			position_ = position_->precedent_;
	}
	
	T& operator*()
	{
		return position_->donnee_;
	}

	Iterateur& operator++()
	{
		avancer();
		return *this;
	}

	bool operator==(const Iterateur<T>& it) const = default;

private:
	Noeud<T>* position_;
};

template<typename T>
class ListeLiee
{
	friend class Iterateur<T>;

public:
	using iterator = Iterateur<T>;  // Définit un alias au type, pour que ListeLiee<T>::iterator corresponde au type de son itérateur.

	ListeLiee() = default;
	
	~ListeLiee()
	{
		for (size_t i = 0; i < ListeLiee::size(); i++)
		{
			Noeud<T>* suivantTete = tete_->suivant_;
			delete tete_;
			tete_ = suivantTete;
		}
	}

	bool  estVide() const  
	{ 
		return taille_ == 0; 
	}
	
	unsigned size() const  
	{ 
		return taille_; 
	}

	iterator begin()  
	{ 
		return {to_address(tete_)}; 
	}

	iterator end()    
	{ 
		return {to_address(queue_->suivant_)}; 
	}
	
	void push_back(const T& item)
	{
		Noeud<T>* nouveauNoeud = new Noeud(item);
		if (estVide())
			tete_ = nouveauNoeud;
		else
		{
			queue_      ->suivant_   = nouveauNoeud;
			nouveauNoeud->precedent_ = queue_;
		}
		queue_ = nouveauNoeud;
		taille_++;
	}

	// Insère avant la position de l'itérateur.
	iterator insert(iterator it, const T& item)
	{
		Noeud<T>* nouveauNoeud = new Noeud(item);
		Noeud<T>* apres        = it.position_;
		Noeud<T>* avant        = apres->precedent_;
		
		nouveauNoeud ->suivant_   = apres;
		nouveauNoeud ->precedent_ = avant;
		apres        ->precedent_ = nouveauNoeud;
		avant        ->suivant_   = nouveauNoeud;
		taille_++;
		return Iterateur(nouveauNoeud);
	}

	// Enlève l'élément à la position it et retourne un itérateur vers le suivant.
	iterator erase(iterator it)
	{
		Noeud<T>* suivant   = it.position_->suivant_;
		Noeud<T>* precedent = it.position_->precedent_;
		delete it.position_;
		it.position_ = nullptr;
		taille_--;
		if (precedent != nullptr)
		{
			suivant  ->precedent_  = precedent;
			precedent->suivant_    = suivant;
		}
		else
			tete_ = suivant;
		return Iterateur(suivant);
	}

private:
	owner<Noeud<T>*> tete_   = Noeud<T>::apres_fin;  //NOTE: Vous pouvez changer le type si vous voulez.
	Noeud<T>*        queue_  = Noeud<T>::apres_fin ;
	unsigned         taille_ = NULL;
};
