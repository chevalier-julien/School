
#include <iostream>
#include <fstream>

char* ChargerTexte(const char* nom_fichier)
{
	std::ifstream fs;
	fs.open(nom_fichier, std::fstream::binary);

	char* texte = nullptr;
	if (fs)
	{
		fs.seekg(0, fs.end);
		int len = fs.tellg();
		fs.seekg(0, fs.beg);

		texte = new char[len+1];
		fs.read(texte, len);
		texte[len] = 0;

		fs.close();
	}

	return texte;
}

void DechargerTexte(char* texte)
{
	delete[] texte;
}

int main()
{
	char* gogogo = ChargerTexte("gogogo.txt");

	std::cout << gogogo;

	DechargerTexte(gogogo);

	system("pause");

	return 0;
}
