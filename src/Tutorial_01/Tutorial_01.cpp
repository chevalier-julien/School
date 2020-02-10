// Salut !
// Bienvenue dans le tuto pour apprendre à programmer.
// On va voir les bases pour écrire un programme.
// Bon courage !


// Qu'est-ce qu'un programme ?
// C'est une liste d'instructions qui vont être executées les unes après les autres par l'ordinateur.
// On appelle cette liste un algorithme.

// On peut écrire un algorithme en langue courante ou avec des schémas.
// Mais pour des raisons pratiques, les programmeurs ont développé leurs propres langages.
// Apprendre à programmer c'est aussi apprendre de nouvelles langues.

// On va utiliser le langage C pour écrire le programme.
// Il a commencé à être développé dans les années 70.
// C'est un langage très répandu et qui est toujours d'actualité notamment dans le jeu vidéo.

// Petite remarque : c'est un langage que l'ordinateur ne comprend pas directement.
// Il faut traduire le programme en langage machine.
// Et c'est autre programme (le compilateur) qui va faire la traduction durant une étape qu'on appelle la compilation.
// Si le compilateur n'arrive pas à traduire le programme qu'on a écrit, il affichera une liste d'erreurs.

// On utilise Visual Studio qui est l'outil de Microsoft pour créer des programmes.
// Visual intègre un compilateur pour le language C.
// Pour lancer une compilation, on appuie sur F7 ou "Générer > Générer la solution"
// Les messages du compilateur s'affichent dans la fenêtre de "Sortie".

// La compilation génère un programme executable sous forme de fichier .exe .
// On peut lancer ce programme depuis Visual en appuyant sur F5 ou "Déboguer > Démarrer le débugage".


// Passons maintenant à l'écriture du programme !


// "Mais pourquoi tu commences toutes tes phrases par // ?"
// Et bien parcequ'on est dans le code d'un programme !
// Tout ce qu'on y écrit va être lu par le compilateur comme une instruction.
// Pour que le compilateur ne prenne pas en compte ce que j'écris, on utilise la balise "//".
// Lorsqu'il voit "//", le compilateur ignore le reste de la ligne.
// C'est pratique pour laisser des commentaires explicatifs.
// Je peux aussi utiliser "/*" et "*/"


// A partir de là commence les choses sérieuses.
// C'est parti !


/* On programme souvent pour résoudre un problème.
   Et on modélise souvent les problèmes à l'aide des mathématiques.
   Par exemple, je veux connaitre la surface d'un terrain de foot.
   Je connais sa longueur (105m) et sa largeur (68m).
   Je peux poser : surface = longueur x largeur
   Easy ! */

   // Voilà le programme qui correspond à ce calcul :

int longueur = 105;
int largeur = 68;
int surface = longueur * largeur;

/* Remarquez qu'on utilise un ";" pour signaler la fin d'une instruction.

   Que fait ce programme dans le détail :
   - Créer une variable "longueur" de type "int" et lui assigner la valeur "105"
   - Créer une variable "largeur" de type "int" et lui assigner la valeur "68"
   - Créer une variable "surface" de type "int"
   - Calculer le produit de la valeur de la variable "longueur" et de la valeur de la variable "largeur"
   - Assigner le résultat du calcul à la variable "surface"

   Vous comprenez maintenant pourquoi on n'utilise pas le langage courant pour écrire des programmes ^^.

   Après avoir executé ces instructions, la variable "surface" a une valeur de "7140".
   Problème résolu !

   Dans cet exemple on a utilisé des variables.
   Comme en maths on les utilise pour manipuler des valeurs.
   En créant une variable, le programme réserve une place dans la mémoire de l'ordinateur.
   Quand on utilise la variable dans une instruction, le programme va récupérer cette valeur en mémoire.

   Le langage C demande de préciser le type des variables.
   On a utilisé le type "int" qui correspond à un nombre entier (encodé sur 32bits en général).
   Le langage C dispose de plusieurs autres types qu'on vera plus tard si besoin.

   Pour créer une variable, on utilise la syntaxe suivante : <type> nom_variable;

   Le signe "=" est l'opérateur d'assignation.
   Il signifie "stocker la valeur à droite dans la variable à gauche".
   Je l'ai utilisé pour donner une valeur initiale aux variables.

   Enfin le signe "*" est l'opérateur de multiplication.
   Il signifie "calculer le produit de la valeur à gauche et à droite".
   On peut aussi faire des additions "+" et des divisions "/".


   "Hey ! mais calculer la surface d'un terrain de foot ça sert à rien !"
   "Tu aurais pu juste écrire 'int surface = 7140;' !"
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
   Elle prend des paramètres en entrée (entre des parenthèses et séparés par des virgules).
   Et elle renvoit une valeur.
   Cool non ?
   Dans notre cas, on a deux variables de type "int" en entrée ("l" et "L") et on renvoit une valeur de type "int".
   Le code qui permet de calculer la valeur de retour est situé dans un "bloc" de code délimité par des accolades "{" et "}".
   le mot clef "return" indique la valeur qu'on veut renvoyer.

   La syntaxe d'une fonction :

   <type_de_sortie> NomDeLaFonction(<type_param> nom_param, ...)
   {
	  <mon_code_super_classe>
	  return <valeur_de_sortie>;
   }

   "Hey ! ça change toujours rien ! T'as juste tout compliqué avec tes histoires de fonctions !"
   "Je te cacule tout ça à l'avance moi !"
   Et c'est encore vrai !
   Mais si maintenant on ne connaissait vraiment pas la longueur et la largeur de départ ?
   Par exemple en demandant à l'utilisateur de les donner.

   Pour ça on va utiliser la console et son système d'entrées/sorties.
   A savoir :
   - Afficher du texte dans la fenêtre de console
   - Récupérer le texte tappé dans la fenêtre de console

   Les programmeurs sont des fainéants.
   Ou plutôt, ils n'aiment pas écrire plusieurs fois la même chose.
   On va avoir besoin de fonctionalités standards qui ont déjà été écrites dans un autre fichier.
   Pour les copier dans mon code, j'utilise le mot clef #include suivi d'un nom de fichier.
   le fichier "iostream" contient des fonctionalités pour lire et écrire dans la fenêtre de console. */

