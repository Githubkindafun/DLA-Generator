#pragma once
#include<bits/stdc++.h>
#include "config.hpp"

using namespace std;

struct Node {
    int x;
    int y;
    // indeks rodzica 
    int parent = -1;
    // wysokosc wierzcholka
    float height = 0.0f;
    // indeksy dzieci wierzcholka
    vector<int> children;
};

class Graph {
public:
    vector<Node> nodes;
    int root = -1;
    // dodaje wierzcholek i zwraca jego indeks
    int addNode(int x, int y, int parent) {
        int index = (int)(nodes.size());
        Node& n = nodes.emplace_back();
        n.x = x;
        n.y = y;
        n.parent = parent;
        if(parent >= 0) {
            nodes[parent].children.push_back(index);
        }
        if(root < 0) {
            root = index;
        }
        return index;
    }
    // przydziela wysokosc wszystkim wierzcholkom
    // odbywa sie na koniec kazdej iteracji:
    // 1. BFS od korzenia, wyznacza glebokosc kazdego wierzcholka
    // 2. toposort (od lisci do korzenia)
    // 3. wyznaczenie surowej wysokosci
    // 4. normalizacja + wykorzystanie funkcji wysokosci
    void computeHeights() {
        if(root < 0 || nodes.empty()) {
            return;
        }
        int n = (int)(nodes.size());
        // 1. BFS odleglosc wierzcholkow od korzenia
        vector<int> depthFromRoot(n, 0);
        queue<int> que;
        que.push(root);
        while(!que.empty()) {
            int index = que.front();
            que.pop();
            for(int child : nodes[index].children) {
                depthFromRoot[child] = depthFromRoot[index] + 1;
                que.push(child);
            }
        }
        // 2. toposort
        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {return depthFromRoot[a] > depthFromRoot[b];});
        // 3. wyznaczanie wysokosci od lisci w gore
        vector<int> heightRaw(n, 0);
        for(int i : order) {
            if(nodes[i].children.empty()) {
                heightRaw[i] = 1;
            } else {
                int best = 0;
                for(int child : nodes[i].children) {
                    best = max(best, heightRaw[child]);
                }
                heightRaw[i] = 1 + best;
            }
        }
        // 4. normalizacja i funkcja wysokosci
        int heightMax = 0;
        for(int i = 0; i < n; i++) {
            heightMax = max(heightMax, heightRaw[i]);
        }
        float invHeightMax;
        if(heightMax > 0) {
            invHeightMax = 1.0f / (float)(heightMax);
        } else {
            invHeightMax = 1.0f;
        }
        for(int i = 0; i < n; i++) {
            // normalizacja do [0,1]
            float height = (float)(heightRaw[i]) * invHeightMax;
            // funkcja z oryginalnego wideo Josha
            nodes[i].height = 1.0f - 1.0f /(1.0f + height);
        }
    }

    // podwaja rozdzielczosc grafu przed kolejna iteracja DLA w dwoch krokach:
    // 1. mnozy wspolrzedne wszystkich wezlow x2
    // 2. na kazdej krawedzi miedzy 2 wierzcholkami (powiekszonej) wstawia nowy werzcholek
    // z losowym jiggle dla bardziej naturalnie wygladajacego rezultatu
    void crispResize(int newGridWidth, int newGridHeight, int jiggleRadius, mt19937& rng) {
        nodes.reserve(nodes.size() * 2 + 1);
        // 1. podwojenie wspolrzednych
        for(Node& node : nodes) {
            node.x *= 2;
            node.y *= 2; 
        }
        // 2. nowy wierzcholek na kazda krawedz
        int originalNodesCount = (int)(nodes.size());
        // losowy jiggle
        uniform_int_distribution<int> jiggle(-jiggleRadius, jiggleRadius);
        for(int i = 0; i < originalNodesCount; i++) {
            int parent = nodes[i].parent;
            if(parent < 0) {
                continue;
            }
            int middleX = (nodes[i].x + nodes[parent].x) / 2 + jiggle(rng);
            int middleY = (nodes[i].y + nodes[parent].y) / 2 + jiggle(rng);
            middleX = clamp(middleX, 0, newGridWidth - 1);
            middleY = clamp(middleY, 0, newGridHeight - 1);
            int midIndex = (int)(nodes.size());
            // no i wpinamy nowy wierzcholek
            auto& parentChildren = nodes[parent].children;
            replace(parentChildren.begin(), parentChildren.end(), i, midIndex);
            nodes[i].parent = midIndex;
            Node mid;
            mid.x = middleX;
            mid.y = middleY;
            mid.parent = parent;
            mid.children.push_back(i);
            nodes.push_back(move(mid));
        }
    }
};