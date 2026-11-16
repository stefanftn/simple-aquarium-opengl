#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

// Scena akvarijuma: strukture, konstante, stanje objekata i logika kretanja (bez crtanja)

// ---------------- Strukture ----------------
struct Fish {
    float x, y;     // centar ribe (NDC)
    float w, h;     // sirina i visina poligona (NDC)
    bool flipped;   // true - riba gleda ulevo (crta se u ogledalu)
};

struct Bubble {
    float x, y;     // centar mehurica
    float size;     // visina mehurica (NDC)
    unsigned tex;   // jedna od tri teksture mehurica
};

struct Food {
    float x, y;     // centar cestice
    bool onSand;    // true - cestica je pala na pesak i stoji
};

struct Crab {
    float x;        // centar krabe po x
    float w, h;     // sirina i visina
    bool jumping;   // da li je skok u toku
    double jumpStart; // vreme pocetka skoka
    float jumpY;    // trenutna visina skoka iznad peska
};

// ---------------- Konstante scene (NDC) ----------------
const float WATER_TOP = 0.3f;    // povrsina vode: akvarijum zauzima 65% visine ekrana
const float SAND_TOP = -0.68f;   // gornja ivica peska
const float SINK = 0.03f;        // koliko su ukrasi "utonuli" u pesak
const float FOOD_SIZE = 0.018f;  // velicina cestice hrane
const float FOOD_SPEED = 0.25f;  // brzina padanja hrane (NDC u sekundi)
const float FISH_SPEED = 0.6f;   // brzina riba (NDC u sekundi)
const float CRAB_SPEED = 0.5f;   // brzina krabe
const float BUBBLE_SPEED = 0.35f;// brzina mehurica
const float G = 4.0f;            // gravitacija za skok krabe
const float JUMP_H = 0.45f;      // maksimalna visina skoka

// ---------------- Stanje scene (definisano u Scene.cpp) ----------------
extern float aspect;             // sirina / visina ekrana
extern float borderX, borderY;   // 10 piksela preracunato u NDC
extern Fish goldfish;
extern Fish clownfish;
extern Crab crab;
extern bool chestOpen;
extern std::vector<Bubble> bubbles;
extern std::vector<Food> foods;
extern unsigned bubbleTex[3];

// ---------------- Funkcije ----------------
void initScene(float fishRatio, float crabRatio); // pocetni polozaji i velicine objekata
void spawnBubbles(Fish& f);                         // tri mehurica kod usta ribe
void spawnFood();                                   // 5 do 8 cestica iznad ekrana
void startCrabJump();                               // Space: skok ako prethodni nije u toku
void moveFish(GLFWwindow* window, Fish& f, int up, int down, int left, int right, float dt);
void moveCrab(GLFWwindow* window, float dt);        // tasteri 1 i 2
void updateCrabJump();                              // s = sqrt(2gh) * t - g * t^2 / 2
void updateBubbles(float dt);                       // mehurici nagore, brisanje iznad povrsine
void updateFood(float dt);                          // padanje do peska
void eatFood();                                     // AABB sudari i rast za 0.01
float crabCenterY();                                // centar krabe po y (crta se od dna, pa raste nagore)
bool overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void keepFishInWater(Fish& f);
