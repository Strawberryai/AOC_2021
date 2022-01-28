#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define MSTR_IMPLEMENTATION
#include "../mstr.h"

#define MIN_INIT_VAL 1000000000

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

int get_num_of_crabs(char *data, int count){
    int num_crabs = 0;

    for(int i = 0; i < count; i++){
        if(data[i] == ',') num_crabs++;

    }
    return num_crabs + 1;
}

int sum(int n){
    int sum = 0;
    for(int i = 1; i <= n; i++) sum += i;

    return sum;
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

    int n_crabs = get_num_of_crabs(input_data, count);
    int data_array[n_crabs];
    int min_pos = MIN_INIT_VAL;
    int max_pos = 0;

    for(int i = 0; i < n_crabs; i++){
        Mstr pos_str = mstr_chop_by_delimiter(&data, ',');
        int pos = atoi(pos_str.data);

        data_array[i] = pos;

        if(pos < min_pos) min_pos = pos;
        if(pos > max_pos) max_pos = pos;
    }

    int min_fuel = MIN_INIT_VAL;
    int pos = 0;

    for(int k = min_pos; k <= max_pos; k++){
        int fuel = 0;

        for(int i = 0; i < n_crabs; i++){
            int current_pos = data_array[i];
            int disp = k - current_pos;
            if(disp < 0) disp *= -1;

            int consumption = sum(disp);

            fuel += consumption;
        }

        if(fuel < min_fuel){
            min_fuel = fuel;
            pos = k;
        }
    }
    
    printf("The position %d has the lowest fuel consumption: %d\n", pos, min_fuel);
    
    return 0;
}
