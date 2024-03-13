/**
* Ce fichier contient une description des attributs
* de la struct Couleur.
* \fichier  Couleur.h
* \auteurs Hamza Boukaftane and Rayan-Atman Aït-Aoudia
* \date   8 novembre 2022
* \Created 31 octobre 2022
*/

#pragma once
#include <string>
using namespace std;

struct Couleur 
{
	string bleu  = "\033[94m";
	string rouge = "\033[91m";
	string vert  = "\033[92m";
	string blanc = "\033[0m";
};