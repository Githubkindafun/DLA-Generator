#pragma once
#include <bits/stdc++.h>
#include "Grid.h"
#include "Graph.h"

using namespace std;


// sposob losowania punktu startowego dla nowych czasteczek
// Uniform, losowy na calej przestrzeni
// Border, losowo na krawedziach
// Circle, losowo na okregu wokol istniejacego grafu
enum class SpawnMode {
    Uniform,
    Border,
    Circle
};

// rozrost grafu DLA
int runDLA(Grid& grid, Graph& graph, float targetFill, SpawnMode mode, mt19937& rng);