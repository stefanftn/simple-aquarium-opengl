#include "Objects.h"
#include "Draw.h"
#include <cmath>

// Riba nacrtana od elipsi i trouglova; clown = true dodaje bele pruge
void drawFish(const Fish& f, bool clown) {
    float cw = f.flipped ? -f.w : f.w; // ogledalo: negativna sirina obrne sve delove oko centra
    float br = 0.98f, bg = 0.67f, bb = 0.27f;   // telo zlatne ribice
    float fr = 1.0f, fg = 0.8f, fb = 0.37f;     // peraja
    if (clown) { br = 0.96f; bg = 0.44f; bb = 0.16f; fr = 0.96f; fg = 0.5f; fb = 0.2f; }

    drawPart(TRI_RIGHT_FIRST, 3, f.x, f.y, cw, f.h, -0.33f, 0.0f, 0.34f, 0.75f, fr, fg, fb, true);  // rep
    drawPart(TRI_UP_FIRST, 3, f.x, f.y, cw, f.h, 0.02f, 0.4f, 0.34f, 0.34f, fr, fg, fb, true);      // gornje peraje
    drawPart(TRI_UP_FIRST, 3, f.x, f.y, cw, f.h, 0.08f, -0.34f, 0.2f, -0.24f, fr, fg, fb, true);    // donje peraje (vrh nadole)
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, f.x, f.y, cw, f.h, 0.1f, 0.0f, 0.8f, 0.72f, br, bg, bb, true); // telo (elipsa)

    if (clown) {
        // tri bele pruge sa tamnim ivicama; visina prati oblik elipse tela
        float stripes[3] = { -0.22f, -0.03f, 0.17f };
        for (int i = 0; i < 3; i++) {
            float k = (stripes[i] - 0.1f) / 0.4f;
            float sh = 0.7f * sqrt(1.0f - k * k);
            drawPart(CIRCLE_FIRST, CIRCLE_COUNT, f.x, f.y, cw, f.h, stripes[i], 0.0f, 0.12f, sh, 0.15f, 0.12f, 0.1f, false);
            drawPart(CIRCLE_FIRST, CIRCLE_COUNT, f.x, f.y, cw, f.h, stripes[i], 0.0f, 0.07f, sh * 0.96f, 1.0f, 1.0f, 1.0f, false);
        }
    }
    else {
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, f.x, f.y, cw, f.h, 0.06f, 0.12f, 0.46f, 0.2f, 1.0f, 0.8f, 0.5f, false); // odsjaj
    }
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, f.x, f.y, cw, f.h, -0.06f, -0.17f, 0.16f, 0.12f, fr, fg, fb, true); // bocno peraje
    drawDot(f.x, f.y, cw, f.h, 0.3f, 0.1f, 0.26f, 1.0f, 1.0f, 1.0f, true);     // oko
    drawDot(f.x, f.y, cw, f.h, 0.33f, 0.1f, 0.13f, 0.1f, 0.1f, 0.1f, false);   // zenica
    drawDot(f.x, f.y, cw, f.h, 0.31f, 0.15f, 0.05f, 1.0f, 1.0f, 1.0f, false);  // odsjaj u oku
    drawDot(f.x, f.y, cw, f.h, 0.46f, -0.1f, 0.06f, 0.26f, 0.2f, 0.18f, false); // usta
}

