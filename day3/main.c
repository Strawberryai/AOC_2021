#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MSTR_IMPLEMENTATION
#include "../mstr.h"

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

int get_num_of_digits(char *data, int count){
    int max_digits = 0, n_digits = 0, n_diff = 0;

    for(int i = 0; i < count; i++){
        if(data[i] != '\n') n_digits++;
        else{
            if(n_digits > max_digits){
                max_digits = n_digits;
                n_diff++;
            }

            n_digits = 0;
        }
    }
    assert(n_diff == 1);

    return max_digits;
}

int my_pow(int b, int e){
    int exp = 1;
    while(e > 0){
        exp *= b;
        e--;
    }
    return exp;
}

int binary_array_to_int(int *array, int length){
    int dec_value = 0;
    for(int i = 0; i < length; i++){
        dec_value += my_pow(2, length -i -1) * array[i];
    }
    return dec_value;
}

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
    int n_digits = get_num_of_digits(input_data, count);

    Mstr data = mstr_from_parts(input_data, count);

    Mstr lines[n_lines];
    for(int i = 0; i < n_lines; i++){
        Mstr line = mstr_chop_by_delimiter(&data, '\n');
        lines[i] = mstr_trim(line);
    }

    printf("num lines: %d, num digits: %d\n", n_lines, n_digits);

    printf("--- First part ---\n");

    int gamma[n_digits];
    for(int i = 0; i < n_digits; i++){
        int cero_count = 0, one_count = 0;

        for(int j = 0; j < n_lines; j++){
            if(lines[j].data[i] == '0') cero_count++;
            else if (lines[j].data[i] == '1') one_count++;
        }
        gamma[i] = cero_count > one_count ? 0 : 1;
    }

    int gamma_value = binary_array_to_int(gamma, n_digits);

    int epsilon[n_digits];
    for(int i = 0; i < n_digits; i++){
        int cero_count = 0, one_count = 0;

        for(int j = 0; j < n_lines; j++){
            if(lines[j].data[i] == '0') cero_count++;
            else if (lines[j].data[i] == '1') one_count++;
        }
        epsilon[i] = cero_count > one_count ? 1 : 0;
    }

    int epsilon_value = binary_array_to_int(epsilon, n_digits);

    int consumption = gamma_value * epsilon_value;
    printf("gamma: %d, epsilon: %d\n",gamma_value, epsilon_value);
    printf("consumption: %d\n", consumption);

    printf("--- Second part ---\n");

    Mstr NULL_str = MSTR("NULL");

    // Oxygen generator ratin
    Mstr oxygen_numbers[n_lines];
    int oxygen_count = n_lines;
    for(int i = 0; i < n_lines; i++){
        oxygen_numbers[i] = lines[i];
    }

    int i = 0;
    while(i < n_digits && oxygen_count > 1){
        int cero_count = 0, one_count = 0;

        for(int j = 0; j < n_lines; j++){
            if(!mstr_equals(oxygen_numbers[j], NULL_str)){
                if(oxygen_numbers[j].data[i] == '0') cero_count++;
                else if (oxygen_numbers[j].data[i] == '1') one_count++;
            }
        }

        char bit_criteria = cero_count <=  one_count ? '1': '0';
        for(int j = 0; j < n_lines; j++){
            if(!mstr_equals(oxygen_numbers[j], NULL_str)){
                if(oxygen_numbers[j].data[i] != bit_criteria){
                    oxygen_numbers[j] =NULL_str;
                    oxygen_count--;
                }
            }
        }
//        printf("digit: %d, bit_criteria: %c, remaining numbers: %d\n", i, bit_criteria, oxygen_count);


        i++;
    }

    assert(oxygen_count == 1);

    int oxygen_bit_array[n_digits];
    int num_count = 0;
    for(int i = 0; i < n_lines; i++){
        if(!mstr_equals(oxygen_numbers[i], NULL_str)){
            num_count++;
            for(int j = 0; j < n_digits; j++){
                if(oxygen_numbers[i].data[j] == '0') oxygen_bit_array[j] = 0; 
                else if(oxygen_numbers[i].data[j] == '1') oxygen_bit_array[j] = 1; 
            }
        }
    }

    assert(num_count == 1);

    int oxygen_value = binary_array_to_int(oxygen_bit_array, n_digits);

    // CO2 generator ratin
    Mstr CO2_numbers[n_lines];
    int CO2_count = n_lines;
    for(int i = 0; i < n_lines; i++){
        CO2_numbers[i] = lines[i];
    }

    i = 0;
    while(i < n_digits && CO2_count > 1){
        int cero_count = 0, one_count = 0;

        for(int j = 0; j < n_lines; j++){
            if(!mstr_equals(CO2_numbers[j], NULL_str)){
                if(CO2_numbers[j].data[i] == '0') cero_count++;
                else if (CO2_numbers[j].data[i] == '1') one_count++;
            }
        }

        char bit_criteria = cero_count <=  one_count ? '0': '1';

        for(int j = 0; j < n_lines; j++){
            if(!mstr_equals(CO2_numbers[j], NULL_str)){
                if(CO2_numbers[j].data[i] != bit_criteria){
                    CO2_numbers[j] = NULL_str;
                    CO2_count--;
                }
            }
        }

        i++;
    }

    assert(CO2_count == 1);


    int CO2_bit_array[n_digits];
    num_count = 0;
    for(int i = 0; i < n_lines; i++){
        if(!mstr_equals(CO2_numbers[i], NULL_str)){
            num_count++;
            for(int j = 0; j < n_digits; j++){
                if(CO2_numbers[i].data[j] == '0') CO2_bit_array[j] = 0; 
                else if(CO2_numbers[i].data[j] == '1') CO2_bit_array[j] = 1; 
            }
        }
    }
    assert(num_count == 1);
    int CO2_value = binary_array_to_int(CO2_bit_array, n_digits);

    printf("oxygen value: %d, CO2 value: %d, generator ratin: %d\n", oxygen_value, CO2_value, oxygen_value * CO2_value);

    return 0;
}




