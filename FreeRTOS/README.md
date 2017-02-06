FreeRTOS (+TLS)
===

지원 사양
---
1. Footprint
	+ CPU : 50MHz 이상
	+ RAM : 64KB 이상
	+ Flash memory : 256KB 이상
2. 테스트 환경
 1. Main board
	+ STMicroelectronics Nucleo-64 F411RE
	![NUCLEO-F411RE.jpg](images/NUCLEO-F411RE.jpg "" "width:50%;")
		+ CPU : 100MHz ARM 32-bit Cortex-M4 with FPU
		+ RAM : 128KB
		+ Flash memory : 512KB
 2. Ethernet board
	+ WIZnet W5500
    ![w5500_ethernet_shield.jpg](images/w5500_ethernet_shield.jpg "" "width:50%;")

Source Tree
---
* [Source](https://github.com/SKT-ThingPlug/thingplug-device-middleware/tree/master/SDK/FreeRTOS)
+ __FreeRTOS__ (project root)
	+ __Drivers__
		+ __CMSIS__
		+ __STM32F4xx_HAL_Driver__ (Nucleo F411RE driver)
	+ __Inc__ (FreeRTOS 및 WIZnet driver configuration headers)
	+ __Library__ (libraries)
		+ __Ethernet_WIZnet__ (WIZnet library)
			+ __Ethernet__ (WIZnet driver 및 socket source)
			+ __Internet__ (internet 통신 source)
				+ __DHCP__ (DHCP 통신 처리)
				+ __DNS__ (DNS 통신 처리)
				+ __SNTP__ (SNTP 통신 처리)
		+ __mbedtls_2_3_0__ (mbed TLS library)
			+ __include__ (library headers)
			+ __library__ (TLS library source)
		+ __Mqtt_Paho__ (mqtt library, TLS library 및 WIZnet socket 연결)
			+ __MQTTPacket__ (MQTT packet 처리 source)
		+ __oneM2M__ (oneM2M library)
			+ __include__ (library headers)
			+ __src__ (oneM2M library source)
				+ __net__ (MQTT handler)
				+ __oneM2M_V1__ (ThingPlug oneM2M v1 API)
				+ __oneM2M_V1_12__ (ThingPlug oneM2M v1.12 API)
	+ __Middlewares__
		+ __Third_Party__
			+ __FreeRTOS__ (FreeRTOS source)
	+ __Src__ (FreeRTOS main)
		+ __oneM2M__ (test sample)
			+ __MA__ (ManagementAgent)
			+ __SMA__ (SensorManagementAgent)
			+ __SRA__ (ServiceReadyAgent)

Project build
===

IDE 설정
---
1. Atollic TrueSTUDIO for ARM
	+ http://atollic.com/resources/downloads/
	+ FreeRTOS homepage에서 추천하고 있음, Eclipse 기반의 embeded 환경 프로그램 개발 tool
2. STM32CubeMX
 1. Main board device driver와 FreeRTOS를 포함한 project templete을 생성할 수 있음
	+ download - http://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-configurators-and-code-generators/stm32cubemx.html
	+ 설치 시 아래와 같은 오류 발생 시 command line에서 다음 명령으로 실행
		~~~
		> java -jar '설치파일명'
		~~~
		![Java_Runtime_Environment_error.png](images/Java_Runtime_Environment_error.png "" "width:50%;")
 2. 설치 후 실행해서 New Project 선택
	![STM32CubeMX_excute.png](images/STM32CubeMX_excute.png "" "width:50%;")
	+ 실행 시 아래와 같은 오류 발생 시 command line에서 다음 명령으로 실행
		~~~
		> java -jar '실행파일명'
		~~~
		![Java_Runtime_Environment_error2.png](images/Java_Runtime_Environment_error2.png "" "width:50%;")
 3. Board Selector 탭에서 Vendor:STMicroelectronics, Type of Board:Nucleo64, MCU Series:STM32F4로 설정한 다음 아래 오른쪽 목록에서 NUCLEO-F411RE 선택하고 아래 OK 버튼 클릭
	![STM32CubeMX_new_project.png](images/STM32CubeMX_new_project.png "" "width:50%;")
 4. 메뉴에서 Help > Install New Libraries를 선택한 후 STM32CubeF4 Releases의 Firmware Package for Family STM32F4를 선택한 다음 Install Now 버튼 클릭
	![STM32CubeMX_install_library.png](images/STM32CubeMX_install_library.png "" "width:50%;")
	+ library 설치 실패 시 수동 설치 방법
		+ download - http://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-embedded-software/stm32cubef4.html
		+ 위의 library 설치 팝업에서 하단의 From Local... 버튼 클릭 후 다운로드 받은 파일 선택
 5. 왼쪽 Configuration 리스트 수정
	+ '/Middlewares/FREERTOS'의 'Enabled' 체크
	+ '/Peripherals/SPI2'의 'Mode'를 'Full-Duplex Master'로 설정
	+ '/Peripherals/USART2'의 'Mode를 'Asynchronous'로 설정
	![STM32CubeMX_freeRTOS.png](images/STM32CubeMX_freeRTOS.png "" "width:50%;")
 6. 메뉴에서 Project > Generate Code를 선택한 후 'Project Name', 'Project Location'을 입력하고 'Toolchain / IDE'를 TrueSTUDIO로 설정 후 OK 버튼 클릭
	![STM32CubeMX_project_settings.png](images/STM32CubeMX_project_settings.png "" "width:50%;")
 7. 생성된 project template을 TrueSTUDIO에서 로드
 8. 참고 site
	+ http://igotit.tistory.com/entry/STM32-%EB%9D%BC%EC%9D%B4%EB%B8%8C%EB%9F%AC%EB%A6%AC-%ED%99%95%EB%B3%B4%EB%B0%A9%EB%B2%95%EB%B0%8F-%ED%99%9C%EC%9A%A9%EB%B2%95-%EA%B0%80%EC%9D%B4%EB%93%9CSTM32CubeMX
	+ http://igotit.tistory.com/159

Library import
---
1. FreeRTOS
 1. Library download
	+ STM32CubeMX tool에서 생성된 code 사용
2. project root에 '/Library' directory 생성
3. WIZnet ioLibrary
 1. Library download
	+ https://github.com/Wiznet/ioLibrary_Driver
 2. '/Library/Ethernet_WIZnet' directory 생성
 3. 사용 code 추출
	+ '/Library/Ethernet_WIZnet' directory에 library의 'Ethernet' directory 복사
	+ '/Library/Ethernet_WIZnet' directory에 library의 'Internet' directory 복사
		+ 'DHCP', 'DNS', 'SNTP' directory만 사용하고 나머지 directory는 삭제
 4. handling 파일 추가
	+ Source Tree의 '/Library/Ethernet_WIZnet' directory의 'Board.h', 'W5500HardwareDriver.h', 'W5500HardwareDriver.c', 'wizSystem.h', 'wizSystem.c' 파일을 '/Library/Ethernet_WIZnet' directory에 복사
 5.  DNS 환경 설정 변경
	+ '/Library/Ethernet_WIZnet/Internet/DNS' directory의 'dns.h' 파일의 MAX_DOMAIN_NAME 값을 '16'에서 '32'로 변경 (ThingPlug domain name 길이가 16보다 크기 때문임)
4. MQTT
 1. Library download
	+ https://github.com/eclipse/paho.mqtt.embedded-c
 2. '/Library/Mqtt_Paho' directory 생성
 3. '/Library/Mqtt_Paho/MQTTPacket' directory 생성
 4. 사용 code 추출
	+ '/Library/Mqtt_Paho/MQTTPacket' directory에 library의 '/MQTTPacket/src' directory의 파일 복사
	+ 'MQTTClient-C' directory
		+ '/Library/Mqtt_Paho' directory에 library의 '/MQTTClient-C/src' directory의 'MQTTClient.h', 'MQTTClient.c' 파일을 복사
		+ '/Library/Mqtt_Paho' directory에 library의 '/MQTTClient-C/src/FreeRTOS' directory의 'MQTTFreeRTOS.h', 'MQTTFreeRTOS.c' 파일을 복사
 4. FreeRTOS 연동 수정
	+ Source Tree의 '/Library/Mqtt_Paho' directory의 'MQTTFreeRTOS.h', 'MQTTFreeRTOS.c' 파일과 비교해서 FreeRTOS+TCP library 사용 부분을 WIZnet ioLibrary 사용으로 변경
 5. TLS 지원 파일 추가
	+ Source Tree의 '/Library/Mqtt_Paho' directory의 'MQTTFreeRTOSTLS.h', 'MQTTFreeRTOSTLS.c' 파일을 '/Library/Mqtt_Paho' directory에 복사
	+ '/Library/Mqtt_Paho' directory의 'MQTTClient.h' 파일에 '#include "MQTTFreeRTOSTLS.h"' line 추가
 6. 중복 정의 제거
	+ '/Library/Mqtt_Paho' directory의 'MQTTClient.h' 파일에 있는 enum returnCode 중 'SUCCESS'가 chip driver에 이미 정의돼 있음
		+ /Drivers/CMSIS/Device/ST/STM32F4xx/Include directory의 'stm32f4xx.h' 파일의 ErrorStatus enum 정의 제거(코드가 사용되는 곳이 없어서 영향이 없음)
5. mbedtls
 1. Library download
	+ https://tls.mbed.org/
 2. '/Library/mbedtls_2_3_0' directory 생성
 3. 사용 code 추출
	+ '/Library/mbedtls_2_3_0' directory에 library의 'include' directory 복사
	+ '/Library/mbedtls_2_3_0' directory에 library의 'library' directory 복사
 4. config.h 변경
	+ Source Tree의 '/Library/mbedtls_2_3_0/SSL_Config.h' 파일을 mbedtls library directory 아래의 include directory에 있는 config.h 파일에 덮어씀
6. oneM2M
 1. Library import
	+ Source Tree의 '/Library/oneM2M' directory를 '/Library' directory에 복사
 2. oneM2MConfig.h 수정
	+ __DEBUG_ENABLE__ : define 됐을 경우 debug log 활성화 됨
	+ __IMPORT_TLS_LIB__ : define 됐을 경우 TLS library가 포함됨
	+ __MAC_ADDRESS__ : 사용할 mac address 지정(다른 device와 겹치면 안됨)

Sample code import
---
1. Code import
	+ Source Tree의 '/Src/oneM2M' directory를 '/Src' directory에 복사
2. '/Src/oneM2M/Configuration.h' 수정
	+ __ONEM2M_V1_12__ : define 됐을 경우 oneM2M v1.12가 활성화 됨
	+ __MQTT_ENABLE_SERVER_CERT_AUTH__ : 0(Normal socket 사용), 1(TLS socket 사용)
	+ __ACCOUNT_USER__ : ThingPlug 계정의 UKEY
	+ __ACCOUNT_PASSWORD__ : ThingPlug 계정의 Password
	+ __ONEM2M_NODEID__ : ThingPlug 계정에 등록된 Device ID
3. '/Src/main.c' 수정
	+ Source Tree의 '/Src/main.c' 파일과 비교해서 main() 함수에 WIZnet library 초기화 등의 code 추가
	+ serial port speed 변경
		+ '/Src/main.c' 파일에서 MX_USART2_UART_Init() 함수의 huart2.Init.BaudRate = 115200; 수정
4. '/Src/oneM2M/ThingPlug_oneM2M_SDK.cpp' 수정
 1. 실행 대상 변경
	1. SDK verification을 실행할 경우
		+ ThingPlug_oneM2M_SDK() function 내의 SDKVerificationMain(); 활성화
	2. MA를 실행할 경우
		+ ThingPlug_oneM2M_SDK() function 내의 MARun(); 활성화

Library path 설정
---
1. include path 설정
	+ 메뉴 > 프로젝트 > 특성 > C/C++ General > Includes tab
	![TrueSTUDIO_setting_include.png](images/TrueSTUDIO_setting_include.png "" "width:50%;")
2. source path 설정
	+ 메뉴 > 프로젝트 > 특성 > C/C++ General > Source Location tab
	![TrueSTUDIO_setting_source.png](images/TrueSTUDIO_setting_source.png "" "width:50%;")

Build & Run
---
1. Build
	+ 메뉴 > 프로젝트 > Build Project
	![TrueSTUDIO_compile.png](images/TrueSTUDIO_compile.png "" "width:50%;")
2. Run
	+ build 후 '/Debug' directory에 생성 된 .elf 파일을 사용해서 다음 명령으로 .bin 생성
	~~~
	> C:\Program Files (x86)\Atollic\TrueSTUDIO for ARM 6.0.0\ARMTools\bin\arm-atollic-eabi-objcopy.exe -O binary ${project_name}.elf ${project_name}.bin
	~~~
	+ 생성된 .bin 파일을 윈도우 탐색기에서 'NODE_F411RE'로 연결된 device drive에 복사
3. 실행 로그 확인
	+ Serial 통신 프로그램(ex. Putty, Tera Term 등)을 사용해서 프로그램 로그 확인
	(※ sample code의 main.c 파일에서 지정한 speed와 맞춰 환경을 설정해야 글자가 깨져 보이지 않음)
	![serial_log.png](images/serial_log.png "" "width:50%;")

Copyright (c) 2016 SK Telecom Co., Ltd. All Rights Reserved.