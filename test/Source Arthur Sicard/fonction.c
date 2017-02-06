#include "ia.h"

/*-----------------------------------------------------
              Fonction d'initialisation
-----------------------------------------------------*/

void Inserer_element(pRep *Rep, int x, int y, sGrille* Grille)//permet d'inserer des elements dans la liste chainee et de les initialises
{
	if( Rep->Taille == 0 || (Rep->Taille != 0 && (Rep->Fin->x != x || Rep->Fin->y != y))){//on verifie pour eviter la duplication successive de case
		//printf("\tINSERTION X: %d Y: %d\n", x, y);
		sElement* newElement = (sElement*)malloc(sizeof(sElement));
		newElement->x = x;
		newElement->y = y;
		newElement->fire = On_Fire(Grille->ligne[x][y]);
		newElement->pSuivant = NULL;
		newElement->pPrecedent = NULL;
		
		if(Rep->Taille == 0){//si la liste est vide
			Rep->Debut = newElement;
			Rep->Fin = newElement;
		}
		else{//sinon on insert en fin
			Rep->Fin->pSuivant  = newElement;
			newElement->pPrecedent = Rep->Fin;
			Rep->Fin = newElement;
		}
		Rep->Taille++;
	}
}

pRep *Init_Rep()//initialisation du repertoire
{
	pRep *liste = (pRep*)malloc(sizeof(pRep));
	liste->Taille = 0;
	liste->Debut = NULL;
	liste->Fin = NULL;
	return liste;
}

sRobot Init_Robot(sGrille* Grille)//initialisation du robot
{
	sRobot Robot;
	int i, j;

	Robot.equiper = 0;//port ou non de l'extincteur
	Robot.vie = VIE;//nb de pts de vie
	Robot.Show = 'R';//permet de stocker plus simplement l'etat du robot P ou R
	Robot.FeuTrouver = 0;//indique si le feu a ete trouver sur ça trajectoire
	for(i = 0; i < Grille->hauteur; i++){
		for(j = 0; j < Grille->largeur; j++){
			if (Grille->ligne[i][j] == 'D')//on initialise ça position de depart
			{
				Grille->ligne[i][j] = 'R';
				Robot.x = i;
				Robot.y = j;
				Robot.orientation = N;
			}
			if(Grille->ligne[i][j] == 'E')//on recupere les coord de l'extincteur
			{
				Robot.Ex = i;
				Robot.Ey = j;
			}
		}
	}
	return Robot;
}

