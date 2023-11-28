#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "flipdot.h"

/**
 * Render a text string to the dotboard using the 4x5 font.
 */
void render_ghost(dotboard_t *dotboard, int x)
{
    
    int ghost[8][9] = {
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 1, 0},
    };

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 9; j++) {
            (*dotboard)[x + j][i] = ghost[i][j];
        }
    }
}

void render_cyberman(dotboard_t *dotboard, int x)
{
    int cyberman[8][7] = {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 7; j++) {
            (*dotboard)[x + j][i] = cyberman[i][j];
        }
    }
}