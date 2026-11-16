#include "Scene.h"
#include <cmath>
#include <cstdlib>

// ---------------- Stanje scene ----------------
float aspect = 16.0f / 9.0f;  // postavlja se iz rezolucije u main
float borderX, borderY;
Fish goldfish;
Fish clownfish;
Crab crab;
bool chestOpen = false;
std::vector<Bubble> bubbles;
std::vector<Food> foods;
unsigned bubbleTex[3];

// Pocetni polozaji i velicine (poziva se kad je aspect vec poznat)
void initScene(float fishRatio, float crabRatio) {
    goldfish.h = 0.26f;
    goldfish.w = goldfish.h * fishRatio / aspect;
    goldfish.x = -0.45f; goldfish.y = -0.05f;
    goldfish.flipped = false;

    clownfish.h = 0.24f;
    clownfish.w = clownfish.h * fishRatio / aspect;
    clownfish.x = 0.4f; clownfish.y = -0.25f;
    clownfish.flipped = true;

    crab.h = 0.22f;
    crab.w = crab.h * crabRatio / aspect;
    crab.x = -0.1f;
    crab.jumping = false;
    crab.jumpStart = 0.0;
    crab.jumpY = 0.0f;
}

// Mehurici nastaju kod usta ribe; usta su desno ili levo u zavisnosti od smera
void spawnBubbles(Fish& f) {
    float mouthX = f.flipped ? f.x - f.w * 0.45f : f.x + f.w * 0.45f;
    float mouthY = f.y + f.h * 0.05f;
    for (int i = 0; i < 3; i++) {
        Bubble b;
        b.size = 0.035f + 0.01f * i;
        b.x = mouthX + (i % 2 == 0 ? 0.0f : 0.01f);
        b.y = mouthY + i * 0.06f; // jedan iznad drugog
        b.tex = bubbleTex[i];
        bubbles.push_back(b);
    }
}

// Hrana se stvara iznad ekrana (y > 1) na nasumicnim x pozicijama unutar akvarijuma
void spawnFood() {
    int count = 5 + rand() % 4; // 5 do 8 cestica
    float minX = -1.0f + borderX + 0.05f;
    float maxX = 1.0f - borderX - 0.05f;
    for (int i = 0; i < count; i++) {
        Food f;
        f.x = minX + (maxX - minX) * (rand() / (float)RAND_MAX);
        f.y = 1.05f + 0.3f * (rand() / (float)RAND_MAX);
        f.onSand = false;
        foods.push_back(f);
    }
}

// Space: novi skok tek kad se prethodni zavrsi
void startCrabJump() {
    if (crab.jumping) return;
    crab.jumping = true;
    crab.jumpStart = glfwGetTime();
}

// Preklapanje dva pravougaonika (AABB) zadata centrom i dimenzijama
bool overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return fabs(x1 - x2) * 2 < (w1 + w2) && fabs(y1 - y2) * 2 < (h1 + h2);
}

// Riba ne sme da izadje iz vode (ivice, povrsina, pesak)
void keepFishInWater(Fish& f) {
    float minX = -1.0f + borderX + f.w / 2;
    float maxX = 1.0f - borderX - f.w / 2;
    float minY = SAND_TOP + f.h / 2;
    float maxY = WATER_TOP - f.h / 2;
    if (f.x < minX) f.x = minX;
    if (f.x > maxX) f.x = maxX;
    if (f.y < minY) f.y = minY;
    if (f.y > maxY) f.y = maxY;
}

