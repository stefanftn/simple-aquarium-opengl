#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Util.h"
#include "Scene.h"
#include "Draw.h"
#include "Objects.h"

// Main.cpp: prozor, ucitavanje tekstura, tastatura, glavna petlja i redosled crtanja scene.
// Logika scene je u Scene.cpp, crtanje oblika u Draw.cpp, a ribe, kraba i kovceg u Objects.cpp.

const double FRAME_TIME = 1.0 / 75.0; // trajanje jednog frejma za 75 FPS

// Odnos sirine i visine slika (sirina / visina u pikselima) - da se teksture ne razvlace
const float SEAWEED1_RATIO = 338.0f / 269.0f;
const float SEAWEED2_RATIO = 235.0f / 293.0f;
const float STONE1_RATIO = 280.0f / 375.0f;
const float STONE3_RATIO = 203.0f / 97.0f;
const float STONE6_RATIO = 406.0f / 330.0f;
const float BARREL_RATIO = 264.0f / 212.0f;
const float NAME_RATIO = 480.0f / 140.0f;

// Tema "zalazak sunca": sunce je na horizontu (povrsini vode)
const float SUN_X = 0.23f;       // x centra sunca (NDC)
const float SUN_SIZE = 1.67f;    // visina slike sunca sa sjajem (NDC)

// Sve teksture scene na jednom mestu
struct Textures {
    unsigned sand, seaweed, coral, stone1, stone3, stone6, barrel, name;
    unsigned sky, sun, farReef, waterLight, caustics, vignette, chestGlow;
};

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

void loadTextures(Textures& t) {
    preprocessTexture(t.sand, "res/Sand.png");
    preprocessTexture(t.seaweed, "res/Seaweed_1.png");
    preprocessTexture(t.coral, "res/Seaweed_2.png");
    preprocessTexture(t.stone1, "res/Stone_1.png");
    preprocessTexture(t.stone3, "res/Stone_3.png");
    preprocessTexture(t.stone6, "res/Stone_6.png");
    preprocessTexture(t.barrel, "res/Barrel_2.png");
    preprocessTexture(t.name, "res/NameTag.png");
    // slojevi teme "zalazak sunca"
    preprocessTexture(t.sky, "res/Sky.png");
    preprocessTexture(t.sun, "res/Sun.png");
    preprocessTexture(t.farReef, "res/FarReef.png");
    preprocessTexture(t.waterLight, "res/WaterLight.png");
    preprocessTexture(t.caustics, "res/Caustics.png");
    preprocessTexture(t.vignette, "res/Vignette.png");
    preprocessTexture(t.chestGlow, "res/ChestGlow.png");
    preprocessTexture(bubbleTex[0], "res/Bubble_1.png");
    preprocessTexture(bubbleTex[1], "res/Bubble_2.png");
    preprocessTexture(bubbleTex[2], "res/Bubble_3.png");
}

// Akcije na jedan pritisak tastera
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_C) chestOpen = !chestOpen;
    if (key == GLFW_KEY_Z) spawnBubbles(goldfish);
    if (key == GLFW_KEY_K) spawnBubbles(clownfish);
    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) spawnFood();
    if (key == GLFW_KEY_SPACE) startCrabJump();
}

