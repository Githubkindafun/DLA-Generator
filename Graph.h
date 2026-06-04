#pragma once
#include <bits/stdc++.h>

using namespace std;

struct Node {
    int x;
    int y;
    // in this context this is the index
    // of the node to which this stick to
    int parent = -1;
    // height of each node
    float height = 0.0f;
    vector<int> children;
};

class Graph {
private:

public:
    vector<Node> nodes;
    int root = -1;

    int addNode(int x, int y, int parent) {
        int index = (int)(nodes.size());
        Node& n = nodes.emplace_back();
        n.x = x;
        n.y = y;
        n.parent = parent;
        if(parent >= 0) {
            nodes[parent].children.push_back(index);
        }
        if(root < 0) {
            root = index;
        }
        return index;
    }

    // alright this is one of the most important parts of the whole code
    // here we calculate the height for each node after each resize + new detail
    // it is done in sort of phases:
    // 1. bfs from root to max depth (new detail usually)
    // 2. sort the nodes so we can iterate depsest first
    // 3. we compue the raw height for each node 
    // 4. we normalize the height
    void computeHeights() {
        if(root < 0 || nodes.empty()) {
            return;
        }
        int n = (int)(nodes.size());
        // part 1.
        // BFS -> we calculate how far from root is each node/particle
        vector<int> depthFromRoot(n, 0);
        queue<int> que;
        que.push(root);
        while(!que.empty()) {
            int index = que.front();
            que.pop();
            for(int child : nodes[index].children) {
                depthFromRoot[child] = depthFromRoot[index] + 1;
                que.push(child);
            }
        }
        // part 2.
        // we create "order" and using depthFromRoot we create an order
        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(),
            [&](int a, int b) {return depthFromRoot[a] > depthFromRoot[b];});
        
        // part 3.
        // now we march from bottom up calculating the height for each node
        vector<int> heightRaw(n, 0);
        for(int i : order) {
            if(nodes[i].children.empty()) {
                heightRaw[i] = 1;
            } else {
                int best = 0;
                for(int child : nodes[i].children) {
                    best = max(best, heightRaw[child]);
                }
                heightRaw[i] = 1 + best;
            }
        }
        // part 4.
        // normalize the height and apply the curve
        int heightMax = 0;
        for(int i = 0; i < n; i++) {
            heightMax = max(heightMax, heightRaw[i]);
        }
        float invHeightMax;
        if(heightMax > 0) {
            invHeightMax = 1.0f / (float)(heightMax);
        } else {
            invHeightMax = 1.0f;
        }
        for(int i = 0; i < n; i++) {
            float height = (float)(heightRaw[i]) * invHeightMax;
            // formula from video -> something fishy is here
            nodes[i].height = 1.0f - 1.0f / (1.0f + height);
            // nodes[i].height = powf(2.0f - 2.0f / (1.0f + height), 1.5f) / 2;
            // nodes[i].height = height;
            // nodes[i].height = powf(2.0f - 2.0f / (1.0f + height), 2.5f);
        }
    }
    
    // one of the "big phases" of the whole algorithm in short  by crisp resize i mean 
    // doubling the coordinates of  the nodes and adding the in between nodes
    void crispResize(int newGriWidth, int newGridHeight, int jiggleRadius, mt19937& rng) {
        nodes.reserve(nodes.size() * 2 + 1);
        // (1) first we need to double all of the coordinates
        for(Node& node : nodes) {
            node.x *= 2;
            node.y *= 2;
        }
        // (2) for each now longer edge we add a midpoint jiggled a bit to avoid 
        // unnatural shapes :)
        int originalNodesCount = (int)(nodes.size());
        // jiggle in questiotn
        uniform_int_distribution<int> jiggle(-jiggleRadius, jiggleRadius);
        for(int i = 0; i < originalNodesCount; i++) {
            int parent = nodes[i].parent;
            if(parent < 0) {
                continue;
            }
            int middleX = (nodes[i].x + nodes[parent].x) / 2 + jiggle(rng);
            int middleY = (nodes[i].y + nodes[parent].y) / 2 + jiggle(rng);
            middleX = clamp(middleX, 0, newGriWidth - 1);
            middleY = clamp(middleY, 0, newGridHeight - 1);
            int midIndex = (int)(nodes.size());
            // and now we pack the new guys in
            auto& parentChildred = nodes[parent].children;
            replace(parentChildred.begin(), parentChildred.end(), i, midIndex);
            nodes[i].parent = midIndex;

            Node mid;
            mid.x = middleX;
            mid.y = middleY;
            mid.parent = parent;
            mid.children.push_back(i);
            nodes.push_back(move(mid));
        }
    }
};