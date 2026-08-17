#pragma once
#include <bits/stdc++.h>
#include "Graph.h"
#include "config.hpp"

using namespace std;

constexpr float INV_R2 = 1.0f / (float)(config::RIDGE_RADIUS * config::RIDGE_RADIUS);

class Heightmap {
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
    // bierze koordynaty i zwraca blend 4 sasiadujacych pikseli
    float sample(float x, float y);
    // podwojenie rozmiaru mapy
    Heightmap linearInterpolationResize(int newWidth, int newHeight);
    // rozmycia gaussa
    Heightmap gaussianBlure(float sigma);
    // render heightmapy z grafu
    void renderFromGraph(Graph& g);
    // laczenie czesci crisp i blure
    static Heightmap combine(Heightmap& crisp, Heightmap& blure, float alpha);
    void normalize();
    // zapis do PNG
    void savePNG(const string& path);
    // zapis do surowych floatow
    void saveRaw(const string& path);
    // export do .obj
    void exportOBJ(const string& path, float heightScale);
};