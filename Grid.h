#pragma once
#include<bits/stdc++.h>

using namespace std;

class Grid{
private:
    // wartosc komorki wektora -> indeks wierzcholka w grafie wpp. -1
    vector<int> cellToNode;
    // helper do zamiany wspolrzednych z 2D na wspolrzedne w wektorze 1D
    int index(int x, int y) {
        return y * width + x;
    }
    // ilosc zajetych komorek 
    int occupied = 0;
public:
    int width;
    int height;
    Grid(int w, int h) {
        this->width = w;
        this->height = h;
        // startuje puste -> wszystko na -1
        cellToNode = vector<int>(w * h, -1);
    }
    // czy (x, y) znajduje sie w granicach obszaru width x height
    bool inBounds(int x, int y) {
        return (x >= 0 && x < width) && (y >= 0 && y < height);
    }
    // czy zajeta komorka
    bool isOccupied(int x, int y) {
        return cellToNode[index(x, y)] != -1;
    }
    // indeks wierzcholka w tej komorce 
    int getNode(int x, int y) {
        return cellToNode[index(x, y)];
    }
    // wypelnienie komorki wierzcholkiem + incrementujemy ilosc zajetych komorek
    void setNode(int x, int y, int nodeIndex) {
        int& cell = cellToNode[index(x, y)];
        if(cell == -1) {
            occupied++;
        }
        cell = nodeIndex;
    }
    // liczba zajetych komorek, przydaje sie przy glownej petli runDLA
    int occupiedCount() {
        return occupied;
    }
    // przydatne do wyliczania FILL_RATIO
    int totalCells() {
        return width * height;
    }
    // warunek dla komorki bledzacej losowo (zasadniczo kiedy sie zatrzymuje)
    bool hasOccupiedNeighbour(int x, int y) {
        return (x + 1 < width && isOccupied(x + 1, y)) || 
        (x - 1 >= 0 && isOccupied(x - 1, y)) ||
        (y + 1 < height && isOccupied(x, y + 1)) ||
        (y - 1 >= 0 && isOccupied(x, y - 1));
    }
};