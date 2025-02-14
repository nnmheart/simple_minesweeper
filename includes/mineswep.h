#ifndef _MINESWEP_H
#define _MINESWEP_H

#include<stdbool.h>

typedef struct cell_t {
    bool is_mine;
    bool is_flagged;
    bool is_revealed;
    int number;
} cell_t;

typedef struct minesweeper_t {
    int grid_width;
    int grid_height;
    cell_t** cells;
} minesweeper_t;

minesweeper_t* minesweeper_create(int grid_width, int grid_height, int mine_count);
void minesweeper_destroy(minesweeper_t* minesweeper);

#endif // _MINESWEP_H