#include "ia.h"

int On_Fire(char Chr) //Fonction qui indique si la case comporte du feu ou non
{
	if(Chr >= '0' && Chr <= '3')
		return 1;
	else
		return 0;
}

void Suivre_Chemin(sGrille* Grille, sRobot *Robot, pRep *Chemin) //Fonction permettant au robot de ce deplacer en suivant un chemin precalculer
{
	sElement *cSuivant;
	cSuivant = Chemin->Debut;//premiere caase ou va ce rendre le robot
	char old = ' ';//variable contenent le caractere qui va etre ecraser par le deplacement du robot
	int nbPas = 0;//compteur de pas

	do{//on deplace la robot en suivant l'ordre des cases enregistrer
		nbPas++;
		
		Grille->ligne[Robot->x][Robot->y] = old;//on remplace l'ancienne position du robot par le caractere qui si trouvais
		old = Grille->ligne[cSuivant->x][cSuivant->y];
		
		if(old == 'E'){//Si le robot est ou est passer sur l'extincteur on le nomme P et non R
			Robot->Show = 'P';
			old = 35;
		}	

		if(On_Fire(Grille->ligne[cSuivant->x][cSuivant->y])){//Gestion de la perte de point de vie
			Robot->vie -= Grille->ligne[cSuivant->x][cSuivant->y]-48;
		}

		Grille->ligne[cSuivant->x][cSuivant->y] = Robot->Show;//On inscrit la nouvelle position du robot, on le deplace graphiquement
		
		Robot->x = cSuivant->x;//on met a jour les coord du robot
		Robot->y = cSuivant->y;
		
		CleanScreen();//on reset le terminal
		AfficherGrille(Grille, Robot, nbPas);//on affiche la grille
		
		
		if(Robot->vie <= 0)//si le robot est mort on quitte
			break;
		usleep(SLEEP);

		cSuivant = cSuivant->pSuivant;
	}while(cSuivant != NULL);

	if(Robot->vie > 0 && Robot->FeuTrouver)//Gestion de l'extinction du feu
		Extinction(Grille);

	CleanScreen();
	AfficherGrille(Grille, Robot, nbPas);//On affiche la grille
}

void Propagation(sIntGrille* IntGrille, int Fx, int Fy)//fonction du pathfinding
{
	int find = 0;
	int weight = 0;
	int i, j;

	while(!find){
		for(i = 0; i < IntGrille->hauteur-1; i++){
			for(j = 0; j < IntGrille->largeur-1; j++){
				if(IntGrille->ligne[i][j] == weight){
					if(i+1 < IntGrille->hauteur){
						if(IntGrille->ligne[i+1][j] == SPACE)
							IntGrille->ligne[i+1][j] = weight+1;
					}
					if(i-1 > 0){
						if(IntGrille->ligne[i-1][j] == SPACE)
							IntGrille->ligne[i-1][j] = weight+1;
					}
					if(i+1 < IntGrille->largeur){
						if(IntGrille->ligne[i][j+1] == SPACE)
							IntGrille->ligne[i][j+1] = weight+1;
					}
					if(j-1 > 0){
						if(IntGrille->ligne[i][j-1] == SPACE)
							IntGrille->ligne[i][j-1] = weight+1;
					}
					if((i-1 == Fx || i+1 == Fx) && (j-1 == Fy || j+1 == Fy))
						find = 1;
				}
			}
		}
		weight++;
		IntGrille->maxWeight = weight;
	}
}

void Calcul_Chemin(sIntGrille* IntGrille, pRep* Chemin,int x, int y, sGrille* Grille, int mode, sRobot* Robot)//x, y coordonnee de depart
{	//Fontion qui grace au pathfinding calcul un chemin pour atteindre l'extincteur ou un autre objectif
	int i;
	for(i = IntGrille->maxWeight; i >= 0; i--){
		//printf("I: %2d, x: %2d, y: %2d, w
		if(x+1 < IntGrille->hauteur){
			if(IntGrille->ligne[x+1][y] == i){
				x++;
			}
		}
		if(x-1 > 0){
			if(IntGrille->ligne[x-1][y] == i){
				x--;
			}
		}
		if(y+1 < IntGrille->largeur+-1){
			if(IntGrille->ligne[x][y+1] == i){
				y++;
			}
		}

		if(y-1 > 0){
			if(IntGrille->ligne[x][y-1] == i){
				y--;
			}
		}
		Inserer_element(Chemin, x, y, Grille);
	}

	if(mode){//en mode 1 calcul chemin trouve le cheminj vers l'extincteur puis vers le feu, 
			//en mode 0 la fonction trouve le chemin entre 2 points via un pathfinding prealablement calculer entre ces points
		int dist = PasserSurFeu(Chemin);//Si en allant a l'extincteur on a trouver le feu on y retourne
			if( dist != 0){
				sElement* Element;
				Element = Chemin->Fin;
				Robot->FeuTrouver = 1;
				for (i = 0; i < dist; i++){
					Inserer_element(Chemin, Element->x, Element->y, Grille);
					Element = Element->pPrecedent;
				}
			}
			else{//sinon ou parcourt l'appartement
				int pos;
				sElement* Element;
				Element = Chemin->Debut;
				
				ParcoursAppartement(Grille, Chemin, Robot);//on genere un parcourt de TOUT l'appartement 
				
				for(pos = 0; pos < Chemin->Taille; pos++){//on cherche dans le parcourt generer si on croise le feu
					if(Element->fire){//si oui on va supprimer toute les cases superflue
						Robot->FeuTrouver = 1;
						break;
					}
					else
						Element = Element->pSuivant;
				}
				if(pos > 0)
					SupprimerPartChaine(Chemin, pos);
			}
	}
	
}


