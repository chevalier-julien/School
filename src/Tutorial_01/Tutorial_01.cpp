// Salut !
// Bienvenue dans le tuto pour apprendre � programmer.
// On va voir les bases pour �crire un programme.
// Bon courage !


// Qu'est-ce qu'un programme ?
// C'est une liste d'instructions qui vont �tre execut�es les unes apr�s les autres par l'ordinateur.
// On appelle cette liste un algorithme.

// On peut �crire un algorithme en langue courante ou avec des sch�mas.
// Mais pour des raisons pratiques, les programmeurs ont d�velopp� leurs propres langages.
// Apprendre � programmer c'est aussi apprendre de nouvelles langues.

// On va utiliser le langage C pour �crire le programme.
// Il a commenc� � �tre d�velopp� dans les ann�es 70.
// C'est un langage tr�s r�pandu et qui est toujours d'actualit� notamment dans le jeu vid�o.

// Petite remarque : c'est un langage que l'ordinateur ne comprend pas directement.
// Il faut traduire le programme en langage machine.
// Et c'est autre programme (le compilateur) qui va faire la traduction durant une �tape qu'on appelle la compilation.
// Si le compilateur n'arrive pas � traduire le programme qu'on a �crit, il affichera une liste d'erreurs.

// On utilise Visual Studio qui est l'outil de Microsoft pour cr�er des programmes.
// Visual int�gre un compilateur pour le language C.
// Pour lancer une compilation, on appuie sur F7 ou "G�n�rer > G�n�rer la solution"
// Les messages du compilateur s'affichent dans la fen�tre de "Sortie".

// La compilation g�n�re un programme executable sous forme de fichier .exe .
// On peut lancer ce programme depuis Visual en appuyant sur F5 ou "D�boguer > D�marrer le d�bugage".


// Passons maintenant � l'�criture du programme !


// "Mais pourquoi tu commences toutes tes phrases par // ?"
// Et bien parcequ'on est dans le code d'un programme !
// Tout ce qu'on y �crit va �tre lu par le compilateur comme une instruction.
// Pour que le compilateur ne prenne pas en compte ce que j'�cris, on utilise la balise "//".
// Lorsqu'il voit "//", le compilateur ignore le reste de la ligne.
// C'est pratique pour laisser des commentaires explicatifs.
// Je peux aussi utiliser "/*" et "*/"


// A partir de l� commence les choses s�rieuses.
// C'est parti !


/* On programme souvent pour r�soudre un probl�me.
   Et on mod�lise souvent les probl�mes � l'aide des math�matiques.
   Par exemple, je veux connaitre la surface d'un terrain de foot.
   Je connais sa longueur (105m) et sa largeur (68m).
   Je peux poser : surface = longueur x largeur
   Easy ! */

   // Voil� le programme qui correspond � ce calcul :

int longueur = 105;
int largeur = 68;
int surface = longueur * largeur;

/* Remarquez qu'on utilise un ";" pour signaler la fin d'une instruction.

   Que fait ce programme dans le d�tail :
   - Cr�er une variable "longueur" de type "int" et lui assigner la valeur "105"
   - Cr�er une variable "largeur" de type "int" et lui assigner la valeur "68"
   - Cr�er une variable "surface" de type "int"
   - Calculer le produit de la valeur de la variable "longueur" et de la valeur de la variable "largeur"
   - Assigner le r�sultat du calcul � la variable "surface"

   Vous comprenez maintenant pourquoi on n'utilise pas le langage courant pour �crire des programmes ^^.

   Apr�s avoir execut� ces instructions, la variable "surface" a une valeur de "7140".
   Probl�me r�solu !

   Dans cet exemple on a utilis� des variables.
   Comme en maths on les utilise pour manipuler des valeurs.
   En cr�ant une variable, le programme r�serve une place dans la m�moire de l'ordinateur.
   Quand on utilise la variable dans une instruction, le programme va r�cup�rer cette valeur en m�moire.

   Le langage C demande de pr�ciser le type des variables.
   On a utilis� le type "int" qui correspond � un nombre entier (encod� sur 32bits en g�n�ral).
   Le langage C dispose de plusieurs autres types qu'on vera plus tard si besoin.

   Pour cr�er une variable, on utilise la syntaxe suivante : <type> nom_variable;

   Le signe "=" est l'op�rateur d'assignation.
   Il signifie "stocker la valeur � droite dans la variable � gauche".
   Je l'ai utilis� pour donner une valeur initiale aux variables.

   Enfin le signe "*" est l'op�rateur de multiplication.
   Il signifie "calculer le produit de la valeur � gauche et � droite".
   On peut aussi faire des additions "+" et des divisions "/".


   "Hey ! mais calculer la surface d'un terrain de foot �a sert � rien !"
   "Tu aurais pu juste �crire 'int surface = 7140;' !"
   Et c'est totalement vrai !
   On peut faire mieux.
   Par exemple un programme qui calcule une surface selon une longeur et une largeur qu'on ne connait pas encore. */

