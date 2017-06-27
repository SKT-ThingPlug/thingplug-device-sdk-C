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
		+ __SMA__ (SensorManagementAgent)
		+ __SRA__ (ServiceReadyAgent)
	+ __lib__ (prebuilt libraries including SDK and paho)
	+ __include__ (Header files of the ThingPlug SDK)
	+ __src__ (Source files of the ThingPlug SDK)
		+ __oneM2M_V1_14__ (Source files of ThingPlug 1.5 oneM2M v1.14 SDK)
		+ __net__ (Source file of the MQTT)

SDK build
===
1. 빌드

	```
	# make
	```
	
2. 빌드 클리어

	```
	# make clean
	```
	
3. SDK 라이브러리(libtplinuxsdk.a) 빌드 확인

	```
	# ls lib/
	libtplinuxsdk.a
	```

Sample App build
===

Configuration 설정(samples/Configuration.h)
---
1. Device 정보 입력
	+ __ONEM2M_AE_NAME__ : ThingPlug에 등록할 디바이스 ID (예: MAC주소 사용 시, "mydevice_0012BB45EE1D")
	+ __ONEM2M_SERVICE_ID__ : 디바이스를 등록할 서비스 ID (예: "smarthome")
	+ __ACCOUNT_USER_ID__ :  ThingPlug 계정 ID
	+ __ACCOUNT_CREDENTIAL_ID__ : ThingPlug 계정 CREDENTIAL-ID (포털 'My page/회원정보'에서 확인가능)
	

ThingPlug_oneM2M_SDK 빌드(oneM2M/samples/ThingPlug_oneM2M_SDK.c)
---
1. 빌드

	```
	# cd samples
	# make
	```
	
2. 빌드 클리어

	```
	# make clean
	```
	
3. 실행

	```
	# output/ThingPlug_oneM2M_SDK
	```


	
Copyright (c) 2017 SK Telecom Co., Ltd. All Rights Reserved.