#include <iostream>


   /* Voilà une nouvelle fonction.
	  Celle-ci est spéciale. On l'appelle le point d'entrée du programme.
	  Lorsqu'on lance un programme c'est elle qui est executée en premier.
	  Son petit nom est généralement "main". */
int main()
{
	/* Pour écrire du texte dans la fenêtre de console on va utiliser "std::cout".
	   C'est un objet spécial qui représente un flux vers la console.
	   Pour insérer du texte dans ce flux, on utilise l'opérateur "<<".
	   Il signifie "Insérer ce qu'il y'a à droite dans le flux de gauche".
	   Le texte est formé d'une chaîne de caractères entre des double-guillemets : "<chaine_de_caracteres>".
	   Traditionellement le premier programme qu'on écrit s'appelle "Hello World !". */
	std::cout << "Hello World !\n";

	/* Le texte est encodé en ASCII.
	   C'est un alphabet comportant 256 caractères.
	   Des chiffres, l'alphabet romain en majuscules et minuscules, de la ponctuation, etc...
	   Un caractère spécial permet de sauter à la ligne suivante : '\n'.
	   On peut aussi utiliser "std::endl". */
	std::cout << "Je suis ton premier programme." << std::endl;
	std::cout << "Je ne suis pas un jeu mais je sais calculer des surfaces." << std::endl;

	// On peut envoyer plein de choses différentes dans le flux "std::cout".
	// Par exemple pour afficher des valeurs.
	std::cout << "Je connais deja " << 3 << " surfaces." << std::endl;
	std::cout << "Un terrain de foot fait " << surface_foot << " m^2." << std::endl;

	// "std::cout" a un cousin qui s'appelle "std::cin".
	// C'est aussi un flux mais qui fonctionne dans le sens inverse de "std::cout".
	// Il permet de récupérer ce qui est tapé dans la console et le copie dans une variable.
	std::cout << "Donne-moi un chiffre s'il te plait : ";

	int un_chiffre;
	std::cin >> un_chiffre;

	// On utilise l'opérateur ">>".
	// Il signifie "écrit ce qui vient du flux à gauche dans la variable à droite".
	// Le programme va attendre qu'on appuie sur la touche "Enter" pour valider l'entrée.

	std::cout << "Tu as tape le chiffre " << un_chiffre << ". Tres bon choix !" << std::endl;

	// On peut maintenant demander au programme de calculer une surface.
	std::cout << "Donne-moi une longueur et une largeur." << std::endl;

	int ma_longueur;
	std::cin >> ma_longueur;

	int ma_largeur;
	std::cin >> ma_largeur;

	int ma_surface = CalculerSurfaceRectangle(ma_longueur, ma_largeur);

	std::cout << "Ton rectangle fait " << ma_surface << " m^2." << std::endl;

	// Pour aller plus loin, on va demander au programme de comparer ce résultat à la taille d'un terrain de foot.
	// On va utiliser une structure de contrôle.
	// L'instruction "if" évalue le résultat du test entre parenthèses.
	// Si le résultat est vrai, alors le programme execute les instructions dans le bloc en-dessous.
	if (ma_surface == surface_foot)
	{
		std::cout << "C'est exactement la surface d'un terrain de foot !" << std::endl;
	}

	/* L'opérateur "==" est l'opérateur d'égalité.
	   Il renvoit "vrai" si la valeur de gauche et la valeur de droite sont égales.
	   Il existe d'autres opérateurs de comparaison :

	   "!=" (différent)
	   "<" (strictement inférieur), "<=" (inférieur ou égal)
	   ">" (strictement supérieur), ">=" (supérieur ou égal) */

	   // On peut aussi imbriquer les structures de contrôle.
	if (ma_surface != surface_foot)		// si ma_surface est différent de surface_foot alors
	{
		if (ma_surface > surface_foot)	// si ma_surface est supérieur à surface_foot alors
		{
			std::cout << "C'est plus grand qu'un terrain de foot !" << std::endl;
		}
		else								// sinon
		{
			std::cout << "C'est plus petit qu'un terrain de foot !" << std::endl;
		}
	}

	/* Le mot clef "else" permet d'executer un bloc d'instructions différent si le test échoue.
	   On peut aussi effectuer un nouveau test avec le mot clef "else if".
	   Pour résumer :

	   if (<test_1>)
	   {
		  <code_executé_si_test_1_est_reussi>
	   }
	   else if (<test_2>)
	   {
		  <code_executé_si_test_2_est_reussi>
	   }
	   ...
	   else if (<test_N>)
	   {
		  <code_executé_si_test_N_est_reussi>
	   }
	   else
	   {
		  <code_executé_si_les_tests_precedents_ont_echoué>
	   }
	*/

	system("pause");

	return 0;
}

// On a fini ce tutorial.
// On a vu l'environnement de développement et les bases du langage C avec :
// - les variables
// - les fonctions
// - les entrées/sorties sur la console
// - les structures de contrôle
//
// A vous maintenant d'imaginer un jeu et de le réaliser avec ce que vous avez appris
// Bonne journée !
