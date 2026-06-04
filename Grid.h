#pragma once
#include <bits/stdc++.h>

using namespace std;

class Grid {
private:
    // we store in a row major style :)
    // xxxyyy ->
    // xxx
    // yyy
    vector<int> cellToNode;
    // usefull helper since we went with major style :)
    int index(int x, int y) {
        return y * width + x;
    }
    int occupied = 0;

public:
    // current grid resolution 8x8 -> 16x16 -> ...
    int width; 
    int height;
    Grid(int w, int h) {
        this->width = w;
        this->height = h;
        // we initialize the grid with -1 (empty)
        cellToNode = vector<int>(w * h, -1);
    }

    // some self explainatory fuctions
    // basicly helpers for random walker
    bool inBounds(int x, int y) {
        return (x >= 0 && x < width) && (y >= 0 && y < height);
    }

    bool isOccupied(int x, int y) {
        return cellToNode[index(x, y)] != -1;
    }

    int getNode(int x, int y) {
        return cellToNode[index(x, y)];
    }

    // self explainatory again basicly for seting a gird cell occupied
    void setNode(int x, int y, int nodeIndex) {
        int& cell = cellToNode[index(x, y)];
        if(cell == -1) {
            occupied++;
        }
        cell = nodeIndex;
    }

    // returns the count of all ocupied cells in current grid size
    int occupiedCount() {
        return occupied;
    }

    // returns the count of all cells in current grid size
    int totalCells() {
        return width * height;
    }

    // condition to stop random walker and stick somwhere
    // cheks all neighbours up, down, left, right
    // for now we stick with 4 way walker in the future 
    // we could experiment with "8 way" walker
    bool hasOccupiedNeighbour(int x, int y) {
        return (x + 1 < width && isOccupied(x + 1, y)) || 
        (x - 1 >= 0 && isOccupied(x - 1, y)) || 
        (y + 1 < height && isOccupied(x, y + 1)) || 
        (y - 1 >= 0 && isOccupied(x, y - 1));
    }
};