sGrille* Import(char* map){//importation de la map
	sGrille* Grille = (sGrille*)malloc(sizeof(sGrille));
	FILE *carte;
	int i;

	carte = fopen(map, "r");
	if(carte != NULL){//on verifie qu'un fichier a put etre importer
		fscanf(carte,"%d:%d", &Grille->largeur, &Grille->hauteur);
		//printf("Largeur: %d Hauteur: %d\n",Grille->largeur, Grille->hauteur );
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
	else{
		printf("Fichier ilisible !\n");
		exit(0);
	}

}

sIntGrille* GenerationTabInt(int hauteur, int largeur, sGrille* Grille)//generation d'un tableau de int a partir de la carte pour le pathfinding
{
	sIntGrille* IntGrille = (sIntGrille*) malloc(sizeof(sIntGrille));
	int i, j;

	IntGrille->ligne = malloc(sizeof(int *)*hauteur);
	IntGrille->hauteur = hauteur;
	IntGrille->largeur = largeur;

	for (i = 0; i < hauteur; ++i)
		IntGrille->ligne[i] = (int*)malloc(sizeof(int)*(largeur));
	
	for (i = 0; i < hauteur; i++){
		for (j = 0; j < largeur; j++){
			switch(Grille->ligne[i][j]){
				case 'x':
					IntGrille->ligne[i][j] = MUR;
					break;
				case ' ':
					IntGrille->ligne[i][j] = SPACE;
					break;
				case '1':
					IntGrille->ligne[i][j] = SPACE;
					break;
				case '2':
					IntGrille->ligne[i][j] = SPACE;
					break;
				case '3':
					IntGrille->ligne[i][j] = SPACE;
					break;
				case 'E':
					IntGrille->ligne[i][j] = EXTINCTEUR;
					break;
				case 'R':
					IntGrille->ligne[i][j] = ROBOT;
					break;
				default:
					IntGrille->ligne[i][j] = -5;
					break;
			}
		}
	}

	return IntGrille;
}

/*--------------------------------------------
             Fonction d'affichage
--------------------------------------------*/

void CleanScreen()
{
	printf("\x1B[2J");
}

void AfficherGrille(sGrille* Grille, sRobot* Robot, int nbPas){
	int i, j;
	
	printf("\n");
	for (i = 0; i < Grille->hauteur; i++){
		for (j = 0; j < Grille->largeur; j++){
			switch(Grille->ligne[i][j]){//en fonction du caractere a afficher on l'affiche d'une certaine couleur
				case 'x':
					printf("\e[0;36m%2c", Grille->ligne[i][j]);
					break;
				case ' ':
					printf("\e[0;30m%2c", Grille->ligne[i][j]);
					break;
				case 'E':
					printf("\e[0;34m%2c", Grille->ligne[i][j]);
					break;
				case '#':
					printf("\e[1;37m%2c", Grille->ligne[i][j]);
					break;
				case '1':
					printf("\e[1;33m%2c", Grille->ligne[i][j]);
					break;
				case '2':
					printf("\e[1;31m%2c", Grille->ligne[i][j]);
					break;
				case '3':
					printf("\e[0;31m%2c", Grille->ligne[i][j]);
					break;
				case 'R':
					printf("\e[1;35m%2c", Grille->ligne[i][j]);
					break;
				case 'P':
					printf("\e[0;31m%2c", Grille->ligne[i][j]);
					break;
				case '.':
					printf("\e[0;33m%2c", Grille->ligne[i][j]);
					break;
			}
		}
		printf("\e[0m\n");
	}
	//suite de for et print permettant d'afficher le compteur de vie et nb de pas
	for(i = 0; i < (int)((Grille->largeur-33)/2 ); i++)
		printf("  ");
	for(i = 0; i < 22; i++)
		printf("\e[1;30m---\e[0m");
	printf("\n");
	for(i = 0; i < (int)((Grille->largeur-33)/2 ); i++)
		printf("  ");
	printf("\e[1;30m|\e[0m Vie: ");
	for(i = 0; i < 10; i++){
		if(i < Robot->vie)
			printf("\e[0;31m<3 \e[0m");
		else
			printf("   ");
	}
	printf(" \e[1;30m|\e[0m Nombre de Mouvement:\e[0;32m%4d \e[1;30m|\e[0m\n", nbPas);
	for(i = 0; i < (int)((Grille->largeur-33)/2 ); i++)
		printf("  ");
	for(i = 0; i < 22; i++)
		printf("\e[1;30m---\e[0m");
	printf("\n");
}

void AfficherIntGrille(sIntGrille* IntGrille)//fonction de debug
{
	
	printf("\nMa IntGrille:\n");
	int i, j;

	for (i = 0; i < IntGrille->hauteur; i++){
		printf("%2d ", i);
		for (j = 0; j < IntGrille->largeur; j++){
			switch(IntGrille->ligne[i][j]){
				case SPACE:
					printf("   ");
					break;
				case MUR:
					printf(" x ");
					break;
				case FEU:
					printf(" F ");
					break;
				default:
					printf("%3d", IntGrille->ligne[i][j]);
					break;
			}
		}
		printf("\n");
	}
	printf("  ");
	for(j = 0; j < IntGrille->largeur; j++)
		printf(" %2d", j);
	printf("\n\n");
}

void AfficherChemin(pRep* Chemin)//fonction de debug
{
	sElement *cElement;
	cElement = Chemin->Fin;
	int i;
	printf("Chemin parcourue:\n");
	for(i = 0; i < Chemin->Taille; i++){
		printf("Case n°%d:\n\tX: %2d, Y: %2d\n",i+1,  cElement->x, cElement->y);
		cElement = cElement->pPrecedent;
	}
}

/*--------------------------------
             Divers
--------------------------------*/

void SupprimerPartChaine(pRep* Chemin, int nb)//permet de supprimer tout les elements de la liste chaine a partir d'un certain rang
{
	sElement* Element;
	Element = Chemin->Debut;

	int i;
	for(i = 0; i < nb; i++)
		Element = Element->pSuivant;
	Chemin->Fin = Element;

	for(i = 0; i < Chemin->Taille-nb; i++){
		Chemin->Taille--;
		Element = Element->pSuivant;
		free(Element->pPrecedent);
	}
}