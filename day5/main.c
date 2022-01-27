#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


#define MSTR_IMPLEMENTATION
#include "../mstr.h"

#define NUM_COLS 1024
#define NUM_ROWS 1024

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
    int x;
    int y;
}POINT;

typedef struct{
    POINT *array;
    int n_points;
} LINE;

typedef struct{
    int content[NUM_COLS][NUM_ROWS];
} BOARD;

void get_line_from_points(LINE *line, POINT p1, POINT p2){
    // Considering only horizontal and vertical lines

    if(p1.x == p2.x){
        // Vertical line
        if(p1.y > p2.y){
            int aux = p1.y;
            p1.y = p2.y;
            p2.y = aux;
        }

        int n_points = p2.y -p1.y +1;
        
        POINT *array = malloc(sizeof(POINT) * n_points);
        array[0] = p1; array[n_points -1] = p2;

        for(int i = 1; i < n_points -1; i++){
            POINT new_point = {p1.x, p1.y + i};
            array[i] = new_point;
        }

        line->array = array;
        line->n_points = n_points;

    }else if(p1.y == p2.y){
        // Horizontal line
        if(p1.x > p2.x){
            int aux = p1.x;
            p1.x = p2.x;
            p2.x = aux;
        }

        int n_points = p2.x -p1.x +1;
        
        POINT *array = malloc(sizeof(POINT) * n_points);
        array[0] = p1; array[n_points -1] = p2;

        for(int i = 1; i < n_points -1; i++){
            POINT new_point = {p1.x + i , p1.y};
            array[i] = new_point;
        }

        line->array = array;
        line->n_points = n_points;

    }else{
        // Cross lines
        int m = (p2.y - p1.y)/(p2.x - p1.x);

        if(m == 1 || m == -1){
            if(p2.y < p1.y){
                POINT aux = p1;
                p1 = p2;
                p2 = aux;
            }

            int n_points = p2.y - p1.y + 1;
            
            POINT *array = malloc(sizeof(POINT) * n_points);
            array[0] = p1; array[n_points -1] = p2;

            for(int i = 1; i < n_points -1; i++){
                POINT new_point = {p1.x + i *m , p1.y + i};
                array[i] = new_point;
            }

            line->array = array;
            line->n_points = n_points;

        }
        // Slope != to 45º
        
    }


}

void mark_board_with_point(BOARD *board, POINT p){
    board->content[p.x][p.y]++;
}

void print_board(BOARD *board){
    printf("---    ---\n");
    for(int j = 0; j < NUM_ROWS; j++){
        for(int i = 0; i < NUM_COLS; i++){
            int val = board->content[i][j];
            if(val > 0) printf("\033[1;31m");
            printf("%d", val);
            printf("\033[0m");
        }
        printf("\n");
    }
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

    int n_lines = get_num_of_lines(input_data, count);
    int max_value = 0;
    Mstr data = mstr_from_parts(input_data, count);


    LINE line_set[n_lines];

    for(int i = 0; i < n_lines; i++){
        Mstr line_str = mstr_chop_by_delimiter(&data, '\n');
        Mstr par1 = mstr_trim(mstr_chop_by_delimiter(&line_str, '-'));
        mstr_chop_by_delimiter(&line_str, '>');
        Mstr par2 = mstr_trim(line_str);

        int x1 = atoi(mstr_trim(mstr_chop_by_delimiter(&par1, ',')).data);
        int y1 = atoi(mstr_trim(par1).data);
        POINT p1 = {x1, y1};

        int x2 = atoi(mstr_trim(mstr_chop_by_delimiter(&par2, ',')).data);
        int y2 = atoi(mstr_trim(par2).data);
        POINT p2 = {x2, y2};

        LINE new_line = {NULL, 0};
        get_line_from_points(&new_line, p1, p2);
        line_set[i] = new_line;

        if(max_value < x1) max_value = x1;
        if(max_value < y1) max_value = y1;
        if(max_value < x2) max_value = x2;
        if(max_value < y2) max_value = y2;
    }

    printf("Num lines: %d, max_value: %d\n", n_lines, max_value);

    BOARD board;
    for(int i = 0; i < NUM_COLS; i++){
        for(int j = 0; j < NUM_ROWS; j++){
            board.content[i][j] = 0;
        }
    }

    for(int n = 0; n < n_lines; n++){
        LINE line = line_set[n];
        for(int p = 0; p < line.n_points; p++){
            mark_board_with_point(&board, line.array[p]);
        }
    }

//    print_board(&board);

    int n_overlaps = 0;
    for(int i = 0; i < NUM_COLS; i++){
        for(int j = 0; j < NUM_ROWS; j++){
            if(board.content[i][j] > 1) n_overlaps++;
        }
    }

    printf("--- First Part ---\n");
    printf("num overlaps: %d\n", n_overlaps);

    return 0;
}
