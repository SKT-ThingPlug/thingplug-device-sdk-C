#ifndef _SRA_H_
#define _SRA_H_

typedef enum _sra_event {
    IOT_RAW_TO_TLV,
    IOT_GET_TIME_TLV,
    IOT_GET_GPS_TLV,
    EVENT_TAG_MAX
} SRA_EVENT_TAG;

typedef struct {
    char *tlv;
    char *type;
    char *value;
}raw2tlv;

typedef struct {
    char *tlv;
}timetlv;

typedef struct {
    char *tlv;
    char east[3];
    char north[3];
}gpstlv;

void SRADataConvert(int event, void *data);

#endif//_SRA_H_
