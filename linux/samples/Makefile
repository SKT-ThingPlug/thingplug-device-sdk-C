OUTPUT = ./output
SDK_DIR = ../src

DEBUG = -DDEBUG_ENABLE#

FEATURE += $(DEBUG)

#c files
ONEM2M_SDK_OBJS = $(SDK_DIR)/SKTtpDebug.o \
	$(SDK_DIR)/oneM2M.o \
	$(SDK_DIR)/net/MQTTClient.o \
	$(SDK_DIR)/oneM2M_V1/oneM2M_V1.o \
	$(SDK_DIR)/oneM2M_V1_12/oneM2M_V1_12.o \
	SDKVerification.o

INC = -I../../../../common -I../include 
LIBS = -L../lib -L/usr/local/lib
CFLAGS = -g -c -Wall $(FEATURE)
CXXFLAGS = -c -Wall -D__STDC_LIMIT_MACROS
COBJS = $(ONEM2M_SDK_OBJS)
LDFLAGS = -lpthread -lcrypto -lssl -lpaho-mqtt3as
TARGET = SDKVerification

all: $(TARGET)

$(TARGET): $(CXXOBJS) $(COBJS)
	$(CC) $(COBJS) $(INC) $(LIBS) $(LDFLAGS) -o $@
	mkdir -p $(OUTPUT)
	mv $(TARGET) $(OUTPUT)
	
.c.o:
	$(CC) $(CFLAGS) $(INC) $(LIBS)  $< -o $@
	
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC) $(LIBS)  $< -o $@

clean :
	rm -rf $(ONEM2M_SDK_OBJS)
	rm -rf $(OUTPUT)
	
