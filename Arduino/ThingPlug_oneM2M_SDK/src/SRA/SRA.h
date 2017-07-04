#ifndef _SRA_H_
#define _SRA_H_

typedef struct {
    char *ttv;
    char type;
    char datatype;
    char *value;
}raw2ttv;

void SRAGetTTV(char **out_buf, char type, char datatype, char *value );

#endif//_SRA_H_
