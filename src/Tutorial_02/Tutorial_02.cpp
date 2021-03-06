/* Pour ce tuto on va créer un programme pour jouer à papier, caillou, ciseaux.
   L'adversaire sera le programme lui-même.
   On va introduire deux nouveaux éléments :
   - l'aléatoire
   - les boucles
*/

// Pour gérer l'aléatoire, on a besoin des fichiers "cstdlib" et "ctime" qu'on rajoute à l'aide de l'instruction "#include".
#include <cstdlib>
#include <iostream>
#include <ctime>

int main()
{
	// On crée des variables pour contenir le score des deux opposants.
	int score_joueur = 0;
	int score_PC = 0;

	// On initialise le générateur de nombres aléatoires avec la fonction "std::srand".
	// On utilise "std::time" pour récupérer leur courante.
	// De cette manière, la séquence de nombres aléatoire sera différente à chaque lancement du jeu.
	std::srand((unsigned int)std::time(nullptr));

	// On affiche les règles du jeu.
	std::cout << "Shi Fu Mi !" << std::endl;
	std::cout << "Choisis parmis ces proposition :" << std::endl;
	std::cout << "0 => papier" << std::endl;
	std::cout << "1 => caillou" << std::endl;
	std::cout << "2 => ciseaux" << std::endl;

	/* L'instruction "while" démarre une boucle.
	   Les instructions contenues dans le bloc de code suivant vont être répété "tant que" la condition entre les parenthèse est "vraie".
	  
	   while (<condition>)
	   {
	      <code_répété>
	   }
	  
	   Dans notre cas, le condition est toujours vraie. C'est une boucle infinie.
	*/
	while (true)
	{
		std::cout << "Papier, Caillou, Ciseaux !" << std::endl;

		std::cout << "choix joueur : ";

		// On récupère le choix du joueur via "cin"
		int choix_joueur;
		std::cin >> choix_joueur;

		// Si le choix du joueur est un nombre négatif, on appelle l'instruction "break" qui permet de sortir de la boucle.
		// De cette manière le joueur décide quand quitter le jeu.
		if (choix_joueur < 0)
		{
			break;
		}

		// Pour le choix du PC, on utilise "std::rand" qui renvoit un nombre aléatoire.
		// Comme on veut un nombre entre 0 et 2, on utilise l'opérateur "%" ou "modulo".
		// Il renvoit le reste de la division entière du nombre à gauche par le nombre à droite.
		// Par exemple : 13%3 renvoit 1 . 13 = 4 x 3 + 1 . 1 est le reste de la division de 13 par 3.
		int choix_PC = std::rand()%3;

		std::cout << "choix PC " << choix_PC << std::endl;

		// Enfin on utilise les structures de contrôle pour déterminer qui gagne/perd et mettre à jour le score.
		// Ca fait beaucoup de "if" ^^.
		// Il existe une solution plus simple avec moins de répétitions de code.
		if (choix_joueur == choix_PC)
		{
			std::cout << "Egalite !" << std::endl;
		}
		else
		{
			if (choix_joueur == 0) // papier
			{
				if (choix_PC == 1) // papier vs caillou
				{
					std::cout << "Tu gagnes !" << std::endl;
					score_joueur = score_joueur + 1;
				}
				else
				{
					std::cout << "Tu perds !" << std::endl;
					score_PC = score_PC + 1;
				}
			}
			else if (choix_joueur == 1) // caillou
			{
				if (choix_PC == 2) // caillou vs ciseaux
				{
					std::cout << "Tu gagnes !" << std::endl;
					score_joueur = score_joueur + 1;
				}
				else
				{
					std::cout << "Tu perds !" << std::endl;
					score_PC = score_PC + 1;
				}
			}
			else if (choix_joueur == 2) // ciseaux
			{
				if (choix_PC == 0) // ciseaux vs papier
				{
					std::cout << "Tu gagnes !" << std::endl;
					score_joueur = score_joueur + 1;
				}
				else
				{
					std::cout << "Tu perds !" << std::endl;
					score_PC = score_PC + 1;
				}
			}
			else
			{
				std::cout << "Tu essayes de m'embrouiller ! Point au PC !" << std::endl;
				score_PC = score_PC + 1;
			}
		}

		std::cout << "Score : Joueur " << score_joueur << " PC " << score_PC << std::endl;
	}

	std::cout << "Game over !" << std::endl;

	system("pause");

	return 0;
}
