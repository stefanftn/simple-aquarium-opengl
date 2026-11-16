#pragma once
#include <GL/glew.h>

// Crtanje: zajednicki VAO sa jedinicnim oblicima, teksture i pomocne funkcije za slozene objekte

// Oblici u zajednickom VBO-u: biraju se prvim temenom (first) i brojem temena u glDrawArrays
const int CIRCLE_SEGMENTS = 40;
const int QUAD_FIRST = 0;        // cetvorougao: 4 temena
const int TRI_RIGHT_FIRST = 4;   // trougao sa vrhom desno: 3 temena
const int TRI_UP_FIRST = 7;      // trougao sa vrhom gore: 3 temena
const int CIRCLE_FIRST = 10;     // krug (elipsa): centar + CIRCLE_SEGMENTS + 1 temena
const int CIRCLE_COUNT = CIRCLE_SEGMENTS + 2;

const float OUTLINE = 0.006f;    // debljina tamne konture oblika (NDC po y)

extern unsigned int basicShader; // jedini sejder program
extern unsigned int quadVAO;     // jedan VAO sa svim oblicima (cetvorougao, trouglovi, krug)

// Pravljenje VAO-a (poziva se jednom, posle glewInit)
void createShapesVAO();
unsigned int createSandVAO(int screenW, int screenH); // pesak: teksturne koordinate > 1 (ponavljanje)

void preprocessTexture(unsigned& texture, const char* filepath);

// Osnovno crtanje
void drawTexture(unsigned tex, float x, float y, float w, float h, float alpha = 1.0f);
void drawShape(int first, int count, float x, float y, float w, float h, float r, float g, float b, float a, bool grad = false);
void drawColor(float x, float y, float w, float h, float r, float g, float b, float a, bool grad = false);
void drawStrip(unsigned tex, float top, float bottom, float alpha = 1.0f);
void drawOnSand(unsigned tex, float x, float h, float ratio);
void drawSand(unsigned sandVAO, unsigned sandTex);

// Delovi slozenih objekata u lokalnim koordinatama objekta (-0.5 do 0.5)
void drawPart(int first, int count, float cx, float cy, float cw, float ch,
              float lx, float ly, float lw, float lh, float r, float g, float b, bool outline);
void drawDot(float cx, float cy, float cw, float ch, float lx, float ly, float size,
             float r, float g, float b, bool outline);
