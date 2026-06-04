#pragma once
#include <bits/stdc++.h>
#include "Grid.h"
#include "Graph.h"

using namespace std;

enum class SpawnMode{
    Uniform,
    Border,
    Circle
};

// self expaintory but lets add a bit more detail:
// Uniform - totaly random
// Border - spawning from border 
// Circle - spawning on a circle around structure to save time

int runDLA(Grid& grid, Graph& graph, float targetFill, SpawnMode mode, mt19937& rng);