// Crtanje cele scene, od pozadine ka posmatracu (kasnije nacrtano je ispred)
void drawScene(const Textures& t, unsigned sandVAO) {
    float aquaH = WATER_TOP + 1.0f;          // visina akvarijuma u NDC
    float aquaY = (WATER_TOP - 1.0f) / 2;    // centar akvarijuma po y

    // Nebo sa oblacima iznad vode
    drawStrip(t.sky, 1.0f, WATER_TOP);
    // Sunce na horizontu: donju polovinu prekriva voda koja se crta posle
    drawTexture(t.sun, SUN_X, WATER_TOP + 0.02f, SUN_SIZE / aspect, SUN_SIZE);

    // Voda (ljubicasto-plava, blagi gradijent)
    drawColor(0.0f, aquaY, 2.0f, aquaH, 0.35f, 0.43f, 0.67f, 1.0f, true);
    // Udaljeni greben (zamucene siluete) i svetlo u vodi (zraci iz sunca, odsjaji, cestice)
    drawStrip(t.farReef, WATER_TOP, SAND_TOP);
    drawStrip(t.waterLight, WATER_TOP, SAND_TOP);

    // Pesak (poseban VAO sa ponavljanjem teksture)
    drawSand(sandVAO, t.sand);
    // Topla senka preko peska, pa kaustike (svetle sare od talasa)
    drawColor(0.0f, (SAND_TOP - 1.0f) / 2, 2.0f, SAND_TOP + 1.0f, 0.31f, 0.2f, 0.35f, 0.3f);
    drawStrip(t.caustics, SAND_TOP, -1.0f);

    // Ukrasi i morska trava (trava raste iz peska)
    drawOnSand(t.stone1, -0.88f, 0.32f, STONE1_RATIO);
    drawOnSand(t.seaweed, -0.55f, 0.32f, SEAWEED1_RATIO);
    drawOnSand(t.coral, -0.28f, 0.3f, SEAWEED2_RATIO);
    drawOnSand(t.stone3, 0.0f, 0.07f, STONE3_RATIO);
    drawOnSand(t.seaweed, 0.6f, 0.28f, SEAWEED1_RATIO);
    drawOnSand(t.barrel, 0.7f, 0.12f, BARREL_RATIO);
    drawOnSand(t.stone6, 0.86f, 0.22f, STONE6_RATIO);

    // Kovceg sa sadrzajem (nacrtan kodom)
    float chestX = 0.25f;
    float chestBottom = SAND_TOP - SINK;
    if (chestOpen) {
        // zlatna svetlost iz otvorenog kovcega: veliki sjaj iza kovcega
        float glowH = CHEST_H * 3.4f;
        drawTexture(t.chestGlow, chestX, chestBottom + CHEST_H * 0.7f, glowH / aspect, glowH, 1.0f);
    }
    drawChest(chestX, chestBottom, CHEST_H, chestOpen);
    if (chestOpen) {
        // manji sjaj ispred blaga, kao da svetlost izlazi iz kovcega
        float glow2H = CHEST_H * 1.7f;
        drawTexture(t.chestGlow, chestX, chestBottom + CHEST_H * 0.78f, glow2H / aspect, glow2H, 0.85f);
    }

    // Hrana (mali obojeni cetvorouglovi)
    float foodW = FOOD_SIZE / aspect;
    for (int i = 0; i < (int)foods.size(); i++) {
        drawColor(foods[i].x, foods[i].y, foodW, FOOD_SIZE, 0.55f, 0.33f, 0.18f, 1.0f);
    }

    // Kraba (nacrtana kodom, nema ogledala)
    drawCrab(crab.x, crabCenterY(), crab.w, crab.h);

    // Ribe (nacrtane kodom)
    drawFish(goldfish, false);
    drawFish(clownfish, true);

    // Mehurici
    for (int i = 0; i < (int)bubbles.size(); i++) {
        Bubble& b = bubbles[i];
        drawTexture(b.tex, b.x, b.y, b.size / aspect, b.size);
    }

    // Zatamnjenje ivica akvarijuma (dubina)
    drawStrip(t.vignette, WATER_TOP, -1.0f);

    // Beli cetvorougao providnosti 0.2 preko celog akvarijuma
    drawColor(0.0f, aquaY, 2.0f, aquaH, 1.0f, 1.0f, 1.0f, 0.2f);

    // Crne ivice debljine 10 piksela (pravougaonici): leva, desna, donja
    drawColor(-1.0f + borderX / 2, aquaY, borderX, aquaH, 0.0f, 0.0f, 0.0f, 1.0f);
    drawColor(1.0f - borderX / 2, aquaY, borderX, aquaH, 0.0f, 0.0f, 0.0f, 1.0f);
    drawColor(0.0f, -1.0f + borderY / 2, 2.0f, borderY, 0.0f, 0.0f, 0.0f, 1.0f);

    // Poluprovidni natpis sa imenom u gornjem desnom uglu
    float nameH = 0.2f;
    float nameW = nameH * NAME_RATIO / aspect;
    drawTexture(t.name, 1.0f - 0.03f - nameW / 2, 1.0f - 0.05f - nameH / 2, nameW, nameH, 0.75f);
}

int main()
{
    srand((unsigned)time(NULL));

    // Inicijalizacija GLFW i postavljanje na verziju 3 sa programabilnim pajplajnom
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Full screen: prozor dobija monitor i njegovu nativnu rezoluciju
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Akvarijum", monitor, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // iskljucen V-Sync, FPS ogranicava nas frame limiter

    glfwSetKeyCallback(window, keyboardCallback);

    // Kursor sa slikom sidra
    GLFWcursor* cursor = loadImageToCursor("res/cursor.png");
    if (cursor != nullptr) glfwSetCursor(window, cursor);

    // Inicijalizacija GLEW
    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    // Viewport preko celog ekrana i preracunavanje 10 piksela u NDC
    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    glViewport(0, 0, screenW, screenH);
    aspect = (float)screenW / screenH;
    borderX = 10.0f / screenW * 2.0f; // ceo ekran je sirok 2 u NDC
    borderY = 10.0f / screenH * 2.0f;

    basicShader = createShader("basic.vert", "basic.frag");
    glUseProgram(basicShader);
    glUniform1i(glGetUniformLocation(basicShader, "uTex"), 0);

    Textures textures;
    loadTextures(textures);
    createShapesVAO();
    unsigned int sandVAO = createSandVAO(screenW, screenH);
    initScene(FISH_RATIO, CRAB_RATIO);

    glClearColor(1.0f, 0.65f, 0.5f, 1.0f); // boja neba (ispod teksture neba)

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Eksplicitna naredba programu da uzima u obzir providnost

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double frameStart = glfwGetTime();
        float dt = (float)(frameStart - lastTime); // proteklo vreme od proslog frejma
        lastTime = frameStart;
        if (dt > 0.05f) dt = 0.05f; // zastita od velikog skoka (npr. posle pomeranja prozora)

        // Unos sa tastature (drzanje tastera)
        moveFish(window, goldfish, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, dt);
        moveFish(window, clownfish, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, dt);
        moveCrab(window, dt);

        // Logika scene
        updateCrabJump();
        updateBubbles(dt);
        updateFood(dt);
        eatFood();

        // Crtanje
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(basicShader);
        drawScene(textures, sandVAO);

        glfwSwapBuffers(window); // Zamena bafera
        glfwPollEvents(); // Sinhronizacija pristiglih događaja

        // Frame limiter: cekamo dok ne protekne 1/75 sekunde
        while (glfwGetTime() - frameStart < FRAME_TIME) {}
    }

    glfwDestroyCursor(cursor);
    glfwTerminate();
    return 0;
}
