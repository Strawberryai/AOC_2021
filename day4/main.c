#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


#define MSTR_IMPLEMENTATION
#include "../mstr.h"

#define NUM_ROWS 5
#define NUM_COLS 5

// Usage
void usage(FILE *stream){
    fprintf(stream, "USAGE: main ./input_file.txt\n");
}

//Read file
char *get_input_data_from_file(char *file_path, size_t *count){
    char *buffer = NULL;
    FILE *f = fopen(file_path, "rb");
    
    if(f == NULL){
        goto error;
    }

    if(fseek(f, 0, SEEK_END) < 0){
        goto error;
    }

    size_t m = ftell(f);

    if(m < 0){
        goto error;
    }

    if(fseek(f, 0, SEEK_SET) < 0){
        goto error;
    }

    buffer = (char *) malloc(m);

    size_t n = fread(buffer, 1, m, f);
    assert(n == m);
    *count = n;

    fclose(f);

    return buffer;
error:
    if(f){
        fclose(f);
    }

    if(buffer){
        free(buffer);
    }

    return NULL;
}

int get_num_of_lines(char *data, int count){
    int n_lines = 0;
    for(int i = 0; i < count; i++){
        if(data[i] == '\n') n_lines++;
    }

    return n_lines;
}

// Data structures
typedef struct{
    int data;
    bool mark;
} Cell;

typedef struct{
    Cell cell[NUM_COLS][NUM_ROWS];
    bool win;
} Board;

int num_of_instructions(Mstr str){
    int n = 0;
    for(int i = 0; i < str.length; i++){
        if(str.data[i] == ',') n++;
    }
    if(n > 0) n++;

    return n;
}

void print_board(Board board){
    if(board.win) printf("\033[0;31m");
    for(int j = 0; j < NUM_ROWS; j++){
        for(int i = 0; i < NUM_COLS; i++){
            if(!board.win && board.cell[i][j].mark) printf("\033[0;36m");
            printf("%d ", board.cell[i][j].data);
            printf("\033[0m");

            if(board.win) printf("\033[0;31m");
        }
        printf("\n");
    }

    printf("\033[0m");
    printf("\n");
}

bool check_winner_winner_chicken_dinner(Board board, int x, int y){
    // Check the row
    int row_win_count = 0;
    for(int i = 0; i < NUM_COLS; i++){
        if(board.cell[i][y].mark) row_win_count++;
    }
    if(row_win_count == NUM_COLS) return true;

    // Check the column
    int col_win_count = 0;
    for(int j = 0; j < NUM_ROWS; j++){
        if(board.cell[x][j].mark) col_win_count++;
    }
    if(col_win_count == NUM_ROWS) return true;

    return false;
}

bool mark_board_by_number(Board *board, int number, int *out_x, int *out_y){
    bool win = false;
    for(int j = 0; j < NUM_ROWS && !win; j++){
        for(int i = 0; i < NUM_COLS && !win; i++){

            if(board->cell[i][j].data == number){
                board->cell[i][j].mark = true;
                win = check_winner_winner_chicken_dinner(*board, i, j);
                if(win){
                    *out_x = i;*out_y = j;
                }

            }
        }
    }

    return win;
}

int rest_value_of_board(Board board){
    int rest = 0;
    for(int i = 0; i < NUM_COLS; i++){
        for(int j = 0; j < NUM_ROWS; j++){
            if(!board.cell[i][j].mark) rest += board.cell[i][j].data;
        }
    }

    return rest;
}

// Main program
int main(int args, char **argv){
    if(args < 2){
        usage(stderr);
        exit(1);
    }

    size_t count = 0;
    char *input_data = get_input_data_from_file(argv[1], &count);
    if(input_data == NULL){
        fprintf(stderr, "ERROR: could not load the input file\n");
    }
    Mstr data = mstr_from_parts(input_data, count);
    Mstr game_data_str = mstr_chop_by_delimiter(&data, '\n');

    // Read the play instructions
    int n_plays = num_of_instructions(game_data_str);
    int game_data[n_plays];

    for(int i = 0; i < n_plays; i++){
        game_data[i] = atoi(mstr_chop_by_delimiter(&game_data_str, ',').data);
    }

    // Read the boards info
    int n_lines = get_num_of_lines(input_data, count) -1;
    int n_boards = (n_lines / 6);

    Board boards[n_boards];
    Board boards2[n_boards];


     for(int k = 0; k < n_boards; k++){
        mstr_chop_by_delimiter(&data, '\n');
        Board new_board;

        for(int j = 0; j < NUM_ROWS; j++){
            Mstr line = mstr_trim(mstr_chop_by_delimiter(&data, '\n'));

            for(int i = 0; i < NUM_COLS; i++){
                Mstr value_str =  MSTR("");
                while(value_str.length == 0){
                    value_str = mstr_chop_by_delimiter(&line, ' ');
                }

                int value = atoi(value_str.data);

                new_board.win = false;
                new_board.cell[i][j].data = value;
                new_board.cell[i][j].mark = false;
            }
        }

        boards[k] = new_board;
        boards2[k] = new_board;
    }

    //The algorithm
    printf("--- First part ---\n");
    //for(int k = 0; k < n_boards; k++) print_board(boards[k]);

    bool win = false;
    int k, b, x, y;
    for(k = 0; k < n_plays && !win; k++){
        //printf("---Markin %d ---\n", game_data[k]);

        for(b = 0; b < n_boards &&!win; b++){
            x = 0; y = 0;
            win = mark_board_by_number(&boards[b], game_data[k], &x, &y);

            //print_board(boards[b]);
        }
    }

    int rest_value = rest_value_of_board(boards[b -1]);
    int last_num = boards[b-1].cell[x][y].data;
    int factor = last_num * rest_value;

    printf("---            ---\n");
    printf("Number of boards: %d, number of lines: %d, number of steps: %d\n", n_boards, n_lines, n_plays);
    printf("The board %d is the winner on the step %d\n", b, k);
    printf("The last num was %d and the rest value is %d, so the factor is %d\n", 
            last_num, rest_value, factor);

    printf("\n--- Second part ---\n");
    bool last_win = false;
    int num_wins = 0;
    for(k = 0; k < n_plays && !last_win; k++){
        //printf("--- Marking %d ---\n", game_data[k]);
        for(b = 0; b < n_boards &&!last_win; b++){
            x = 0; y = 0;
            win = mark_board_by_number(&boards2[b], game_data[k], &x, &y);

            if(win && !boards2[b].win){
                num_wins++;
                boards2[b].win = true;
                if(num_wins == n_boards) last_win = true;
            }
         //   print_board(boards2[b]);
        }
    }

    rest_value = rest_value_of_board(boards2[b -1]);
    last_num = boards2[b-1].cell[x][y].data;
    factor = last_num * rest_value;

    printf("---            ---\n");
    printf("num wins: %d\n", num_wins);
    printf("The last winner board is: %d\n", b);
    printf("The last num was %d and the rest value is %d, so the factor is %d\n", 
            last_num, rest_value, factor);

    return 0;
}
