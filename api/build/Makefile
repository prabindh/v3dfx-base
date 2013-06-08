# Makefile to build v3dfx-base.a core library (static)
# Invoke as below
#   PATH=/home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH GFXSDKDIR=/home/prabindh/Graphics_SDK_4_05_00_03/ make
#   PATH=/home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH GFXSDKDIR=/home/prabindh/Graphics_SDK_4_06_00_01/ make
#   PATH="/opt/linaro/gcc-linaro-arm-linux-gnueabi-2012.03-20120326_linux/bin/:$PATH" CROSS_COMPILE=arm-linux-gnueabi- GFXSDKDIR=/home/prabindh/Graphics_SDK_4_08_00_01 make
# prabu@ti.com 2013

.phony: clean

PLAT_CC  = $(CROSS_COMPILE)gcc
PLAT_CPP = $(CROSS_COMPILE)g++
PLAT_AR  = $(CROSS_COMPILE)ar

#Requires Graphics SDK 4.06 or beyond with eglimage support
_ENABLE_EGLIMAGE := 1


COMMON_INCLUDES := $(addprefix -I, \
		$(GFXSDKDIR)/GFX_Linux_KM/services4/3rdparty/bufferclass_ti \
		$(GFXSDKDIR)/GFX_Linux_KM/include4 \
		$(GFXSDKDIR)/include/OGLES2 \
		../include )
AR_FLAGS := rcs

ifeq ($(_ENABLE_EGLIMAGE),1)
	EGLIMAGE_FLAGS="-D_ENABLE_EGLIMAGE"
endif

CC_FLAGS := -Wall -DRELEASE -O2 $(COMMON_INCLUDES) $(EGLIMAGE_FLAGS)

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

