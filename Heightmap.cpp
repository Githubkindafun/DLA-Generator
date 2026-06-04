#include <bits/stdc++.h>
#include "Heightmap.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "config.hpp"

using namespace std;

float Heightmap::sample(float x, float y) {
    // clamp to valid range
    x = clamp(x, 0.0f, (float)(width - 1));
    y = clamp(y, 0.0f, (float)(height - 1));

    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = min(x0 + 1, width - 1);
    int y1 = min(y0 + 1, height - 1);

    float px = x - x0;
    float py = y - y0;
    // returns the blend from 4 directions
    return (1 - px) * (1 - py) * get(x0, y0) +
            px * (1 - py) * get(x1, y0) +
            (1 - px) * py * get(x0, y1) +
            px * py * get(x1, y1); 
}

Heightmap Heightmap::linearInterpolationResize(int newWidth, int newHeight) {
    Heightmap newHeightmap(newWidth, newHeight);
    float scaleX = (float)(width - 1) / (float)(newWidth - 1);
    float scaleY = (float)(height - 1) / (float)(newHeight - 1);
    for(int x = 0; x < newWidth; x++) {
        for(int y = 0; y < newHeight; y++) {
            newHeightmap.set(x, y, sample(x * scaleX, y * scaleY));
        }
    }
    return newHeightmap;
}

Heightmap Heightmap::gaussianBlure(float sigma) {
    int radius = (int)(ceil(3.0f * sigma));
    int kernelSize = 2 * radius + 1;
    vector<float> kernel(kernelSize);
    float kernelSum = 0.0f;
    for(int i = 0; i < kernelSize; i++) {
        float x = (float)(i - radius);
        kernel[i] = exp(-0.5 * x * x / (sigma * sigma));
        kernelSum += kernel[i];
    }
    for(float& k : kernel) {
        k /= kernelSum;
    }
    Heightmap tmp(width, height);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            float value = 0.0f;
            for(int k = 0; k < kernelSize; k++) {
                value += kernel[k] * get(clamp(x + k - radius, 0, width - 1), y);
            }
            tmp.set(x, y, value);
        }
    }
    Heightmap result(width, height);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            float value = 0.0f;
            for(int k = 0; k < kernelSize; k++) {
                value += kernel[k] * tmp.get(x, clamp(y + k - radius, 0, height - 1));
            }
            result.set(x, y, value);
        }
    }
    return result;
}

void Heightmap::renderFromGraph(Graph& g) {
    fill(data.begin(), data.end(), 0.0f);
    auto plotDome = [&](int domeCenterX, int domeCenterY, float h) {
        for(int dy = -config::RIDGE_RADIUS; dy <= config::RIDGE_RADIUS; dy++) {
            for(int dx = -config::RIDGE_RADIUS; dx <= config::RIDGE_RADIUS; dx++) {
                int x = domeCenterX + dx;
                int y = domeCenterY + dy;
                if(x < 0 || x >= width || y < 0 || y >= height) {
                    continue;
                }
                float r2 = (float)(dx * dx + dy * dy);
                if(r2 > config::RIDGE_RADIUS * config::RIDGE_RADIUS) {
                    continue;
                }
                float value = h * (1.0f - r2 * config::INV_R2);
                if(value > get(x, y)) {
                    set(x, y, value);
                }
            }
        }
    };
    auto line = [&](int x0, int y0, float h0, int x1, int y1, float h1) {
        int steps = max(abs(x1 - x0), abs(y1 - y0));
        if(steps == 0) {
            plotDome(x0, y0, h0);
            return;
        }
        for(int i = 0; i <= steps; i++) {
            float t = (float)(i) / (float)(steps);
            int x = (int)(round(x0 + t * (x1 - x0)));
            int y = (int)(round(y0 + t * (y1 - y0)));
            float h = h0 + t * (h1 - h0);
            plotDome(x, y, h);
        }
    };
    for(Node& node : g.nodes) {
        plotDome(node.x, node.y, node.height);
        if(node.parent >= 0) {
            Node& p = g.nodes[node.parent];
            line(node.x, node.y, node.height, p.x, p.y, p.height);
        }
    }
}

Heightmap Heightmap::combine(Heightmap& crisp, Heightmap& blure, float alpha) {
    Heightmap result(crisp.width, crisp.height);
    for(int i = 0; i < (int)(result.data.size()); i++) {
        result.data[i] = crisp.data[i] + alpha * blure.data[i];
    }
    return result;
}

void Heightmap::normalize() {
    if(data.empty()) {
        return;
    }
    vector<float> sorted(data);
    sort(sorted.begin(), sorted.end());

    size_t lowIdx = (size_t)(sorted.size() * 0.01);
    size_t highIdx = (size_t)(sorted.size() * 0.99);
    float mn = sorted[lowIdx];
    float mx = sorted[highIdx];
    if(mx - mn < 1e-6f) {
        return;
    }
    float inv = 1.0f / (mx - mn);
    for(float& value : data) {
        value = (value - mn) * inv;
    }
}

void Heightmap::savePNG(const string& path) {
    float vmax = 1.0f;
    for(float v : data) {
        if(v > vmax) {
            vmax = v;
        }
    }
    float inv_max = 1.0f / vmax;
    vector<uint8_t> pixels(width * height);
    for(int i = 0; i < (int)(data.size()); i++) {
        pixels[i] = (uint8_t)(pow(clamp(data[i] * inv_max, 0.0f, 1.0f), 0.4f) * 255.0f);
    }
    stbi_write_png(path.c_str(), width, height, 1, pixels.data(), width);
}