int CalculerSurfaceRectangle(int l, int L)
{
	int surface = l * L;
	return surface;
}

int surface_foot = CalculerSurfaceRectangle(105, 68);
int surface_tennis = CalculerSurfaceRectangle(24, 8);
int surface_petanque = CalculerSurfaceRectangle(15, 4);

/* "CaclulerSurfaceRectangle" est une fonction.
   Elle prend des param�tres en entr�e (entre des parenth�ses et s�par�s par des virgules).
   Et elle renvoit une valeur.
   Cool non ?
   Dans notre cas, on a deux variables de type "int" en entr�e ("l" et "L") et on renvoit une valeur de type "int".
   Le code qui permet de calculer la valeur de retour est situ� dans un "bloc" de code d�limit� par des accolades "{" et "}".
   le mot clef "return" indique la valeur qu'on veut renvoyer.

   La syntaxe d'une fonction :

   <type_de_sortie> NomDeLaFonction(<type_param> nom_param, ...)
   {
	  <mon_code_super_classe>
	  return <valeur_de_sortie>;
   }

   "Hey ! �a change toujours rien ! T'as juste tout compliqu� avec tes histoires de fonctions !"
   "Je te cacule tout �a � l'avance moi !"
   Et c'est encore vrai !
   Mais si maintenant on ne connaissait vraiment pas la longueur et la largeur de d�part ?
   Par exemple en demandant � l'utilisateur de les donner.

   Pour �a on va utiliser la console et son syst�me d'entr�es/sorties.
   A savoir :
   - Afficher du texte dans la fen�tre de console
   - R�cup�rer le texte tapp� dans la fen�tre de console

   Les programmeurs sont des fain�ants.
   Ou plut�t, ils n'aiment pas �crire plusieurs fois la m�me chose.
   On va avoir besoin de fonctionalit�s standards qui ont d�j� �t� �crites dans un autre fichier.
   Pour les copier dans mon code, j'utilise le mot clef #include suivi d'un nom de fichier.
   le fichier "iostream" contient des fonctionalit�s pour lire et �crire dans la fen�tre de console. */

#include <iostream>


   /* Voil� une nouvelle fonction.
	  Celle-ci est sp�ciale. On l'appelle le point d'entr�e du programme.
	  Lorsqu'on lance un programme c'est elle qui est execut�e en premier.
	  Son petit nom est g�n�ralement "main". */
