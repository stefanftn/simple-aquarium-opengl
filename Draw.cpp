#include "Draw.h"
#include "Scene.h"
#include "Util.h"
#include <cmath>

unsigned int basicShader;
unsigned int quadVAO;

// Svi oblici u jednom nizu: jedan VBO i jedan VAO za sve objekte
void createShapesVAO() {
    // Svaki oblik je jedinicne velicine oko (0, 0); polozaj i velicinu daju uniforme uPos i uScale.
    float vertices[(CIRCLE_FIRST + CIRCLE_COUNT) * 4] = {
        //  x      y       s     t
        -0.5f,  0.5f,   0.0f, 1.0f, // cetvorougao: gornje levo teme
        -0.5f, -0.5f,   0.0f, 0.0f, //              donje levo teme
         0.5f, -0.5f,   1.0f, 0.0f, //              donje desno teme
         0.5f,  0.5f,   1.0f, 1.0f, //              gornje desno teme
        -0.5f,  0.5f,   0.0f, 1.0f, // trougao sa vrhom desno (rep, bocno peraje)
        -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f,  0.0f,   1.0f, 0.5f,
        -0.5f, -0.5f,   0.0f, 0.0f, // trougao sa vrhom gore (peraja, dragulj)
         0.5f, -0.5f,   1.0f, 0.0f,
         0.0f,  0.5f,   0.5f, 1.0f,
    };
    // Krug (V4): centar je sidro lepeze, ostala temena su na kruznici poluprecnika 0.5
    int c0 = CIRCLE_FIRST * 4;
    vertices[c0] = 0.0f; vertices[c0 + 1] = 0.0f; vertices[c0 + 2] = 0.5f; vertices[c0 + 3] = 0.5f;
    for (int i = 1; i < CIRCLE_COUNT; i++) {
        float angle = (i - 1) * 2.0f * 3.14159265f / CIRCLE_SEGMENTS;
        int k = c0 + i * 4;
        vertices[k] = 0.5f * cos(angle);
        vertices[k + 1] = 0.5f * sin(angle);
        vertices[k + 2] = vertices[k] + 0.5f;     // teksturne koordinate (ne koriste se, ali atribut postoji)
        vertices[k + 3] = vertices[k + 1] + 0.5f;
    }

    unsigned int VBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atribut 0 (pozicija):
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atribut 1 (tekseli):
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

// Pesak: poseban VAO jer su teksturne koordinate > 1 (ponavljanje teksture)
unsigned int createSandVAO(int screenW, int screenH) {
    float sandHeightPx = (SAND_TOP + 1.0f) / 2.0f * screenH;
    float repeatX = screenW / 200.0f;      // jedna plocica peska je oko 200 piksela
    float repeatY = sandHeightPx / 200.0f;
    float sandVertices[] = {
        -0.5f,  0.5f,   0.0f,    repeatY,
        -0.5f, -0.5f,   0.0f,    0.0f,
         0.5f, -0.5f,   repeatX, 0.0f,
         0.5f,  0.5f,   repeatX, repeatY,
    };
    unsigned int sandVAO, sandVBO;
    glGenVertexArrays(1, &sandVAO);
    glGenBuffers(1, &sandVBO);
    glBindVertexArray(sandVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sandVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sandVertices), sandVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return sandVAO;
}

void preprocessTexture(unsigned& texture, const char* filepath) {
    texture = loadImageToTexture(filepath); // Učitavanje teksture
    glBindTexture(GL_TEXTURE_2D, texture); // Vezujemo se za teksturu kako bismo je podesili

    glGenerateMipmap(GL_TEXTURE_2D);

    // Strategija za wrap-ovanje: GL_REPEAT - tekstura se ponavlja kad su teksturne koordinate > 1 (potrebno za pesak)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Crta jedinicni cetvorougao sa teksturom; (x, y) je centar, (w, h) dimenzije u NDC
void drawTexture(unsigned tex, float x, float y, float w, float h, float alpha) {
    glUniform1i(glGetUniformLocation(basicShader, "useTex"), true);
    glUniform1i(glGetUniformLocation(basicShader, "gradient"), false);
    glUniform1f(glGetUniformLocation(basicShader, "uAlpha"), alpha);
    glUniform2f(glGetUniformLocation(basicShader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(basicShader, "uScale"), w, h);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// Crta jedan obojen oblik iz zajednickog VBO-a (first i count biraju oblik)
void drawShape(int first, int count, float x, float y, float w, float h, float r, float g, float b, float a, bool grad) {
    glUniform1i(glGetUniformLocation(basicShader, "useTex"), false);
    glUniform1i(glGetUniformLocation(basicShader, "gradient"), grad);
    glUniform4f(glGetUniformLocation(basicShader, "uCol"), r, g, b, a);
    glUniform2f(glGetUniformLocation(basicShader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(basicShader, "uScale"), w, h); // negativna sirina = ogledalo

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, first, count);
}

// Obojen cetvorougao
void drawColor(float x, float y, float w, float h, float r, float g, float b, float a, bool grad) {
    drawShape(QUAD_FIRST, 4, x, y, w, h, r, g, b, a, grad);
}

// Sloj preko cele sirine ekrana, izmedju zadatih visina (nebo, svetlo u vodi, kaustike...)
void drawStrip(unsigned tex, float top, float bottom, float alpha) {
    drawTexture(tex, 0.0f, (top + bottom) / 2, 2.0f, top - bottom, alpha);
}

// Ukras koji stoji na pesku: zadaje se x centra, visina i odnos stranica slike
void drawOnSand(unsigned tex, float x, float h, float ratio) {
    float w = h * ratio / aspect; // sirina se deli sa aspect da slika ne bude razvucena
    drawTexture(tex, x, SAND_TOP - SINK + h / 2, w, h);
}

// Pesak preko celog dna: isti sejder, ali VAO sa ponavljanjem teksture
void drawSand(unsigned sandVAO, unsigned sandTex) {
    glUniform1i(glGetUniformLocation(basicShader, "useTex"), true);
    glUniform1i(glGetUniformLocation(basicShader, "gradient"), false);
    glUniform1f(glGetUniformLocation(basicShader, "uAlpha"), 1.0f);
    glUniform2f(glGetUniformLocation(basicShader, "uPos"), 0.0f, (SAND_TOP - 1.0f) / 2);
    glUniform2f(glGetUniformLocation(basicShader, "uScale"), 2.0f, SAND_TOP + 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTex);
    glBindVertexArray(sandVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// Deo slozenog objekta (ribe, kovcega) zadat u lokalnim koordinatama objekta (-0.5 do 0.5).
// (cx, cy) je centar objekta, (cw, ch) njegove dimenzije; ako je cw negativno, ceo objekat je u ogledalu.
// outline = true: prvo se nacrta malo veci tamni oblik, pa preko njega obojen (kontura).
void drawPart(int first, int count, float cx, float cy, float cw, float ch,
              float lx, float ly, float lw, float lh, float r, float g, float b, bool outline) {
    float x = cx + lx * cw, y = cy + ly * ch;
    float w = lw * cw, h = lh * ch;
    if (outline) {
        float ox = 2 * OUTLINE / aspect, oy = 2 * OUTLINE;
        drawShape(first, count, x, y, w + (w < 0 ? -ox : ox), h + (h < 0 ? -oy : oy), 0.26f, 0.2f, 0.18f, 1.0f);
    }
    drawShape(first, count, x, y, w, h, r, g, b, 1.0f);
}

// Okrugao deo (oko, novcic): sirina se racuna tako da krug ne bude razvucen
void drawDot(float cx, float cy, float cw, float ch, float lx, float ly, float size,
             float r, float g, float b, bool outline) {
    float lw = size * ch / (aspect * fabs(cw));
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, cx, cy, cw, ch, lx, ly, lw, size, r, g, b, outline);
}
