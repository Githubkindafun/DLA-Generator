#include<bits/stdc++.h>
#include "Grid.h"
#include "Graph.h"
#include "Heightmap.h"
#include "DLA.h"
#include "config.hpp"

int main() {
    // if we pass SEED rng is done with this seed
    // otherwise it is totally random
    mt19937 rng;
    if(config::SEED == 0) {
        rng = mt19937{random_device{}()};
    } else {
        rng = mt19937{config::SEED};
    }
    
    // time for the cold start
    // We create empty graph and empy 8x8 grid
    Grid grid(config::START_SIZE, config::START_SIZE);
    Graph graph;
    // we place our seed at the center
    // not so random tho in future work we can add random placement
    // or custom seed like fancy shape
    int centerX = config::START_SIZE / 2;
    int centerY = config::START_SIZE / 2;
    grid.setNode(centerX, centerY, graph.addNode(centerX, centerY, -1));
    // alright so we run the DLA until we fill the amount we want
    int iter0Added = runDLA(grid, graph, config::FILL_RATIO, SpawnMode::Border, rng);
    // next we compute the heights for the nodes
    graph.computeHeights();
    // crisp part
    Heightmap crisp(config::START_SIZE, config::START_SIZE);
    crisp.renderFromGraph(graph);
    crisp = crisp.gaussianBlure(config::CRISP_PREBLURE_FRACTION * (float)(config::START_SIZE));
    // blure part
    Heightmap blure = crisp.gaussianBlure(config::BLURE_FRACTION * (float)(config::START_SIZE));
    // combine
    Heightmap combined = Heightmap::combine(crisp, blure, config::COMBINE_ALPHA);
    combined.normalize();
    combined.savePNG("iter0_cold_start.png");
    // so now it is time for the iterative part
    int currentSize = config::START_SIZE;
    for(int i = 1; i <= config::ITERATIONS; i++) {
        currentSize *= 2;
        SpawnMode mode;
        if(i >= config::CIRCLE_FROM_ITER) {
            mode = SpawnMode::Circle;
        } else {
            mode = SpawnMode::Border;
        }
        // crisp
        graph.crispResize(currentSize, currentSize, 1, rng);
        Grid newGrid(currentSize, currentSize);
        for(int j = 0; j < (int)(graph.nodes.size()); j++) {
            Node& n = graph.nodes[j];
            if(newGrid.inBounds(n.x, n.y) && !newGrid.isOccupied(n.x, n.y)) {
                newGrid.setNode(n.x, n.y, j);
            }
        }
        grid = move(newGrid);
        int crispAdded = runDLA(grid, graph, config::FILL_RATIO, mode, rng);
        graph.computeHeights();
        Heightmap crispHeightmap(currentSize, currentSize);
        crispHeightmap.renderFromGraph(graph);
        crispHeightmap = crispHeightmap.gaussianBlure(config::CRISP_PREBLURE_FRACTION * (float)(currentSize));
        // blure
        float blureSigma = config::BLURE_FRACTION * (float)(currentSize);
        Heightmap blureHeightmap = combined.linearInterpolationResize(currentSize, currentSize).gaussianBlure(blureSigma);
        // combine
        combined = Heightmap::combine(crispHeightmap, blureHeightmap, config::COMBINE_ALPHA);
        combined.normalize();
        combined.savePNG(format("iter{}_combined.png", i));
    }
    return 0;
}