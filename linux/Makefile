OUTPUT = ./lib
SDK_DIR = ./src

DEBUG = -DDEBUG_ENABLE#

FEATURE += $(DEBUG)

#CROSS_COMPILE = arm-linux-gnueabihf-
CROSS_COMPILE =
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
RANLIB = $(CROSS_COMPILE)ranlib

#c files
ONEM2M_SDK_OBJS = \
	$(SDK_DIR)/SKTtpDebug.o \
	$(SDK_DIR)/net/MQTTClient.o \
	$(SDK_DIR)/oneM2M.o \
	$(SDK_DIR)/oneM2M_V1_14/oneM2M_V1_14.o \

INC = -I./include 
LIBS = 
CFLAGS = -g -c -Wall $(FEATURE)
COBJS = $(ONEM2M_SDK_OBJS)
TARGET = libtplinuxsdk.a

all: $(TARGET)

$(TARGET): $(COBJS)
	$(AR) crv $(TARGET) $(COBJS)
	$(RANLIB) $(TARGET)
	mkdir -p $(OUTPUT)
	mv $(TARGET) $(OUTPUT)
	
.c.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@
	
clean :
	rm -rf $(ONEM2M_SDK_OBJS)
	rm -rf $(OUTPUT)
	