int PasserSurFeu(pRep* Chemin)//fonction qui parcourt le chemin et regarde a qu'elle moment ou croise pour la premiere fois le feu
{
	sElement* Element;
	Element = Chemin->Fin;
	int i;
	for(i = 1; i <= Chemin->Taille; i++){
		if(!Element->fire)
			Element = Element->pPrecedent;
		else
			return i;
	}
	return 0;
}

void Extinction(sGrille* Grille)//extinction du feu
{
	int i, j;
	for(i = 0; i < Grille->hauteur; i++){
		for(j = 0; j < Grille->largeur; j++){
			if(Grille->ligne[i][j] == '1' || Grille->ligne[i][j] == '2' || Grille->ligne[i][j] == '3'){
				Grille->ligne[i][j] = '.';
				//printf("Exctinction d'un feu %d\n",Grille->ligne[i][j]-48);
			}
		}
	}
	//printf("EXTINCTION DES FEUX !\n");
}

void EditTabInt(int Destx, int Desty, int Ox, int Oy, sRobot* Robot, sIntGrille* IntGrille)//permet d'editer le tab de int pour l'utiliser pour ce rendre
{//vers un autre point que l'extincteur
	IntGrille->ligne[Robot->Ex][Robot->Ey] = SPACE;
	IntGrille->ligne[Robot->x][Robot->y] = SPACE;
	IntGrille->ligne[Destx][Desty] = 0;
	IntGrille->ligne[Ox][Oy] = -4;
}

void ParcoursAppartement(sGrille* Grille, pRep* Chemin, sRobot* Robot)//genere le parcourt dans l'appartement si l'on a pas trouver le feu
{
	sIntGrille *IntGrille = GenerationTabInt(Grille->hauteur, Grille->largeur, Grille);

	int i, j, k, l;
	i = 2;
	//on se rend dans la case de coord 2:2 pour ensuite pouvoir comencer le parcourt
	EditTabInt(2, 2, Chemin->Fin->x, Chemin->Fin->y,Robot, IntGrille);
	Propagation(IntGrille, Chemin->Fin->x, Chemin->Fin->y);
	IntGrille->ligne[Chemin->Fin->x][Chemin->Fin->y] = IntGrille->maxWeight+1;
	Calcul_Chemin(IntGrille, Chemin, Chemin->Fin->x, Chemin->Fin->y, Grille, 0, Robot);
	/*trajectoire horizontale
	x xxx xxx xxx
	xxx xxx xxx x....

	trajectoire vertical
	x
	x
	x
	...
	*/

	for(k = 0; k < (int)(Grille->hauteur/10) && i < Grille->hauteur-1; k++){
		//Horizontale droite, on parcourt la carte dans ça largeur
		for(j = 3; j < Grille->largeur-2; j +=3){//pathfinding toute les 3 case
			if(Grille->ligne[i][j] != 'x'){//on verifie que le n'on va pas ce deplacer dans un mur
				//printf("X: %d Y: %d\n",i, j );
				IntGrille = GenerationTabInt(Grille->hauteur, Grille->largeur, Grille);
				EditTabInt(i, j, Chemin->Fin->x, Chemin->Fin->y,Robot, IntGrille);
				Propagation(IntGrille, Chemin->Fin->x, Chemin->Fin->y);
				IntGrille->ligne[Chemin->Fin->x][Chemin->Fin->y] = IntGrille->maxWeight+1;
				Calcul_Chemin(IntGrille, Chemin, Chemin->Fin->x, Chemin->Fin->y, Grille, 0, Robot);
			}
		}
		//Verticale bas
		if(j > Grille->largeur-2)//si l'on sort du tableau on y retourne pour eviter les erreurs
			j = Grille->largeur-2;
		for(l = 0; i < Grille->hauteur-1 && l < 3; l++ ){//on parcourt a la verticale vers le bas et on verifie que l'on n'arrive pas dans un mur mais on peut en faire le tour
			if(Grille->ligne[i+1][j] == 'x'){
				if(i+2 < Grille->hauteur-2){
					if(Grille->ligne[i+2][j] != 'x')
							i+=2;
						else
							l = 6;
				}
				else
					l=6;
			}
			else
				i++;
		}

		//Horizontale gauche
		for( ; j > 1; j -=3){
			if(Grille->ligne[i][j] != 'x'){
				//printf("X: %d Y: %d\n",i, j );
				IntGrille = GenerationTabInt(Grille->hauteur, Grille->largeur, Grille);
				EditTabInt(i, j, Chemin->Fin->x, Chemin->Fin->y,Robot, IntGrille);
				Propagation(IntGrille, Chemin->Fin->x, Chemin->Fin->y);
				IntGrille->ligne[Chemin->Fin->x][Chemin->Fin->y] = IntGrille->maxWeight+1;
				Calcul_Chemin(IntGrille, Chemin, Chemin->Fin->x, Chemin->Fin->y, Grille, 0, Robot);
			}
		}

		//Verticale bas
		if(j <= 0)
			j = 2;
		for(l = 0; i < Grille->hauteur-1 && l < 3; l++ ){
			if(Grille->ligne[i+1][j] == 'x'){
				if(i+2 < Grille->hauteur-2){
					if(Grille->ligne[i+2][j] != 'x')
							i+=2;
						else
							l = 6;
				}
				else
					l=6;
			}
			else
				i++;
		}
	}
	//exit(0);
}
