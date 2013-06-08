# Makefile to build v3dfx-base-test-eglimage
# Invoke as below
#   PATH=/home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH GFXSDKDIR=/home/prabindh/Graphics_SDK_4_05_00_03/ make
# PATH="/opt/linaro/gcc-linaro-arm-linux-gnueabi-2012.03-20120326_linux/bin/:$PATH" CROSS_COMPILE=arm-linux-gnueabi- GFXSDKDIR=/home/prabindh/Graphics_SDK_4_08_00_01 make
# prabu@ti.com 2013

.phony: clean

PLAT_CC  = $(CROSS_COMPILE)gcc
PLAT_CPP = $(CROSS_COMPILE)g++
PLAT_AR  = $(CROSS_COMPILE)ar

ENABLE_CMEM=1
#CMEM_PATH = /home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem
CMEM_PATH=/home/prabindh/work1/linuxutils_3_23_00_01/packages/ti/sdo/linuxutils/cmem
CMEM_LIBDIR = $(CMEM_PATH)/lib

_ENABLE_EGLIMAGE := 1

COMMON_INCLUDES := $(addprefix -I, \
		$(GFXSDKDIR)/GFX_Linux_KM/include4 \
		$(GFXSDKDIR)/include/OGLES2 \
		$(CMEM_PATH)/include \
		../../api/include )
LD_FLAGS := -L$(GFXSDKDIR)/gfx_rel_es8.x \
		-L$(CMEM_LIBDIR) \
		-Wl,--rpath-link,$(GFXSDKDIR)/gfx_rel_es8.x \
		-lEGL -lGLESv2
CC_FLAGS := -Wall -DRELEASE -O2 $(COMMON_INCLUDES) -D_ENABLE_CMEM

TEST_CPP_FILES := $(wildcard *.cpp)
TEST_OBJ_FILES := $(patsubst %.cpp,%.o,$(TEST_CPP_FILES))
v3dfx-base-test-eglimage: $(TEST_OBJ_FILES)
	$(PLAT_CPP) $(LD_FLAGS) -o $@ $^ $(CMEM_LIBDIR)/cmem.a470MV \
	 ../../api/build/v3dfx-base.a

%.o: %.cpp
	$(PLAT_CPP) $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf *.o
	rm v3dfx-base-test-eglimage

