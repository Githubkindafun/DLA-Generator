#pragma once
#include <bits/stdc++.h>

namespace config {
    constexpr int START_SIZE = 8;
    constexpr float FILL_RATIO = 0.25f;
    constexpr float COMBINE_ALPHA = 2.5f;
    constexpr int ITERATIONS = 8;
    constexpr float BLURE_FRACTION = 0.018f;
    constexpr float CRISP_PREBLURE_FRACTION = 0.005f;
    constexpr int CIRCLE_FROM_ITER = 5;
    constexpr uint32_t SEED = 420;
    // from heightmap.cpp
    // every node extands RIDGE_RADIUS in every direction (looks better)
    constexpr int RIDGE_RADIUS = 5;
    // for some optimalisation
    constexpr float INV_R2 = 1.0f / (RIDGE_RADIUS * RIDGE_RADIUS);
}