int main()
{
	/* Pour �crire du texte dans la fen�tre de console on va utiliser "std::cout".
	   C'est un objet sp�cial qui repr�sente un flux vers la console.
	   Pour ins�rer du texte dans ce flux, on utilise l'op�rateur "<<".
	   Il signifie "Ins�rer ce qu'il y'a � droite dans le flux de gauche".
	   Le texte est form� d'une cha�ne de caract�res entre des double-guillemets : "<chaine_de_caracteres>".
	   Traditionellement le premier programme qu'on �crit s'appelle "Hello World !". */
	std::cout << "Hello World !\n";

	/* Le texte est encod� en ASCII.
	   C'est un alphabet comportant 256 caract�res.
	   Des chiffres, l'alphabet romain en majuscules et minuscules, de la ponctuation, etc...
	   Un caract�re sp�cial permet de sauter � la ligne suivante : '\n'.
	   On peut aussi utiliser "std::endl". */
	std::cout << "Je suis ton premier programme." << std::endl;
	std::cout << "Je ne suis pas un jeu mais je sais calculer des surfaces." << std::endl;

	// On peut envoyer plein de choses diff�rentes dans le flux "std::cout".
	// Par exemple pour afficher des valeurs.
	std::cout << "Je connais deja " << 3 << " surfaces." << std::endl;
	std::cout << "Un terrain de foot fait " << surface_foot << " m^2." << std::endl;

	// "std::cout" a un cousin qui s'appelle "std::cin".
	// C'est aussi un flux mais qui fonctionne dans le sens inverse de "std::cout".
	// Il permet de r�cup�rer ce qui est tap� dans la console et le copie dans une variable.
	std::cout << "Donne-moi un chiffre s'il te plait : ";

	int un_chiffre;
	std::cin >> un_chiffre;

	// On utilise l'op�rateur ">>".
	// Il signifie "�crit ce qui vient du flux � gauche dans la variable � droite".
	// Le programme va attendre qu'on appuie sur la touche "Enter" pour valider l'entr�e.

	std::cout << "Tu as tape le chiffre " << un_chiffre << ". Tres bon choix !" << std::endl;

	// On peut maintenant demander au programme de calculer une surface.
	std::cout << "Donne-moi une longueur et une largeur." << std::endl;

	int ma_longueur;
	std::cin >> ma_longueur;

	int ma_largeur;
	std::cin >> ma_largeur;

	int ma_surface = CalculerSurfaceRectangle(ma_longueur, ma_largeur);

	std::cout << "Ton rectangle fait " << ma_surface << " m^2." << std::endl;

	// Pour aller plus loin, on va demander au programme de comparer ce r�sultat � la taille d'un terrain de foot.
	// On va utiliser une structure de contr�le.
	// L'instruction "if" �value le r�sultat du test entre parenth�ses.
	// Si le r�sultat est vrai, alors le programme execute les instructions dans le bloc en-dessous.
	if (ma_surface == surface_foot)
	{
		std::cout << "C'est exactement la surface d'un terrain de foot !" << std::endl;
	}

	/* L'op�rateur "==" est l'op�rateur d'�galit�.
	   Il renvoit "vrai" si la valeur de gauche et la valeur de droite sont �gales.
	   Il existe d'autres op�rateurs de comparaison :

	   "!=" (diff�rent)
	   "<" (strictement inf�rieur), "<=" (inf�rieur ou �gal)
	   ">" (strictement sup�rieur), ">=" (sup�rieur ou �gal) */

	   // On peut aussi imbriquer les structures de contr�le.
	if (ma_surface != surface_foot)		// si ma_surface est diff�rent de surface_foot alors
	{
		if (ma_surface > surface_foot)	// si ma_surface est sup�rieur � surface_foot alors
		{
			std::cout << "C'est plus grand qu'un terrain de foot !" << std::endl;
		}
		else								// sinon
		{
			std::cout << "C'est plus petit qu'un terrain de foot !" << std::endl;
		}
	}

	/* Le mot clef "else" permet d'executer un bloc d'instructions diff�rent si le test �choue.
	   On peut aussi effectuer un nouveau test avec le mot clef "else if".
	   Pour r�sumer :

	   if (<test_1>)
	   {
		  <code_execut�_si_test_1_est_reussi>
	   }
	   else if (<test_2>)
	   {
		  <code_execut�_si_test_2_est_reussi>
	   }
	   ...
	   else if (<test_N>)
	   {
		  <code_execut�_si_test_N_est_reussi>
	   }
	   else
	   {
		  <code_execut�_si_les_tests_precedents_ont_echou�>
	   }
	*/

	system("pause");

	return 0;
}

// On a fini ce tutorial.
// On a vu l'environnement de d�veloppement et les bases du langage C avec :
// - les variables
// - les fonctions
// - les entr�es/sorties sur la console
// - les structures de contr�le
//
// A vous maintenant d'imaginer un jeu et de le r�aliser avec ce que vous avez appris
// Bonne journ�e !
