#pragma once
#include<bits/stdc++.h>

namespace config {
    // szerokosc x wysokosc startowego grida
    constexpr int START_SIZE = 8;
    // liczba podwojen rozdielczosci
    constexpr int ITERATIONS = 8;
    // steruje rozmyciem czesci blure (im wieksza wartosc tym bardziej rozmyte)
    constexpr float BLURE_FRACTION = 0.018f;
    // delikatne rozmycie czesci crisp
    constexpr float CRISP_PREBLURE_FRACTION = 0.005f;
    // od ktorej iteracji przelacza tworzenie czasteczek na tryb Circle
    constexpr int CIRCLE_FROM_ITER = 5;
    // ziarno dla generatora (dla = 0 losowe ziarno)
    constexpr uint32_t SEED = 420;
    // stopien wypelnienia obszaru czasteczkami (na koniec kazdej iteracji)
    constexpr float FILL_RATIO = 0.40f;
    // waga czesci blure przy combine H = H_crisp + COMBINE_ALPHA * H_blure
    constexpr float COMBINE_ALPHA = 2.5f;
    // promien kopoly stawianej na kazdym wierzcholku w renderFromGraph
    constexpr int RIDGE_RADIUS = 5;
    // eksportowanie do .obj
    constexpr bool EXPORT_TO_OBJ = true;
    // przeskalowanie wysokosci z [0,1] do [0,OBJ_HEIGHT_SCALE] (dla .obj)
    constexpr float OBJ_HEIGHT_SCALE = 750.0f;
}