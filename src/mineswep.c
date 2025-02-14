#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<malloc.h>
#include<time.h>
#include<mineswep.h>

void __ms_set_mines(minesweeper_t* game, int mine_count) {
    for (int k = 0; k < mine_count; k++) {
        int x, y;
        do {
            x = rand() % game->grid_width;
            y = rand() % game->grid_height;
        } while (game->cells[x][y].is_mine);

        game->cells[x][y].is_mine = true;
    }
}

void __ms_set_numbers(minesweeper_t* game) {
    int width = game->grid_width;
    int height = game->grid_height;

    cell_t** cells = game->cells;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            cell_t cur_cell = cells[x][y];
            if (cur_cell.is_mine) continue;
            
            int adj_mines = 0;

            if (x > 0) {
                if (                     cells[x - 1][y    ].is_mine) adj_mines++;
                if ((y > 0)          && (cells[x - 1][y - 1].is_mine)) adj_mines++;
                if ((y < height - 1) && (cells[x - 1][y + 1].is_mine)) adj_mines++;
            }

            if (x < width - 1) {
                if (                     cells[x + 1][y    ].is_mine) adj_mines++;
                if ((y > 0)          && (cells[x + 1][y - 1].is_mine)) adj_mines++;
                if ((y < height - 1) && (cells[x + 1][y + 1].is_mine)) adj_mines++;
            }

            if ((y > 0) && (cells[x][y - 1].is_mine)) adj_mines++;
            if ((y < height - 1) && (cells[x][y + 1].is_mine)) adj_mines++;

            cells[x][y].number = adj_mines;
        }
    }

}

minesweeper_t* minesweeper_create(int grid_width, int grid_height, int mine_count) {
    minesweeper_t* game = (minesweeper_t*)malloc(sizeof(minesweeper_t));
    game->grid_width = grid_width;
    game->grid_height = grid_height;

    game->cells = (cell_t**)malloc(sizeof(cell_t*) * grid_width);
    for (int i = 0; i < grid_width; i++) {
        game->cells[i] = (cell_t*)malloc(sizeof(cell_t) * grid_height);
        for (int j = 0; j < grid_height; j++) {
            game->cells[i][j].is_mine = false;
            game->cells[i][j].is_flagged = false;
            game->cells[i][j].is_revealed = false;
            game->cells[i][j].number = 0;
        }
    }

    __ms_set_mines(game, mine_count);
    __ms_set_numbers(game);

    return game;
}

void minesweeper_destroy(minesweeper_t* game) {
    for (int i = 0; i < game->grid_width; i++) {
        free(game->cells[i]);
    }
    free(game->cells);
    free(game);
}