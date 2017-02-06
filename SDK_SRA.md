## 1. SRA 의 역활
    * IoT 기기들의 다양한 데이터를 원하는 형태로 변환하는 것에 있다. 
    * 데이터 변환에 대한 정책을 관리한다. 
## 2. SRA 폴더 구조
    * SRA ( root )
        * SRA.c (정책에 따라 데이터를 변환시키는 함수가 구현되어 있다.)
        * List.c ( 리스트 자료구조 )
        * TLVMaker.c ( Tag:Length:Value 변환 모듈 )
        * GetTime.cpp ( NTP 서버 연결 및 시간 관련 함수가 구현되어 있다.)
## 3. 주요 함수
    * void SRADataConvert(int event, void *data)
    * 데이터 변환을 할 때 사용하는 함수이다.
    * event는 데이터 변환의 종류를 정의한다.
    * 표는 기능구현된 목록을 나타낸다.
    
event | 데이터 변환 종류 설정
----|----
IOT_RAW_TO_TLV | Raw 데이터에서 TLV 데이터
IOT_GET_TIME_TLV | Time TLV 데이터 (단순리턴)
IOT_GET_GPS_TLV | GPS TLV 데이터 (단순리턴)

## 4. 사용 방법

* Raw 데이터를 TLV로 변환하는 방법 :
```
// raw2lora 구조체 정의 (SRA.h)
raw2tlv temp;
temp.type = "temperature" 
temp.value = "25.25" 

SRADataConvert( IOT_RAW_TO_TLV, (void*) &temp);

printf("tlv :<%s>\n", temp.tlv);
free(temp.tlv);
```
* 시간정보를 TLV로 얻는 방법 (주의!내부 NTP 이용) :
```
timetlv time;

SRADataConvert( IOT_GET_TIME_TLV, (void*) &time);

printf("tlv :<%s>\n", time.tlv);
free(time.tlv);
```
* GPS TLV 변환 방법 :
```
gpstlv gps; 
// T타워 : 북위, 동경
gps.north[0] = 0xb2;
gps.north[1] = 0xda;
gps.north[2] = 0x25;
gps.east[0] = 0xfb;
gps.east[1] = 0x2b;
gps.east[2] = 0x59;

SRADataConvert( IOT_GET_GPS_TLV, (void*) &gps);

printf("tlv :<%s>\n", gps.tlv);
free(gps.tlv);
```
## 5. 내부에서 사용되는 private 함수 정리

함수명 | 기능
---|---
GetPolicyIndex | 구분문자열을 기준으로 정책 번호를 리턴함
SRASetValueToTLV | 정책 번호를 기준으로 데이터를 가공 (예 : "temperature" 27.123 -> 2713)
시간 관련 함수들 | NTP서버에서 시간을 얻어오는 함수 (예 : getYear(), getMonth() ...)
