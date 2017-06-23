OUTPUT = ./output

DEBUG = -DDEBUG_ENABLE#

FEATURE += $(DEBUG)

#c files
ONEM2M_SDK_OBJS = \
	./SRA/List.o \
	./SRA/TTVBuilder.o \
	./SRA/TTVManager.o \
	./SRA/SRA.o \
	./SMA/SMA.o \
	./MA/MA.o \
	./ThingPlug_oneM2M_SDK.o

INC = -I./include -I./ -I./SRA -I./SMA -I./MA 
LIBS = -L./lib -L/usr/local/lib
CFLAGS = -g -c -Wall $(FEATURE)
CXXFLAGS = -c -Wall -D__STDC_LIMIT_MACROS
COBJS = $(ONEM2M_SDK_OBJS)
LDFLAGS = -ltplinuxsdk -lpaho-mqtt3as -lssl -lcrypto -lpthread
TARGET = ThingPlug_oneM2M_SDK

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
