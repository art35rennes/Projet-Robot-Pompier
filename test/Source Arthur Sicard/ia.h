#ifndef IA_H
#define IA_H
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <unistd.h>

#define N 0
#define E 90
#define S 180
#define W 270

#define SPACE -1
#define MUR -2
#define FEU -3
#define ROBOT -4
#define EXTINCTEUR 0

#define VIE 10

#define SLEEP 70000

typedef struct Element
{
	struct Element *pPrecedent;
	int x, y;
	struct Element *pSuivant;;
	short int fire;
}sElement;

typedef struct sRepertoire
{
	sElement *Debut;
	sElement *Fin;
	int Taille;
}pRep;

typedef struct Grille
{
	char** ligne;
	int hauteur;
	int largeur;
}sGrille;

typedef struct IntGrille
{
	int** ligne;
	int hauteur;
	int largeur;
	int maxWeight;
}sIntGrille;

typedef struct Robot
{
	int x, y;
	short int orientation;
	short int equiper;
	int Ex, Ey;
	short int vie;
	char Show;
	int FeuTrouver;
}sRobot;


int On_Fire(char Chr);
int Detection_Mur(sGrille *Grille, sRobot *Robot, int Direction);
sRobot Init_Robot(sGrille *Grille);
void Recup_Extincteur(pRep* rHistorique);
void Suivre_Chemin(sGrille* Grille, sRobot *Robot, pRep *Chemin);

sGrille* Import(char*);
void CleanScreen();
void AfficherGrille(sGrille* Grille, sRobot* Robot, int nbPas);
void Inserer_element(pRep *Rep, int x, int y, sGrille* Grille);
pRep *Init_Rep();

sIntGrille* GenerationTabInt(int hauteur, int largeur, sGrille* Grille);
void AfficherIntGrille(sIntGrille* IntGrille);
void Propagation(sIntGrille* IntGrille,  int Fx, int Fy);
void Calcul_Chemin(sIntGrille* IntGrille, pRep* Chemin,int x, int y, sGrille* Grille, int mode, sRobot* Robot);
void AfficherChemin(pRep* Chemin);

int PasserSurFeu(pRep* Chemin);
void Extinction(sGrille* Grille);
void ParcoursAppartement(sGrille* Grille, pRep* Chemin, sRobot* Robot);
void SupprimerPartChaine(pRep* Chemin, int nb);

#endif