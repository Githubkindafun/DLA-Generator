#include<bits/stdc++.h>
#include "Grid.h"
#include "Graph.h"
#include "Heightmap.h"
#include "DLA.h"
#include "config.hpp"

using namespace std;

int main() {
    // seed = 0 totalnie losowy seed,
    // wpp. seed = SEED z config
    uint32_t seed = config::SEED;
    mt19937 rng;
    if(seed == 0) {
        rng = mt19937{random_device{}()};
    } else {
        rng = mt19937{seed};
    }
    // Cold start, zwykle DLA na gridzie START_SIZE x START_SIZE
    Grid grid(config::START_SIZE, config::START_SIZE);
    Graph graph;
    int centerX = config::START_SIZE / 2;
    int centerY = config::START_SIZE / 2;
    grid.setNode(centerX, centerY, graph.addNode(centerX, centerY, -1));
    runDLA(grid, graph, config::FILL_RATIO, SpawnMode::Border, rng);
    graph.computeHeights();
    // czesc ostra
    Heightmap crisp(config::START_SIZE, config::START_SIZE);
    crisp.renderFromGraph(graph);
    crisp = crisp.gaussianBlure(config::CRISP_PREBLURE_FRACTION * (float)(config::START_SIZE));
    // czesc rozmyta
    Heightmap blure = crisp.gaussianBlure(config::BLURE_FRACTION * (float)(config::START_SIZE));
    Heightmap combined = Heightmap::combine(crisp, blure, config::COMBINE_ALPHA);
    combined.normalize();
    // podwajanie rozdielczosci
    int currentSize = config::START_SIZE;
    SpawnMode mode;
    for(int i = 1; i <= config::ITERATIONS; i++) {
        currentSize *= 2;
        if(i >= config::CIRCLE_FROM_ITER) {
            mode = SpawnMode::Circle;
        } else {
            mode = SpawnMode::Border;
        }
        // podwojenie rozdielczosci grafu, przebudowa grida zajetych komorek
        // kazdy wierzcholek dostaje komorke
        graph.crispResize(currentSize, currentSize, 1, rng);
        Grid newGrid(currentSize, currentSize);
        for(int j = 0; j < (int)(graph.nodes.size()); j++) {
            Node& n = graph.nodes[j];
            if(newGrid.inBounds(n.x, n.y) && !newGrid.isOccupied(n.x, n.y)) {
                newGrid.setNode(n.x, n.y, j);
            }
        }
        grid = move(newGrid);
        runDLA(grid, graph, config::FILL_RATIO, mode, rng);
        graph.computeHeights();
        Heightmap crispHeightmap(currentSize, currentSize);
        crispHeightmap.renderFromGraph(graph);
        crispHeightmap = crispHeightmap.gaussianBlure(config::CRISP_PREBLURE_FRACTION * (float)(currentSize));
        // czesc rozmyta
        float blurSigma = config::BLURE_FRACTION * (float)(currentSize);
        Heightmap blurHeightmap = combined.linearInterpolationResize(currentSize, currentSize).gaussianBlure(blurSigma);
        combined = Heightmap::combine(crispHeightmap, blurHeightmap, config::COMBINE_ALPHA);
        combined.normalize();
    }
    string base = "s" + to_string(seed);
    combined.saveRaw(base + ".r32");
    combined.savePNG(base + ".PNG");
    if(config::EXPORT_TO_OBJ) {
        combined.exportOBJ(base + ".obj", config::OBJ_HEIGHT_SCALE);
    }
    return 0;
}