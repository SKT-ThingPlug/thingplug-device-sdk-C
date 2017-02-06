Linux (+TLS)
===

지원 사양
---
1. 지원 보드
	+ Raspberry PI 2/3, BeagleBone-Black, etc 

2. 최소 동작 환경
	+ CPU : 100MHz
	+ RAM : 5MB
	+ Flash memory : 5MB

Source Tree
---
* [Source](https://github.com/SKT-ThingPlug/thingplug-device-sdk-C/tree/master/linux)
+ __oneM2M__ (ThingPlug oneM2M SDK root folder)
	+ __docs__ (SDK API documentation)
	+ __samples__ (Samples including makefiles for build)
		+ __MA__ (ManagementAgent)
		+ __SMA__ (SensorManagementAgent)
		+ __SRA__ (ServiceReadyAgent)
		+ __lib__ (external libraries - paho)
	+ __include__ (Header files of the ThingPlug oneM2M SDK)
	+ __src__ (Source files of the ThingPlug oneM2M SDK)
		+ __oneM2M_V1__ (Source files of the ThingPlug oneM2M V1 SDK)
		+ __oneM2M_V1_12__ (Source files of the ThingPlug oneM2M V1.12 SDK)
		+ __net__ (Source file of the MQTT)

Sample build
===

Configuration 설정(oneM2M/samples/Configuration.h)
---
1. oneM2M 버전 선택
	+ __ONEM2M_V1_12__ : define 됐을 경우 oneM2M v1.12가 활성화 됨

2. oneM2M V1 정보 입력
	+ __MQTT_ENABLE_SERVER_CERT_AUTH__ : 0(Normal socket 사용), 1(TLS socket 사용)
	+ __MQTT_HOST__ : ThingPlug 서버 주소(TLS 사용시 주소 앞에 __ssl://__ 추가 필요)
	+ __ACCOUNT_USER__ : ThingPlug 계정의 ID
	+ __ACCOUNT_PASSWORD__ : ThingPlug 계정의 UKey
	+ __ONEM2M_NODEID__ : ThingPlug 계정에 등록할 Device ID

2. oneM2M V1.12 정보 입력
	+ __MQTT_ENABLE_SERVER_CERT_AUTH__ : 0(Normal socket 사용), 1(TLS socket 사용:서버 미지원)
	+ __MQTT_HOST__ : ThingPlug 서버 주소
	+ __ACCOUNT_USER__ :  ThingPlug 계정의 ID
	+ __ACCOUNT_PASSWORD__ : ThingPlug 계정의 Password
	+ __ONEM2M_CB__ : CSEBase ID
	+ __ONEM2M_TO__ : To URL
	+ __APP_AEID__ : App AE ID

SDKVerification 빌드(oneM2M/samples/SDKVerification.c)
---
1. 빌드

	```
	# make
	```
	
2. 빌드 클리어

	```
	# make clean
	```
	
3. 실행

	```
	# output/SDKVerification
	```

ThingPlug_oneM2M_SDK 빌드(oneM2M/samples/ThingPlug_oneM2M_SDK.c)
---
1. 빌드

	```
	# make -f Makefile.mw
	```
	
2. 빌드 클리어

	```
	# make -f Makefile.mw clean
	```
	
3. 실행

	```
	# output/ThingPlug_oneM2M_SDK
	```
	
Copyright (c) 2016 SK Telecom Co., Ltd. All Rights Reserved.