// Kraba nacrtana od elipsi; simetricna je i okrenuta ka posmatracu, pa nema ogledala
void drawCrab(float x, float y, float w, float h) {
    float rr = 0.89f, rg = 0.35f, rb = 0.28f;   // crvena
    float dr = 0.72f, dg = 0.22f, db = 0.19f;   // tamnija crvena (noge, ruke)
    float side[2] = { -1.0f, 1.0f };            // leva i desna strana: isti delovi, x sa suprotnim znakom
    for (int i = 0; i < 2; i++) {
        float s = side[i];
        // tri noge: uske uspravne elipse ispod tela
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, s * 0.2f, -0.3f, 0.05f, 0.34f, dr, dg, db, true);
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, s * 0.28f, -0.3f, 0.05f, 0.32f, dr, dg, db, true);
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, s * 0.36f, -0.3f, 0.05f, 0.28f, dr, dg, db, true);
        // ruka i klesta: dva trougla (otvorena klesta) iznad okruglog dela
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, s * 0.34f, 0.06f, 0.07f, 0.34f, dr, dg, db, true);
        drawPart(TRI_UP_FIRST, 3, x, y, w, h, s * 0.29f, 0.4f, 0.11f, 0.22f, rr, rg, rb, true);
        drawPart(TRI_UP_FIRST, 3, x, y, w, h, s * 0.44f, 0.4f, 0.11f, 0.22f, rr, rg, rb, true);
        drawDot(x, y, w, h, s * 0.365f, 0.27f, 0.26f, rr, rg, rb, true);
        // oci na drskama
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, s * 0.09f, 0.2f, 0.03f, 0.24f, dr, dg, db, true);
        drawDot(x, y, w, h, s * 0.09f, 0.36f, 0.16f, 1.0f, 1.0f, 1.0f, true);
        drawDot(x, y, w, h, s * 0.09f, 0.36f, 0.07f, 0.1f, 0.1f, 0.1f, false);
    }
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, 0.0f, -0.1f, 0.62f, 0.48f, rr, rg, rb, true);       // telo
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, 0.0f, -0.02f, 0.36f, 0.14f, 1.0f, 0.55f, 0.48f, false); // odsjaj
    drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, 0.0f, -0.18f, 0.1f, 0.04f, 0.26f, 0.2f, 0.18f, false);  // usta
}

// Kovceg nacrtan od cetvorouglova, elipsi i trouglova; (x, bottom) je sredina donje ivice
void drawChest(float x, float bottom, float h, bool open) {
    float w = h * CHEST_RATIO / aspect;
    float y = bottom + h / 2;
    float wr = 0.62f, wg = 0.38f, wb = 0.2f;     // drvo
    float dr = 0.48f, dg = 0.28f, db = 0.14f;    // tamnije drvo
    float gr = 0.98f, gg = 0.78f, gb = 0.2f;     // zlato

    if (!open) {
        drawPart(CIRCLE_FIRST, CIRCLE_COUNT, x, y, w, h, 0.0f, 0.48f, 1.0f, 0.24f, dr, dg, db, true); // zaobljen vrh poklopca
        drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, 0.32f, 1.0f, 0.36f, dr, dg, db, true);              // poklopac
    }
    else {
        // otvoren poklopac (iza), sa tamnom unutrasnjoscu i zlatnom ivicom
        drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, 0.52f, 0.96f, 0.44f, dr, dg, db, true);
        drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, 0.5f, 0.82f, 0.32f, 0.25f, 0.14f, 0.08f, false);
        drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, 0.74f, 1.0f, 0.07f, gr, gg, gb, true);
        // blago: novcici (zlatni krugovi) i dragulj (dva trougla)
        float coinX[5] = { -0.28f, -0.1f, 0.12f, 0.3f, 0.0f };
        float coinY[5] = { 0.27f, 0.32f, 0.3f, 0.26f, 0.38f };
        for (int i = 0; i < 5; i++) {
            drawDot(x, y, w, h, coinX[i], coinY[i], 0.2f, gr, gg, gb, true);
            drawDot(x, y, w, h, coinX[i], coinY[i], 0.1f, 1.0f, 0.88f, 0.45f, false);
        }
        drawPart(TRI_UP_FIRST, 3, x, y, w, h, 0.02f, 0.58f, 0.26f, 0.12f, 0.4f, 0.85f, 1.0f, true);   // gornji deo dragulja
        drawPart(TRI_UP_FIRST, 3, x, y, w, h, 0.02f, 0.4f, 0.26f, -0.24f, 0.25f, 0.65f, 0.95f, true); // donji deo (vrh nadole)
    }
    // telo kovcega sa daskom, zlatnim trakama i bravom
    drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, -0.18f, 1.0f, 0.64f, wr, wg, wb, true);
    drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, -0.24f, 0.98f, 0.03f, dr, dg, db, false);
    drawPart(QUAD_FIRST, 4, x, y, w, h, -0.36f, -0.18f, 0.09f, 0.64f, gr, gg, gb, true);
    drawPart(QUAD_FIRST, 4, x, y, w, h, 0.36f, -0.18f, 0.09f, 0.64f, gr, gg, gb, true);
    drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, 0.14f, 1.06f, 0.1f, gr, gg, gb, true);
    drawPart(QUAD_FIRST, 4, x, y, w, h, 0.0f, -0.04f, 0.16f, 0.24f, gr, gg, gb, true);
    drawDot(x, y, w, h, 0.0f, -0.03f, 0.07f, 0.26f, 0.2f, 0.18f, false); // kljucaonica
}
