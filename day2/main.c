#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// Usage.
void usage(FILE *stream){
    fprintf(stream, "USAGE: ./main input_file_path" );
}

// File reader and parser.
#define MSTR(v) mstr_from_parts(v, sizeof(v) -1)
#define MSTR_Fmt "%.*s"
#define MSTR_Args(str_args) (int)str_args.length, str_args.data

typedef  struct{
    const char* data;
    size_t length;
}Mstr;

Mstr mstr_from_parts(const char* data, size_t length){
    Mstr str;
    str.data = data;
    str.length = length;

    return str;
}

Mstr mstr_trim_left(Mstr str){
    size_t index = 0;
    while(index < str.length && str.data[index] == ' '){
        index++;
    }

    return mstr_from_parts(str.data + index, str.length -index);
}

Mstr mstr_trim_right(Mstr str){
    size_t index = str.length -1;
    while(index >= 0 && str.data[index] == ' '){
        index--;
    }

    return mstr_from_parts(str.data, index +1);
}

Mstr mstr_trim(Mstr str){
    return mstr_trim_left(mstr_trim_right(str));
}

Mstr mstr_chop_by_delimiter(Mstr *str, char del){
    const char *p_str = str->data;
    size_t index = 0;

    while(index < str->length && str->data[index] != del){
        index++;
    }

    if(index < str-> length){
        str->data += index +1;
        str->length -= index +1;
    }else{
        str->data += index;
        str->length -= index;
    }

    return mstr_from_parts(p_str, index);
}

size_t mstr_estimate_num_of_lines(Mstr str){
    size_t n_lines = 0;
    for(int i = 0; i < str.length; i++){
        if(str.data[i] == '\n')n_lines++;
    }

    return n_lines;
}

int mstr_find_seq_from_init(Mstr str, Mstr seq){
    if(str.length < seq.length) return -1;

    int index = 0;
    bool eq = true;
    while(index < seq.length && eq){
        if(str.data[index] != seq.data[index]) eq = false;
        index++;
    }
    return eq;
}

int mstr_to_int(Mstr str){
    return strtol(str.data, NULL, 10);
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

// The problem itself
#define MSTR_FORWARD MSTR("forward")
#define MSTR_DOWN MSTR("down")
#define MSTR_UP MSTR("up")

typedef enum{
    FORWARD,   // 0
    DOWN,      // 1 
    UP         // 2  
} MOVEMENT;

typedef struct{
    long h_pos;
    long depth;
    long aim;
} SUBMARINE;

void mov_forward(SUBMARINE *sub, long offset){
    sub->h_pos += offset;
    sub->depth += sub->aim * offset;
}

void mov_down(SUBMARINE *sub, long offset){
    //sub->depth += offset;
    sub->aim += offset;
}

void mov_up(SUBMARINE *sub, long offset){
    //sub->depth -= offset;
    sub->aim -= offset;
}

MOVEMENT get_movement_from_mstr(Mstr mov_str){
        if(mstr_find_seq_from_init(mov_str, MSTR_FORWARD) == 1){
            return FORWARD;

        }else if(mstr_find_seq_from_init(mov_str, MSTR_DOWN) == 1){
            return DOWN;

        }else if(mstr_find_seq_from_init(mov_str, MSTR_UP) == 1){
            return UP;
        }

        return -1;
}

int main(int args, char **argv){
    if(args < 2){
        usage(stderr);
        exit(1);
    }
    
    char *file_path = argv[1];
    size_t count = 0;

    char *input_data = get_input_data_from_file(file_path, &count);
    if(input_data == NULL){
        fprintf(stderr, "ERROR: could not load the input file");
        exit(1);
    }

    Mstr data = mstr_from_parts(input_data, count);
    size_t n_lines = mstr_estimate_num_of_lines(data);

    printf("--- Initializing Submarine ---\n");
    SUBMARINE sub = {0, 0};

    int error = 0;
    for(int i = 0; i < n_lines; i++){
        Mstr line = mstr_chop_by_delimiter(&data, '\n');
        line = mstr_trim(line);
        Mstr mov_str = mstr_chop_by_delimiter(&line, ' ');

        MOVEMENT mov = get_movement_from_mstr(mov_str);
        long offset = mstr_to_int(line);

//        printf("movement: %d offset: %lu\n", mov, offset);

        if(mov == FORWARD){
            mov_forward(&sub, offset);
        }else if(mov == DOWN){
            mov_down(&sub, offset);
        }else if(mov == UP){
            mov_up(&sub, offset);
        }else if(mov < 0){
            error++;
        }
    }

    long value = sub.h_pos * sub.depth;
    printf("Num errors: %d, end_hpos: %ld, end_depth: %ld, value: %ld\n", error, sub.h_pos, sub.depth, value);



    return 0;
}
