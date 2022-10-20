#define OLC_PGE_APPLICATION

#include "PGE.h"

#define s_width 512
#define s_height 512

enum ParticleType {
    SAND = 0,
    WATER = 1,
};

struct Particle {
    enum ParticleType type;
    double x, y;
    double vel_x, vel_y;
    bool move;
    bool through_function;
};

struct Particle* gridmap[s_width * s_height];

olc_Pixel type_colours[2];

void mouseInput();
void keyInput();
void updateParticles();
bool moveParticle();

int brush_radius = 1;
enum ParticleType brush_element = SAND;

bool OnUserCreate() {
    memset(gridmap, 0, sizeof(gridmap[0]) * s_width * s_height);
    type_colours[SAND] = olc_PixelRGB(255, 255, 0);
    type_colours[WATER] = olc_PixelRGB(0, 0, 255);//wo'uh

    return true;
}

bool OnUserUpdate(float fElapsedTime) {
    for (int i = 0; i < s_width * s_height; i++) {
        int x = i % s_width;
        int y = i / s_width;
        if (gridmap[i] == NULL) {
            PGE_Draw(x, y, olc_PixelRGB(0, 0, 0));
            continue;
        }
        enum ParticleType type = gridmap[i]->type;

        PGE_Draw(x, y, type_colours[type]);
    }

    updateParticles();
    mouseInput();
    keyInput();
    return !PGE_GetKey(olc_ESCAPE).bPressed;
}

bool OnUserDestroy() {
    for (int i = 0; i < s_width * s_height; i++) {
        if (gridmap[i] != NULL) {
            free(gridmap[i]);
        }
    }
    return true;
}

void updateParticles() {

    for (int i = 0; i < s_width * s_height; i++) {
        if (gridmap[i] == NULL) {
            continue;
        }
        gridmap[i]->through_function = false;
        int random_side = (rand() % 2) * 2 - 1;
        switch (gridmap[i]->type) {
        case SAND:
            if (i / s_width != (int)gridmap[i]->y) {
                printf("sand discepancy: %d %d\n", i / s_width, (int)gridmap[i]->y);
            }
            if (gridmap[i + s_width] == NULL && gridmap[i]->y + 1 < s_height) {
                gridmap[i]->y += 1;
                gridmap[i]->move = true;
            }
            else if (gridmap[i + random_side + s_width] == NULL && gridmap[i]->y + 1 < s_height && gridmap[i]->x + random_side >= 0 && gridmap[i]->x + random_side < s_width) {
                gridmap[i]->y += 1;
                gridmap[i]->x += random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i - random_side + s_width] == NULL && gridmap[i]->y < s_height - 1 && gridmap[i]->x - random_side >= 0 && gridmap[i]->x - random_side < s_width) {
                gridmap[i]->y += 1;
                gridmap[i]->x -= random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i + random_side * 2 + s_width] == NULL && gridmap[i]->y < s_height - 1 && gridmap[i]->x + random_side * 2 >= 0 && gridmap[i]->x + random_side * 2 < s_width) {
                gridmap[i]->y += 1;
                gridmap[i]->x += 2 * random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i - random_side * 2 + s_width] == NULL && gridmap[i]->y < s_height - 1 && gridmap[i]->x - random_side * 2 >= 0 && gridmap[i]->x - random_side * 2 < s_width) {
                gridmap[i]->y += 1;
                gridmap[i]->x -= 2 * random_side;
                gridmap[i]->move = true;
            }
            else {
                gridmap[i]->move = false;
            }
            break;
        case WATER:
            if (i / s_width != (int)gridmap[i]->y) {
                printf("wo'uh discrepancy: %d %d\n", i / s_width, (int)gridmap[i]->y);

            }
            if (gridmap[i + s_width] == NULL && gridmap[i]->y + 1 < s_height) {
                gridmap[i]->y += 1;
                gridmap[i]->move = true;
            }
            else if (gridmap[i + random_side] == NULL && gridmap[i]->x + random_side >= 0 && gridmap[i]->x + random_side < s_width) {
                gridmap[i]->x += random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i - random_side] == NULL && gridmap[i]->x - random_side >= 0 && gridmap[i]->x - random_side < s_width) {
                gridmap[i]->x -= random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i + random_side * 2] == NULL && gridmap[i]->x + random_side * 2 >= 0 && gridmap[i]->x + random_side * 2 < s_width) {
                gridmap[i]->x += 2 * random_side;
                gridmap[i]->move = true;
            }
            else if (gridmap[i - random_side * 2] == NULL && gridmap[i]->x - random_side * 2 >= 0 && gridmap[i]->x - random_side * 2 < s_width) {
                gridmap[i]->x -= 2 * random_side;
                gridmap[i]->move = true;
            }
            else {
                gridmap[i]->move = false;
            }
            break;
        }
    }
    for (int i = 0; i < s_height*s_width; i++) {
        if (gridmap[i] == NULL) {
            continue;
        }
        moveParticle(i);
    }
}

