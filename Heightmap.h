#pragma once 
#include <bits/stdc++.h>
#include "Graph.h"
using namespace std;


class Heightmap {
private:

public:
    int width;
    int height;
    vector<float> data;

    Heightmap(int w, int h, float initValue = 0.0f) {
        this->width = w;
        this->height = h;
        data = vector<float>(w * h, initValue);
    }
    float get(int x, int y) {
        return data[y * width + x];
    }
    void set(int x, int y, float value) {
        data[y * width + x] = value;
    }
    // takes coordinates and returns the blend of 4 surranding pixels
    // basing how close the point is to each corner
    float sample(float x, float y);

    // returns new Heightmap at 2X resolution
    Heightmap linearInterpolationResize(int newWidth, int newHeight);

    // convolution :)
    Heightmap gaussianBlure(float sigma);

    // converts the DLA graph into heightmap
    void renderFromGraph(Graph& g);

    // self explainatory
    static Heightmap combine(Heightmap& crisp, Heightmap& blure, float alpha);

    // self explainatory
    void normalize();

    // self explainatory
    void savePNG(const string& path);
};