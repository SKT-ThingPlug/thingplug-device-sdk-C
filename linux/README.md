ThingPlug 1.5 C SDK for Linux
===

지원 사양
---
1. 지원 보드
	+ ARM: Raspberry Pi 2/3, BeagleBone Black/Green, NVIDIA Jetson TX1/2 and etc.
	+ x86: Intel Edison, Intel Joule, Ubuntu Linux, Windows PC(cygwin) and etc.

2. 최소 동작 환경
	+ CPU : 100MHz
	+ RAM : 5MB
	+ Flash memory : 5MB
	+ Connectivity : Ethernet, Wi-Fi, LTE

Source Tree
---
* [Source](https://github.com/SKT-ThingPlug/thingplug-device-sdk-C/tree/1.5_devel/linux)
+ __linux__ (ThingPlug SDK root folder)
	+ __samples__ (Device Middleware lite including makefiles for build)
		+ __MA__ (ManagementAgent)
		+ __SRA__ (ServiceReadyAgent)
		+ __SMA__ (SensorManagementAgent)
	+ __lib__ (prebuilt libraries including SDK and paho)
		+ __armeabi-v7a__ (Raspberry Pi, BeagleBone)
		+ __arm64-v8a__ (NVIDIA Jetson)
		+ __x86__ (Intel Edison)
		+ __x86_64__ (Intel Joule)
		+ __x86_64_pe__ (cygwin64)
	+ __include__ (Header files of the ThingPlug SDK)
	+ __src__ (Source files of the ThingPlug SDK)
		+ __oneM2M_V1_14__ (Source files of ThingPlug 1.5 oneM2M v1.14 SDK)
		+ __net__ (Source file of the MQTT)

SDK build
===
SDK를 빌드하면 lib/ 폴더에 libtplinuxsdk.a 파일이 생성된다. SDK를 수정할 필요가 없다면 target architecture별로 제공되는 prebuilt library 파일들을 그대로 사용하면 된다. 

1. 빌드

	```
	# make
	```
	
2. SDK 라이브러리(libtplinuxsdk.a) 빌드 확인

	```
	# ls lib/
	libtplinuxsdk.a
	```

Sample App build
===
SDK를 활용해볼 수 있는 Sample App으로서 Device Middleware lite 버전을 제공한다.
실행시키면 Configuration에 명시된 내용에 따라 자동으로 ThingPlug에 등록시키고,
가상 센서 데이터(온도, 습도, 조도 등)와 실제 시스템의 여유 메모리량을 ThingPlug로 매 10초마다 전송한다.

Configuration 설정(samples/Configuration.h)
---
1. Device 정보 입력
	+ __ONEM2M_AE_NAME__ : ThingPlug에 등록할 디바이스 ID (예: MAC주소 사용 시, "mydevice_0012BB45EE1D")
	+ __ONEM2M_SERVICE_ID__ : 디바이스를 등록할 서비스 ID (예: "smarthome")
	+ __ACCOUNT_USER_ID__ :  ThingPlug 계정 ID
	+ __ACCOUNT_CREDENTIAL_ID__ : ThingPlug 계정 CREDENTIAL-ID (포털 'My page/회원정보'에서 확인가능)
	

ThingPlug_oneM2M_SDK 빌드(samples/ThingPlug_oneM2M_SDK.c)
---
1. 빌드

	```
	# cd samples
	# make
	```
	
2. 실행

	```
	# output/ThingPlug_oneM2M_SDK
	```


	
Copyright (c) 2017 SK Telecom Co., Ltd. All Rights Reserved.
