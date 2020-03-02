// copyright Luca Istrate, Andrei Medar

#include "./decisionTree.h"  // NOLINT(build/include)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>
#include <random>

using std::string;
using std::pair;
using std::vector;
using std::unordered_map;
using std::make_shared;

// structura unui nod din decision tree
// splitIndex = dimensiunea in functie de care se imparte
// split_value = valoarea in functie de care se imparte
// is_leaf si result sunt pentru cazul in care avem un nod frunza
Node::Node() {
    is_leaf = false;
    left = nullptr;
    right = nullptr;
}

void Node::make_decision_node(const int index, const int val) {
    split_index = index;
    split_value = val;
}

void Node::make_leaf(const vector<vector<int>> &samples,
                     const bool is_single_class) {
    // TODO(you) - done
    // Seteaza nodul ca fiind de tip frunza (modificati is_leaf si result)
    // is_single_class = true -> toate testele au aceeasi clasa (acela e result)
    // is_single_class = false -> se alege clasa care apare cel mai des

    is_leaf = true;

    // daca toate sample-urile sunt de aceeasi clasa, il luam pe primul
    if (is_single_class) {
        result = samples[0].front();
        return;
    }

    // initializam vector de frecventa pentru a numara clasa  majoritara
    int count[10];
    for (int i = 0; i < 10; ++i) {
        count[i] = 0;
    }

    // altfel, trebuie sa gasim clasa majoritara
    int maxCount = 0;
    int maxIndex = -1;
    for (int i = 0; i < samples.size(); ++i) {
        // apare o clasa o data
        ++count[samples[i].front()];
        int index = samples[i].front();

        if (count[index] > maxCount) {
            maxCount = count[index];
            maxIndex = index;
        } else if (count[index] == maxCount && i < maxIndex) {
            maxIndex = i;
        }
    }

    result = maxIndex;
}

pair<int, int> find_best_split(const vector<vector<int>> &samples,
                               const vector<int> &dimensions) {
    // TODO(you) - done
    // Intoarce cea mai buna dimensiune si valoare de split dintre testele
    // primite. Prin cel mai bun split (dimensiune si valoare)
    // ne referim la split-ul care maximizeaza IG
    // pair-ul intors este format din (split_index, split_value)

    int splitIndex = -1, splitValue = -1;

    // alegem un numar de iteratii pentru a ajunge la un split bun
    double maxIG = 0;

    bool allZero = true;

    // double parentEntropy = get_entropy_by_indexes(samples, dimensions);
    double parentEntropy = get_entropy(samples);

    for (int i = 0; i < dimensions.size(); ++i) {
        int currSplitIdx = dimensions[i];

        int currSplitVal = 0;
        for (int j = 0; j < samples.size(); ++j) {
        	currSplitVal += samples[i][j];
        }
        currSplitVal /= samples.size()*dimensions.size();

        pair <vector<int>, vector<int>> split =
                get_split_as_indexes(samples, currSplitIdx, currSplitVal);

        // daca am gasit un split echilibrat, inseamna ca nu e frunza
        if (!split.first.empty() && !split.second.empty()) {
            allZero = false;
        }

        // daca o ramura e goala nu vrem sa calculam entropia
        if (split.second.empty() || split.first.empty()) {
             continue;
        }

        double leftEntropy = get_entropy_by_indexes(samples, split.first);
        double rightEntropy = get_entropy_by_indexes(samples, split.second);

        double wgSum = (split.first.size() * leftEntropy +
                      split.second.size() * rightEntropy) /
                      dimensions.size();


        double candidateIG = parentEntropy - wgSum;

        if (candidateIG > maxIG) {
            maxIG = candidateIG;
            splitIndex = currSplitIdx;
            splitValue = currSplitVal;
        }
    }

    if (allZero) {
        return {-1, -1};
    }

    return {splitIndex, splitValue};
}

void Node::train(const vector<vector<int>> &samples) {
    // TODO(you) - done
    // Antreneaza nodul curent si copii sai, daca e nevoie
    // 1) verifica daca toate testele primite au aceeasi clasa (raspuns)
    // Daca da, acest nod devine frunza, altfel continua algoritmul.
    // 2) Daca nu exista niciun split valid, acest nod devine frunza. Altfel,
    // ia cel mai bun split si continua recursiv

    // daca toate au aceeasi clasa, e ok
    if (same_class(samples)) {
        make_leaf(samples, true);
        return;
    }

    // daca nu exista niciun split bun, devine frunza
    // vector<int> dims = random_dimensions(samples.size());
    vector <int> dims;
    for (int i = 1; i < samples.size(); ++i) {
    	dims.push_back(i);
    }

    pair <int, int> bestSplit = find_best_split(samples, dims);
    if (bestSplit.first == -1 && bestSplit.second == -1) {
        make_leaf(samples, false);
        return;
    }

    // daca exista un split bun, il facem
    pair <vector<vector<int>>, vector<vector<int>>> currSplit;
    currSplit = split(samples, bestSplit.first, bestSplit.second);

    is_leaf = false;
    split_index = bestSplit.first;
    split_value = bestSplit.second;

    left = make_shared<Node>(Node());
    left->train(currSplit.first);

    right = make_shared<Node>(Node());
    right->train(currSplit.second);
}

