// copyright Luca Istrate, Andrei Medar
#include "randomForest.h"
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include "decisionTree.h"

using std::vector;
using std::pair;
using std::string;
using std::mt19937;

vector<vector<int>> get_random_samples(const vector<vector<int>> &samples,
                                       int num_to_return) {
    // TODO(you) - done
    // Intoarce un vector de marime num_to_return cu elemente random,
    // diferite din samples
    vector<vector<int>> ret;

    vector<int> dims;
    vector<bool> app(samples.size(), false);

    // generam num_to_return numere aleatorii din intervalul
    // 0...samples.size() - 1

    std::random_device rd;
    mt19937 mt(rd());
    std::uniform_int_distribution<int> idxGen(0, samples.size() - 1);

    while (dims.size() < num_to_return) {
        int random = idxGen(mt);
        while (app[random] == true) {
            random = idxGen(mt);
        }

        app[random] = true;
        dims.push_back(random);
    }

    for (int i = 0; i < dims.size(); ++i) {
        ret.push_back(samples[dims[i]]);
    }

    return ret;
}

RandomForest::RandomForest(int num_trees, const vector<vector<int>> &samples)
    : num_trees(num_trees), images(samples) {}

void RandomForest::build() {
    // Aloca pentru fiecare Tree cate n / num_trees
    // Unde n e numarul total de teste de training
    // Apoi antreneaza fiecare tree cu testele alese
    assert(!images.empty());
    vector<vector<int>> random_samples;

    int data_size = images.size() / num_trees;

    for (int i = 0; i < num_trees; i++) {
        // cout << "Creating Tree nr: " << i << endl;
        random_samples = get_random_samples(images, data_size);

        // Construieste un Tree nou si il antreneaza
        trees.push_back(Node());
        trees[trees.size() - 1].train(random_samples);
    }
}

int RandomForest::predict(const vector<int> &image) {
    // TODO(you) - done
    // Va intoarce cea mai probabila prezicere pentru testul din argument
    // se va interoga fiecare Tree si se va considera raspunsul final ca
    // fiind cel majoritar

    int freq[10];
    for (int i = 0; i < 10; ++i) {
        freq[i] = 0;
    }

    // numaram de cate ori apare fiecare predictie pentru imaginea data
    // si pastram maximul

    int maxFreq = 0;
    int maxAns = -1;
    for (int i = 0; i < trees.size()-1; ++i) {
        int currAns = trees[i].predict(image);
        ++freq[currAns];

        if (freq[currAns] > maxFreq) {
            maxFreq = freq[currAns];
            maxAns = currAns;
        }
    }

    return maxAns;
}
