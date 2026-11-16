#pragma once
#include "Scene.h"

// Objekti nacrtani kodom (od oblika iz zajednickog VAO-a): ribe, kraba i kovceg

const float FISH_RATIO = 1.65f;  // sirina / visina ribe (u pikselima)
const float CRAB_RATIO = 1.4f;   // sirina / visina krabe
const float CHEST_RATIO = 1.3f;  // sirina / visina kovcega
const float CHEST_H = 0.26f;     // visina kovcega (NDC)

void drawFish(const Fish& f, bool clown);              // clown = true dodaje bele pruge
void drawCrab(float x, float y, float w, float h);     // simetricna, bez ogledala
void drawChest(float x, float bottom, float h, bool open);
