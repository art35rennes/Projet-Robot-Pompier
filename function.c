#include "function.h"

sGrille* Import(){
	sGrille* Grille = (sGrille*)malloc(sizeof(sGrille));
	FILE *carte;
	int i;

	carte = fopen("../Carte/Appart.txt", "r");
	fscanf(carte,"%d:%d", &Grille->largeur, &Grille->hauteur);
	printf("Largeur: %d Hauteur: %d\n",Grille->largeur, Grille->hauteur );
	Grille->ligne = malloc(sizeof(char *)*Grille->hauteur);
	

	for (i = 0; i < Grille->hauteur; ++i){
		Grille->ligne[i] = (char*)malloc(sizeof(char)*(Grille->largeur+2));
	}
	char temp;
	fscanf(carte, "%c", &temp);

	for (i = 0; i < Grille->hauteur; ++i){
		fgets(Grille->ligne[i], Grille->largeur+2, carte);
	}

	return Grille;

}

void CleanScreen()
{
	printf("\x1B[2J");
}

void AfficherGrille(sGrille* Grille){
	printf("%d %d\n",Grille->hauteur, Grille->largeur );
	printf("Ma Grille:\n");
	int i;

	for (i = 0; i < Grille->hauteur; i++){
		printf("%s", Grille->ligne[i]);
	}
}

pRep *Init_Rep()
{
	pRep *liste = (pRep*)malloc(sizeof(pRep));
	liste->Taille = 0;
	liste->Debut = NULL;
	liste->Fin = NULL;
	return liste;
}
