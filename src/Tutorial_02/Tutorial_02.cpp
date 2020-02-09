
#include <cstdlib>
#include <iostream>
#include <ctime>

int main()
{
	int score_joueur = 0;
	int score_PC = 0;

	std::srand(std::time(nullptr));

	std::cout << "Shi Fu Mi !" << std::endl;
	std::cout << "Choisis parmis ces proposition :" << std::endl;
	std::cout << "0 => papier" << std::endl;
	std::cout << "1 => caillou" << std::endl;
	std::cout << "2 => ciseaux" << std::endl;

	while (true)
	{
		std::cout << "Papier, Caillou, Ciseaux !" << std::endl;

		std::cout << "choix joueur : ";

		int choix_joueur;
		std::cin >> choix_joueur;

		if (choix_joueur < 0)
		{
			break;
		}

		int choix_PC = std::rand()%3;

		std::cout << "choix PC " << choix_PC << std::endl;

		if (choix_joueur == choix_PC)
		{
			std::cout << "Egalité !" << std::endl;
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