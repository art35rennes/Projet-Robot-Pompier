#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "string.h"


sGrille* Import();
void CleanScreen();
void AfficherGrille(sGrille* Grille);
void Inserer_element(pRep *Rep, int x, int y);

#endif