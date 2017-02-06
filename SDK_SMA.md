## 1. SMA 의 역활
    * IoT 기기에서 값을 읽는다. 
    * 각 기기를 구분하여 관리한다. 
## 2. SMA 폴더 구조
    * SMA ( root )
        * SMA.c (기기로 부터 데이터를 얻는 함수가 구현된다. 주의!현재 임의값을 리턴한다.)
## 3. 주요 함수
    * void SMAGetData(char *sensorType, char** output)
    * 데이터를 얻고자 할 때 사용되는 함수이다..
    * 각 기기들은 sensorType이라는 문자열로 구분한다..
    * 표는 기기 목록을 나타낸다.
    
sensorType (구분문자열) | 센서 기능
----|----
"motion" | 모션 감지
"temperature" | 온도 측정
"humidity" | 습도 측정 
"light" | 조도 측정 
 
## 4. 사용 방법
* Raw 데이터를 TLV로 변환하는 방법 :
```
char *sensorType = "temperature";
char *output;

SMAGetData(sensorType, &output);

printf("temperature: %s\n", output);
free(output);
```
## 5. 내부에서 사용되는 private 함수 정리

함수명 | 기능
---|---
SMAGetSensorIndex | 구분문자열을 기준으로 센서 번호를 리턴함