int Node::predict(const vector<int> &image) const {
    // TODO(you) - done
    // Intoarce rezultatul prezis de catre decision tree

    if (is_leaf) {
        return result;
    }

    if (image[split_index - 1] <= split_value) {
        return left->predict(image);
    } else {
        return right->predict(image);
    }
}

bool same_class(const vector<vector<int>> &samples) {
    // TODO(you) - done
    // Verifica daca testele primite ca argument au toate aceeasi
    // clasa(rezultat). Este folosit in train pentru a determina daca
    // mai are rost sa caute split-uri

    int firstClass = samples[0].front();
    for (int i = 1; i < samples.size(); ++i) {
        // returneaza false daca se gaseste vreo clasa diferita de prima
        if (firstClass != samples[i].front()) {
            return false;
        }
    }

    return true;
}

float get_entropy(const vector<vector<int>> &samples) {
    // Intoarce entropia testelor primite
    assert(!samples.empty());
    vector<int> indexes;

    int size = samples.size();
    for (int i = 0; i < size; i++) indexes.push_back(i);

    return get_entropy_by_indexes(samples, indexes);
}

float get_entropy_by_indexes(const vector<vector<int>> &samples,
                             const vector<int> &index) {
    // TODO(you) - done
    // Intoarce entropia subsetului din setul de teste total(samples)
    // Cu conditia ca subsetul sa contina testele ale caror indecsi se gasesc in
    // vectorul index (Se considera doar liniile din vectorul index)

    // initializam p (reprezinta pi din formula de entropie)
    if (index.size() == 0) {
        return 0.0;
    }

    double p[10];
    for (int i = 0; i < 10; ++i) {
        p[i] = 0;
    }

    // calculam numaratorul de la pi
    for (int i = 0; i < index.size(); ++i) {
        int idx = index[i];
        int currClass = samples[idx].front();
        ++p[currClass];
    }


    // finalizam calculul lui pi si calculam si entropia
    double sum = 0;
    for (int i = 0; i < 10; ++i) {
        if (p[i] > 0) {
            p[i] /= index.size();
            sum += p[i] * log2(p[i]);
        }
    }

    return -(float)sum;
}

vector<int> compute_unique(const vector<vector<int>> &samples, const int col) {
    // TODO(you) - done, OK
    // Intoarce toate valorile (se elimina duplicatele)
    // care apar in setul de teste, pe coloana col
    vector<int> uniqueValues;

    vector<int> temp;

    for (int i = 0; i < samples.size(); ++i) {
        temp.push_back(samples[i][col]);
    }

    sort(temp.begin(), temp.end());

    for (int i = 1; i < temp.size(); ++i) {
        if (temp[i] != temp[i - 1]) {
            uniqueValues.push_back(temp[i - 1]);
        }
    }

    if (temp.size()) {
        uniqueValues.push_back(temp.back());
    }

    return uniqueValues;
}

pair<vector<vector<int>>, vector<vector<int>>> split(
    const vector<vector<int>> &samples, const int split_index,
    const int split_value) {
    // Intoarce cele 2 subseturi de teste obtinute in urma separarii
    // In functie de split_index si split_value
    vector<vector<int>> left, right;

    auto p = get_split_as_indexes(samples, split_index, split_value);
    for (const auto &i : p.first) left.push_back(samples[i]);
    for (const auto &i : p.second) right.push_back(samples[i]);

    return pair<vector<vector<int>>, vector<vector<int>>>(left, right);
}

pair<vector<int>, vector<int>> get_split_as_indexes(
    const vector<vector<int>> &samples, const int split_index,
    const int split_value) {
    // TODO(you) - done, OK
    // Intoarce indecsii sample-urilor din cele 2 subseturi obtinute in urma
    // separarii in functie de split_index si split_value

    vector<int> left, right;

    for (int i = 0; i < samples.size(); ++i) {
        if (samples[i][split_index] <= split_value) {
            left.push_back(i);
        } else {
            right.push_back(i);
        }
    }

    return make_pair(left, right);
}

vector<int> random_dimensions(const int size) {
    // TODO(you) - done OK
    // Intoarce sqrt(size) dimensiuni diferite pe care sa caute splitul maxim
    // Precizare: Dimensiunile gasite sunt > 0 si < size

    vector<int> rez;

    int sqRoot = (int) sqrt(size);
    vector<bool> app(size, false);

//    std::default_random_engine engine;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> idxGen(1, size - 1);

    while (rez.size() < sqRoot) {
        int random = idxGen(mt);
        while (app[random] == true) {
            random = idxGen(mt);
        }

        app[random] = true;
        rez.push_back(random);
    }

    return rez;
}
