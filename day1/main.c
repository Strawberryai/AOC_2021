#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void usage(FILE *stream){
    fprintf(stream, "USAGE: ./main input_file.txti\n");
}

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

typedef struct{
    char *data;
    size_t count;
}String;

String str_from_parts(char *data, size_t count){
    String str;
    str.data = data;
    str.count = count;

    return str;
}

String str_chop_by(String *str, char a){
    int index = 0;
    while(index < str->count && str->data[index] != a){
        index++;
    }

    String out_str = str_from_parts(str->data, index);

    if(index < str->count){
        str->count -= index + 1;
        str->data += index + 1;
    } else{
        str->count -= index;
        str->data += index;
    }

    return out_str;
}

long str_to_double(String str, int *suspicious){
    char **str_end = NULL;
    long ret = strtol(str.data, str_end, 10);
    if(ret == 0) *suspicious += 1;

    return  ret;
}

int main(int args, char **argv){

    if(args < 2){
        usage(stderr);
        exit(1);
    }

    size_t count = 0;
    char *input_data_buff = get_input_data_from_file(argv[1], &count);
    if(input_data_buff == NULL){
        fprintf(stderr, "ERROR: couldn't load the data file %s\n", argv[1]);
        exit(1);
    }

    String input_data = str_from_parts(input_data_buff, count);
    String *data_array = (String *) malloc(input_data.count);

    int line_n = 0;

    while(input_data.count > 0){
        data_array[line_n] = str_chop_by(&input_data, '\n');
        line_n++;
    }

    //--- Part One ---
    printf("--- Part One ---\n");

    int n_incr = 0;
    int n_susp = 0;
    long prev = str_to_double(data_array[0], &n_susp);
    if(prev == 0) n_susp++;

    for(int index =1; index < line_n; index++){
        long curr = str_to_double(data_array[index], &n_susp);
        if(curr > prev) n_incr++;

        //printf("prev: %ld, curr: %ld\n", prev, curr);
        
        prev = curr;
    }
    
    printf("n_incr: %d\n", n_incr);
    printf("n_susp: %d\n", n_susp);

    
    //--- Part Two ---
    printf("--- Part Two ---\n");
    
    assert(line_n >= 3);
    n_incr = 0;
    n_susp = 0;
    prev = str_to_double(data_array[0], &n_susp) + 
        str_to_double(data_array[1], &n_susp) + 
        str_to_double(data_array[2], &n_susp);

    for(int index = 3; index < line_n; index++){
        long curr = str_to_double(data_array[index -2], &n_susp) + 
            str_to_double(data_array[index -1], &n_susp) + 
            str_to_double(data_array[index], &n_susp);
        if(curr > prev) n_incr++;

        prev = curr;
    }

    printf("n_incr: %d\n", n_incr);
    printf("n_susp: %d\n", n_susp);

    return 0;
}
