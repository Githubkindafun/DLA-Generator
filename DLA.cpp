#include <bits/stdc++.h>
#include "DLA.h"
#include <math.h>

using namespace std;

float SAFE_MARGIN = 2.0f;
int MAX_ATTEMPTS = 1024;

int runDLA(Grid& grid, Graph& graph, float targetFill, SpawnMode mode, std::mt19937& rng) {
    // directions up, down, right, left
    vector<int> dx{0, 0, 1, -1};
    vector<int> dy{1, -1, 0, 0};

    int target = (int)(targetFill * grid.totalCells());
    int added = 0;
    // alright now we need some randomness
    // used only for random spawn - currently unused but still can be used 
    // gives diffrent results when used
    uniform_int_distribution<int> randX(0, grid.width - 1);
    uniform_int_distribution<int> randY(0, grid.height - 1);
    // used to select index from dx/dy for random walker for unfiorm chance for each
    // direction
    uniform_int_distribution<int> randDirection(0, 3);
    // for border selection during border spawn
    uniform_int_distribution<int> randSide(0, 3);
    // for circle spawn to slect where on the circle to spawn
    uniform_real_distribution<float> randAngle(0.0f, 2 * M_PI);

    // precomputation for circle spawn
    // center of the cricle +/- the DLA struct should grow inside of it
    float circleCenterX = (grid.width - 1) * 0.5f;
    float circleCenterY = (grid.height - 1) * 0.5f;
    // max circle for current resolution
    // if we were to change the "spawn area" from square to something like this
    // #0#
    // 000
    // #0#
    // we need some more elaborate selection for radius
    // ah and max radius is basicly the distance to the closest edge from the center
    // float maxRadius = min({circleCenterX, circleCenterY, grid.width - 1 - circleCenterX, grid.height - 1 - circleCenterY}) - SAFE_MARGIN;
    // but for now since we work only on square areas we can use this 
    float maxRadius = (grid.width - 1) / 2.0f - SAFE_MARGIN;
    float currentRadius = 0.0f;
    if(mode == SpawnMode::Circle) {
        for(Node& n : graph.nodes) {
            float centerToX = n.x - circleCenterX;
            float centerToY = n.y - circleCenterY;
            // pitagoras :o
            float tmpRadius = sqrt(centerToX * centerToX + centerToY * centerToY);
            if(tmpRadius > currentRadius) {
                currentRadius = tmpRadius;
            }
        }
    }
    float safetyCricleMargin = min({grid.width, grid.height}) * 0.1f;

    while(grid.occupiedCount() < target) {
        int px = 0;
        int py = 0;
        switch(mode) {
            // totaly random mode where walker spawns in random place
            case SpawnMode::Uniform: {
                do {
                    px = randX(rng);
                    py = randY(rng);
                } while(grid.isOccupied(px, py));
                break;
            }
            // classic border case where we spawn a walker from a random
            // side of the gird
            // still it does work when we have square or rectangle might
            // be problematic if we were to change the grid shape
            // in case of failing to many times we go with uniform spawn
            case SpawnMode::Border: {
                int failedAttempts = 0;
                bool goUniform = false;
                do {
                    int side = randSide(rng);
                    switch (side) {
                    case 0:
                        px = randX(rng);
                        py = 0;
                        break;
                    case 1:
                        px = randX(rng);
                        py = grid.height - 1;
                        break;
                    case 2:
                        px = 0;
                        py = randY(rng);
                        break;
                    case 3:
                        px = grid.width - 1;
                        py = randY(rng);
                        break;
                    }
                    failedAttempts++;
                    if(failedAttempts > MAX_ATTEMPTS) {
                        do {
                            px = randX(rng);
                            py = randY(rng);
                        } while(grid.isOccupied(px, py));
                        goUniform = true;
                        break;
                    }
                } while(!goUniform && grid.isOccupied(px, py));
                break;
            }

            case SpawnMode::Circle: {
                // we compute the spawn radius for spedific particle
                // we recompute it every run
                float spawnRadius = max(1.5f * currentRadius, currentRadius + safetyCricleMargin);
                spawnRadius = min(spawnRadius, maxRadius);
                int failedAttempts = 0;
                bool goUniform = false;
                while(true) {
                    float angle = randAngle(rng);
                    // we convert angle to cartesian
                    float x = circleCenterX + spawnRadius * cos(angle);
                    float y = circleCenterY + spawnRadius * sin(angle);
                    px = (int)round(x);
                    py = (int)round(y);

                    if(grid.inBounds(px, py) && !grid.isOccupied(px, py)) {
                        break;
                    }

                    failedAttempts++;
                    if(failedAttempts > MAX_ATTEMPTS) {
                        do{
                            px = randX(rng);
                            py = randY(rng);
                        } while(grid.isOccupied(px, py));
                        goUniform = true;
                        break;
                    }
                }
                break;
            }
        }
        // so we got our starting point now walker now needs to do his work
        // that is traverse the grid and stick to something
        while(!grid.hasOccupiedNeighbour(px, py)) {
            int directon = randDirection(rng);
            int nextX = px + dx[directon];
            int nextY = py + dy[directon];

            if(grid.inBounds(nextX, nextY)) {
                px = nextX;
                py = nextY;
            }
        }
        // so it is time to stick now
        int parentNode = -1;
        for(int i = 0; i < 4; i++) {
            int nextX = px + dx[i];
            int nextY = py + dy[i];
            if(grid.inBounds(nextX, nextY) && grid.isOccupied(nextX, nextY)) {
                parentNode = grid.getNode(nextX, nextY);
                break;
            }
        }
        int nodeIdx = graph.addNode(px, py, parentNode);
        grid.setNode(px, py, nodeIdx);
        added++;
        // alright so we end here, buuut we need to update the circle data
        if(mode == SpawnMode::Circle) {
            float centerToX = px - circleCenterX;
            float centerToY = py - circleCenterY;
            float tmpRadius = sqrt(centerToX * centerToX + centerToY * centerToY);
            if(tmpRadius > currentRadius) {
                currentRadius = tmpRadius;
            }
        }
    }
    return added;
}