# Optical-Character-Recognition

Algoritmul utilizat:

Decision Tree:
	    void make_leaf(const vector<vector<int>> &samples, 
	const bool is_single_class)
		Daca toate sample-urile sunt de aceeasi clasa, pur si simplu setam
		rezultatul ca fiind clasa primului sample. Altfel tinem un vector de
		frecventa pentru a numara aparitiile fiecarei clase. O alegem pe cea
		majoritara, iar daca sunt mai multe la fel, o alegem pe cea mai mica.

	    bool same_class(const vector<vector<int>> &samples)
		Comparam toate clasele din samples cu clasa primului sample. Daca
		difera vreunul, atunci nu sunt toate la fel.

	    float get_entropy_by_indexes(const vector<vector<int>> &samples, 
	const vector<int> &index)
		Calculam dupa formula din cerinta entropia unui set de sample-uri
		bazat pe indicii din vectorul index. Pentru toate samples[i] cu i in
		index, numaram intai aparitiile fiecarei clase, dupa care le impartim
		la numarul total de clase. In final inmultim aceste probabilitati cu
		logaritmul lor si le insumam.


	    vector<int> compute_unique(const vector<vector<int>> &samples, const int col)
		Adaugam in temp toate valorile de pe coloana col, dupa care sortam
		vectorul. Apoi comparam fiecare element din temp cu cel precedent, iar
		daca sunt diferite, il adaugam pe cel precedent. Astfel pentru o
		secventa de numere identice, doar ultimul va fi adaugat. Ulterior,
		trebuie sa adaugam ultimul element din temp (caci nu avem cu cine sa
		il comparam).

	  pair<vector<int>, vector<int>> get_split_as_indexes(
	const vector<vector<int>> &samples,
	const int split_index, const int split_value)
		Stiind split_index si split_value, putem pur si simplu sa parcurgem
		samples si sa comparam samples[i][split_index] cu split_value, pe
		parcurs adaugand "i"urile in doi vectori pe care ii vom returna.

	    vector<int> random_dimensions(const int size)
		Generam sqrt(size) numere aleatorii intre 1 si size - 1, la fiecare
		verificand daca nu a fost deja generat (pentru a ne asigura ca sunt
		unice).

	    int predict(const vector<int> &image) const
		Un simplu DFS ce returneaza valorile obtinute din fiul stang, respectiv
		drept, dupa caz (in functie de split_index si split_value) calculate
		anterior. Image nu are clasa pe prima pozitie, deci split_index trebuie
		scazut cu 1. Conditia de oprire este daca nodul in care am ajuns este
		frunza, caz in care intoarcem rezultatul stocat in structura.

	    pair<int, int> find_best_split(const vector<vector<int>> &samples,
	const vector<int> &dimensions)
		Intai calculam entropia parintelui, de care vom avea nevoie mai
		tarziu pentru a calcula Information Gain, conform cerintei. Parcurgem
		fiecare pereche posibila de split_index si split_val pentru a o gasi
		pe cea cu IG maxim. Pastram doar valorile unice din split_val la un
		moment de timp, pentru a nu repeta teste. Apelam functia 
		get_split_as_indexes pentru a gasi splitul asociat perechii curente,
		dupa care updatam flagul allZero (daca toate spliturile au o ramura
		goala, nodul este frunza). Daca avem o ramura goala, pur si simplu
		trecem mai departe, nu este un split bun. Urmatoarele 2 instructiuni
		repetitive intersecteaza vectorii de indecsi de splituri
		obtinute anterior cu dimensions (dims este practic dimensions sortat).
		Dupa intersectii, ramane doar de calculat entropia fiilor si IGul.
		Daca este mai mare decat maximul de dinainte, updatam perechea 
		split_index si split_val, dar si noul IG maxim. La final verificam daca
		nodul va deveni frunza sau nu.

	    void train(const vector<vector<int>> &samples)
		Din nou un DFS, insa putin mai complex. Primele 2 "if"uri verifica
		daca nodul curent trebuie sa devina frunza sau nu (cele 2 sunt
		diferite) datorita faptului ca e posibil sa aiba toate sample-urile
		aceeasi clasa sau pur si simplu sa nu existe un split viabil. Daca,
		totusi exista unul, il calculam si cream matricea de samples pentru
		ramura stanga si ramura dreapta. Apoi setam split_index si split_value
		si cream fii pentru nodul curent, dupa care executam aceleasi operatii
		si pe ele.

Random Forest:
	    int predict(const vector<int> &image)
		Apeleaza predict() pe toti arborii alesi aleator si tine un vector de
		frecventa pe raspunsuri. Rezultatul este pur si simplu clasa obtinuta
		cel mai des.

	    vector<vector<int>> get_random_samples(const vector<vector<int>>& samples, int num_to_return)
		Asemenea functiei random_dimensions, genereaza num_to_return numere
		aleatorii in intervalul cerut. Daca se repeta un numar, il generam
		din nou pana devine unic. La final, trebuie sa extragem din samples
		imaginile cu indecsii generati si sa returnam matricea.