int counter = 0;

bool moveParticle(int i) {
    //gridmap[i]->through_function = true;
    counter++;
    //printf("counter: %d\n", counter);
    int next_i = (int)floor(gridmap[i]->y * s_width + gridmap[i]->x);
    if (next_i > 262144) {
        printf("%lf %lf\n", gridmap[i]->x, gridmap[i]->y);
        gridmap[i]->through_function = true;
        gridmap[i]->move = false;
        counter--;
        return false;
    }
    if (!gridmap[i]->move || gridmap[i]->through_function) {
        counter--;
        return false;
    }
    if (gridmap[next_i] != NULL) {
        gridmap[i]->through_function = true;
        if (moveParticle(next_i)) {
            gridmap[i]->move = false;
            gridmap[next_i] = gridmap[i];
            gridmap[i] = NULL;
            counter--;
            return true;
        }
        else {
            gridmap[i]->move = false;
            gridmap[i]->x = i % s_width;
            gridmap[i]->y = i / s_width;
            counter--;
            return false;
        }
    }
    else {
        gridmap[i]->through_function = true;
        gridmap[i]->move = false;
        gridmap[next_i] = gridmap[i];
        gridmap[i] = NULL;
        counter--;
        return true;
    }
    counter--;
    gridmap[i]->move = false;
    return false;
}

void keyInput() {
    if (PGE_GetKey(olc_E).bPressed) {
        brush_radius++;
    }
    else if (PGE_GetKey(olc_Q).bPressed && brush_radius > 0) {
        brush_radius--;
    }
    else if (PGE_GetKey(olc_K1).bPressed) {
        brush_element = SAND;
    }
    else if (PGE_GetKey(olc_K2).bPressed) {
        brush_element = WATER;
    }
}

void mouseInput() {
    if (PGE_GetMouse(0).bHeld) {
        int r = brush_radius;
        for (int dy = -r; dy < r; dy++) {
            for (int dx = -r; dx < r; dx++) {
                if (dx * dx + dy * dy < r * r) {
                    int y = PGE_GetMouseY() + dy;
                    int x = PGE_GetMouseX() + dx;
                    int index = y * s_width + x;
                    if (index >= sizeof * gridmap[0] * s_width * s_height || index < 0) {
                        continue;
                    }
                    if (gridmap[index] == NULL) {
                        gridmap[index] = malloc(sizeof * gridmap[0]);
                    }
                    gridmap[index]->x = index % s_width;
                    gridmap[index]->y = index / s_width;
                    gridmap[index]->type = brush_element;
                }
            }
        }
    }

}

int main(int argc, char* argv[]) {

    PGE_SetAppName("YAPG");
    if (PGE_Construct(s_width, s_height, 1, 1, false, false)) {
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);
    }

    return 0;
}