#pragma once
#include <cstddef>
#include <iostream>
#include "gsl/span"
#include "cppitertools/range.hpp"
#include "memory"
using namespace std;
using namespace gsl;
using namespace iter;

template <typename T>
class Liste
{
public:
	
	Liste() = default;	
	
#pragma region Constructeur de copie profonde (Inspire des note de cours de INF1015)
	Liste& operator=(const Liste& autre)
	{
		if (this != &autre)
		{
			elements_.release();
			nElements_ = autre.nElements_;
			capacite_  = autre.capacite_;
			elements_  = make_unique<shared_ptr<T>[]>(nElements_);
			
			for (auto i : range(nElements_))
				elements_[i] = autre.elements_[i];
		}
		return *this;
	}

	Liste(Liste&& autre) noexcept
	{
		*this = move(autre);
	}
#pragma endregion
	

	shared_ptr<T>& operator[](const int& indice) const
	{
		return elements_[indice];
	}
	
	span<const shared_ptr<T>> enSpan() const
	{
		return span(elements_.get(), nElements_);
	}

	void ajouter(const shared_ptr<T> tPtr)
	{
		if (nElements_ == capacite_)
			changerCapacite(max(size_t(1), capacite_ * 2));
		elements_[nElements_++] = tPtr;
	}
	
	shared_ptr<T> chercher(const function<bool(shared_ptr<T>)>& critere) 
	{
		for (auto&& ptrElement : enSpan())
		{
			if (critere(ptrElement))
				return ptrElement;
		}
		return nullptr;
	}

	size_t obtenirNombreElement() const
	{
		return nElements_;
	}

	size_t obtenirCapacite() const
	{
		return capacite_;
	}

private:
	
	size_t nElements_ = 0, capacite_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_ = nullptr;
	
	//On la garde privee, car nous voulons que le changement de capacite soit implicite a la classe
	void changerCapacite(std::size_t nouvelleCapacite)
	{
		auto nouveauxElements = make_unique<shared_ptr<T>[]>(nouvelleCapacite);
		for (auto i : range(nElements_))
			nouveauxElements[i] = elements_[i];
		elements_ = move(nouveauxElements);
		capacite_ = nouvelleCapacite;
	}
};