// Kretanje ribe: kad se promeni smer levo-desno, menja se flipped (ogledalo)
void moveFish(GLFWwindow* window, Fish& f, int up, int down, int left, int right, float dt) {
    float dist = FISH_SPEED * dt;
    if (glfwGetKey(window, up) == GLFW_PRESS) f.y += dist;
    if (glfwGetKey(window, down) == GLFW_PRESS) f.y -= dist;
    if (glfwGetKey(window, left) == GLFW_PRESS) {
        f.x -= dist / aspect; // deljenje sa aspect: ista brzina u pikselima po x i y
        f.flipped = true;     // riba je nacrtana okrenuta udesno, pa je za levo potrebno ogledalo
    }
    if (glfwGetKey(window, right) == GLFW_PRESS) {
        f.x += dist / aspect;
        f.flipped = false;
    }
    // gore-dole ne menja flipped, pa riba zadrzava poslednji smer
    keepFishInWater(f);
}

// Kraba levo-desno po pesku (tasteri 1 i 2), u granicama akvarijuma
void moveCrab(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) crab.x -= CRAB_SPEED * dt / aspect;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) crab.x += CRAB_SPEED * dt / aspect;
    float minX = -1.0f + borderX + crab.w / 2;
    float maxX = 1.0f - borderX - crab.w / 2;
    if (crab.x < minX) crab.x = minX;
    if (crab.x > maxX) crab.x = maxX;
}

// Skok krabe: s = sqrt(2gh) * t - g * t^2 / 2
void updateCrabJump() {
    if (!crab.jumping) return;
    float t = (float)(glfwGetTime() - crab.jumpStart);
    crab.jumpY = sqrt(2 * G * JUMP_H) * t - G * t * t / 2;
    if (crab.jumpY <= 0.0f) { // skok je zavrsen, kraba je tacno na pesku
        crab.jumpY = 0.0f;
        crab.jumping = false;
    }
}

// Centar krabe po y: racuna se od donje ivice, pa kraba raste nagore i ostaje na pesku
float crabCenterY() {
    return SAND_TOP - SINK + crab.jumpY + crab.h / 2;
}

// Mehurici idu ka povrsini i brisu se kad je predju (petlja unazad zbog erase)
void updateBubbles(float dt) {
    for (int i = (int)bubbles.size() - 1; i >= 0; i--) {
        bubbles[i].y += BUBBLE_SPEED * dt;
        if (bubbles[i].y > WATER_TOP) bubbles.erase(bubbles.begin() + i);
    }
}

// Hrana pada konstantnom brzinom i zaustavlja se na gornjoj ivici peska
void updateFood(float dt) {
    for (int i = 0; i < (int)foods.size(); i++) {
        if (!foods[i].onSand) {
            foods[i].y -= FOOD_SPEED * dt;
            if (foods[i].y - FOOD_SIZE / 2 <= SAND_TOP) {
                foods[i].y = SAND_TOP + FOOD_SIZE / 2;
                foods[i].onSand = true;
            }
        }
    }
}

// Jedenje (AABB): ribe samo cestice koje padaju, kraba i one na pesku; pojedena cestica nestaje
void eatFood() {
    float foodW = FOOD_SIZE / aspect;
    float crabY = crabCenterY();
    for (int i = (int)foods.size() - 1; i >= 0; i--) {
        Food& f = foods[i];
        bool eaten = false;
        if (!f.onSand) {
            if (overlap(goldfish.x, goldfish.y, goldfish.w, goldfish.h, f.x, f.y, foodW, FOOD_SIZE)) {
                goldfish.h += 0.01f;
                eaten = true;
            }
            else if (overlap(clownfish.x, clownfish.y, clownfish.w, clownfish.h, f.x, f.y, foodW, FOOD_SIZE)) {
                clownfish.h += 0.01f;
                eaten = true;
            }
        }
        if (!eaten && overlap(crab.x, crabY, crab.w, crab.h, f.x, f.y, foodW, FOOD_SIZE)) {
            crab.h += 0.01f;
            eaten = true;
        }
        if (eaten) foods.erase(foods.begin() + i);
    }
    keepFishInWater(goldfish);  // riba je mozda porasla, ponovo je drzimo u vodi
    keepFishInWater(clownfish);
}
