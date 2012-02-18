# Makefile to build v3dfx-base.a core library (static)
# part of https://gitorious.org/v3dfx-base
# Invoke as below
#   PATH=/home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH GFXSDKDIR=/home/prabindh/Graphics_SDK_4_05_00_03/ make
# prabu@ti.com 2012

.phony: clean

CROSS_COMPILE = arm-arago-linux-gnueabi-

PLAT_CC  = $(CROSS_COMPILE)gcc
PLAT_CPP = $(CROSS_COMPILE)g++
PLAT_AR  = $(CROSS_COMPILE)ar

COMMON_INCLUDES := $(addprefix -I, \
		$(GFXSDKDIR)/GFX_Linux_KM/services4/3rdparty/bufferclass_ti \
		$(GFXSDKDIR)/GFX_Linux_KM/include4 \
		$(GFXSDKDIR)/include/OGLES2 \
		../include )
AR_FLAGS := rcs
CC_FLAGS := -Wall -DRELEASE -O2 $(COMMON_INCLUDES)


API_CPP_FILES := $(wildcard ../src/*.cpp)
API_OBJ_FILES := $(patsubst %.cpp,%.o,$(API_CPP_FILES))

v3dfx-base.a : $(API_OBJ_FILES)
	$(PLAT_AR) $(AR_FLAGS) $@ $^

%.o: %.cpp
	$(PLAT_CPP) $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf *.o
	rm -rf ../src/*.o
	rm v3dfx-base.a

