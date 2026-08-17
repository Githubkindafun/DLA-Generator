#include <bits/stdc++.h>
#include "DLA.h"
#include <math.h>

using namespace std;

float SAFE_MARGIN = 2.0f;
int MAX_ATTEMPTS = 1024;

// rozrozst grafu DLA na gridzie dodajac wierzcholki do grafu
int runDLA(Grid& grid, Graph& graph, float targetFill, SpawnMode mode, std::mt19937& rng) {
    // cztery kierunki dla czasteczek 
    vector<int> dx{0, 0, 1, -1};
    vector<int> dy{1, -1, 0, 0};

    int target = (int)(targetFill * grid.totalCells());
    int added = 0;
    // losowa kolumna i losowy wiersz (dla rollbacku gdy przekroczone zostanie max attempts)
    uniform_int_distribution<int> randX(0, grid.width - 1);
    uniform_int_distribution<int> randY(0, grid.height - 1);
    // kierunek ruchu
    uniform_int_distribution<int> randDirection(0, 3);
    // wybor krawdzi dla Border Spawn
    uniform_int_distribution<int> randSide(0, 3);
    // miejsce na okregu przy Circle Spawn
    uniform_real_distribution<float> randAngle(0.0f, 2 * M_PI);

    // srodek okregu 
    float circleCenterX = (grid.width - 1) * 0.5f;
    float circleCenterY = (grid.height - 1) * 0.5f;
    // maksymalna srednica od srodka do krawedzi
    float maxRadius = (grid.width - 1) / 2.0f - SAFE_MARGIN;
    float currentRadius = 0.0f;
    if(mode == SpawnMode::Circle) {
        for(Node& n : graph.nodes) {
            float centerToX = n.x - circleCenterX;
            float centerToY = n.y - circleCenterY;
            float tmpRadius = sqrt(centerToX * centerToX + centerToY * centerToY);
            if(tmpRadius > currentRadius) {
                currentRadius = tmpRadius;
            }
        }
    }
    // maly bufor do promienia grafu,
    // aby czasteczki nie pojawialy sie zbyt blisko
    float safetyCricleMargin = min({grid.width, grid.height}) * 0.1f;

    while(grid.occupiedCount() < target) {
        int px = 0;
        int py = 0;
        switch(mode) {
            // w pelni losowy spawn
            case SpawnMode::Uniform: {
                do {
                    px = randX(rng);
                    py = randY(rng);
                } while(grid.isOccupied(px, py));
                break;
            }
            // klasyczne DLA z losowej krawdzi weduja losowe czasteczki
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
            // optymalizacja czasteczki pojawiaja sie na okregu do okola grafu
            case SpawnMode::Circle: {
                // promien okregu do spawnu czasteczek
                float spawnRadius = max(1.5f * currentRadius, currentRadius + safetyCricleMargin);
                spawnRadius = min(spawnRadius, maxRadius);
                int failedAttempts = 0;
                bool goUniform = false;
                while(true) {
                    float angle = randAngle(rng);
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
        // losowe bladzenie
        while(!grid.hasOccupiedNeighbour(px, py)) {
            int directon = randDirection(rng);
            int nextX = px + dx[directon];
            int nextY = py + dy[directon];

            if(grid.inBounds(nextX, nextY)) {
                px = nextX;
                py = nextY;
            }
        }
        // czasteczka przykleja sie do pierwszego napotkanego zajetego sasiada
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
        // aktualizacja promienia skoro graf rosnie
        if(mode == SpawnMode::Circle) {
            float centerToX = px - circleCenterX;
            float centerToY = py - circleCenterY;
            float tmpRadius = sqrt(centerToX * centerToX + centerToY * centerToY);
            // jak powiekszyl sie promien to naturalnie aktualizujemy aktualny
            if(tmpRadius > currentRadius) {
                currentRadius = tmpRadius;
            }
        }
    }
    return added;
}