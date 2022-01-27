#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


#define MSTR_IMPLEMENTATION
#include "../mstr.h"

#define NUM_DAYS 256

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

int get_num_of_fishes(char *data, int count){
    int n_fishes = 0;
    for(int i = 0; i < count; i++){
        if(data[i] == ',') n_fishes++;
    }

    return n_fishes + 1;
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
    Mstr data_str = mstr_from_parts(input_data, count);
    int n_fishes = get_num_of_fishes(input_data, count);

    printf("Initial number of fishes: %d\n", n_fishes);

    unsigned long int fish_group[9];
    for(int i = 0; i < 9; i++) fish_group[i] = 0;

    for(int i = 0; i < n_fishes; i++){
        Mstr fish_str = mstr_chop_by_delimiter(&data_str, ',');

        int timer = atoi(fish_str.data);
        if(0 <= timer && timer <= 8) fish_group[timer] += 1;
        else{
            printf("ERROR: invalid input");
            exit(1);
        }
    }

    // Simulation
    for(int k = 0; k < NUM_DAYS; k++){
        unsigned long int aux_6 = 0;
        unsigned long int aux_8 = 0;

        for(int i = 0; i < 8 ; i++){
            if(i == 0){
                aux_8 = fish_group[0];
                aux_6 = fish_group[0];
            } 

            fish_group[i] = fish_group[i+1];
        }

        fish_group[6] += aux_6;
        fish_group[8] = aux_8;
    }

    unsigned long int total_fishes = 0;
    for(int i = 0; i < 9; i++){
        total_fishes += fish_group[i];
    }

    printf("Final number of fishes after %d days: %lu\n",NUM_DAYS, total_fishes);
    
    return 0;
}
