#include "main.h"

int main(int argc, char** argv){
	
// On test les paramètres du programme.
	if (argc != 2) {//verification de la presence d'un fichier
		printf("L'utilisation du programme est incorrect !!\n");
		printf("Utilisation :\n");
		printf("\t%s carte.txt\n\n", argv[0]);
		return EXIT_FAILURE;
	}

	sGrille* Grille;//on importe la carte d'apres la saisie de l'utilisateur
	Grille = Import(argv[1]);
	
	pRep* Chemin;//on creer une liste doublement chainee pour stocker le parcourt du robot
	Chemin = Init_Rep();

	sRobot Robot;//on initialise le robot
	Robot = Init_Robot(Grille);
	
	sIntGrille* IntGrille;//on genere un tableau de int pour la pathfinding d'apres la carte
	IntGrille = GenerationTabInt(Grille->hauteur, Grille->largeur, Grille);

	Propagation(IntGrille, Robot.x, Robot.y);//calcul pathfinding
	IntGrille->ligne[Robot.x][Robot.y] = IntGrille->maxWeight+1;//on remplace le robot par la poid max+1 pour pouvoir calculer le chemin le plsu court

	Calcul_Chemin(IntGrille, Chemin, Robot.x, Robot.y, Grille, 1, &Robot);//on calcul la trajectoire vers l'extincteur puis le feu

	Suivre_Chemin(Grille, &Robot, Chemin);//on deplace le robot en suivant le chemin generer
	if(Robot.vie && Robot.FeuTrouver)//on affiche si le feu est eteint ou non
		printf("\nLe Super-Robot pompier a reussi a eteindre le feu !\n");
	else
		printf("\nNotre Super-Robot pompier n'a pas reussi a trouver le feu votre appartement a bruler, une autre fois peut etre !\n");


	return EXIT_SUCCESS;
}