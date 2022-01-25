
#ifndef MSTR_H_
#define MSTR_H_

#ifndef MDEF
#define MDEF
#endif //MDEF

#include <stdio.h>
#include <stdlib.h>

typedef struct{
    const char *data;
    size_t length;
} Mstr;

#define MSTR(char_seq) mstr_from_parts(char_seq, sizeof(char_seq) -1)

#define MSTR_Fmt "%.*s"
#define MSTR_Args(mstr_v) (int)mstr_v.length, mstr_v.data

MDEF Mstr mstr_from_parts(const char *data, size_t length);
MDEF Mstr mstr_trim_left(Mstr str);
MDEF Mstr mstr_trim_right(Mstr str);
MDEF Mstr mstr_trim(Mstr str);
MDEF Mstr mstr_chop_by_delimiter(Mstr *str, char del);
MDEF int mstr_equals(Mstr str1, Mstr str2);

#endif //MSTR_H_
#ifdef MSTR_IMPLEMENTATION

Mstr mstr_from_parts(const char *data, size_t length){
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

    if(index < str->length){
        str->data += index +1;
        str->length -= index+1;
    }else{
        str->data += index;
        str->length -= index;
    }

    return mstr_from_parts(p_str, index);
}

int mstr_equals(Mstr str1, Mstr str2){
    if(str1.length != str2.length) return 0;

    int eq = 1;
    int index = 0;
    while(index < str1.length && eq == 1){
        if(str1.data[index] != str2.data[index]) eq = 0;
        index++;
    }

    return eq;
}

#endif //MSTR_IMPLEMENTATION
