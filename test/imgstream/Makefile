# Makefile to build v3dfx-base-test-imgstream
# part of https://gitorious.org/v3dfx-base
# Invoke as below
#   PATH=/home/prabindh/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH GFXSDKDIR=/home/prabindh/Graphics_SDK_4_05_00_03/ make
# prabu@ti.com 2012

.phony: clean

CROSS_COMPILE = arm-arago-linux-gnueabi-

PLAT_CC  = $(CROSS_COMPILE)gcc
PLAT_CPP = $(CROSS_COMPILE)g++
PLAT_AR  = $(CROSS_COMPILE)ar

ENABLE_CMEM=1
CMEM_PATH = /home/prabindh/work1/cmem/cmem
CMEM_LIBDIR = /home/prabindh/work1/cmem/cmem/lib

COMMON_INCLUDES := $(addprefix -I, \
		$(GFXSDKDIR)/GFX_Linux_KM/services4/3rdparty/bufferclass_ti \
		$(GFXSDKDIR)/GFX_Linux_KM/include4 \
		$(GFXSDKDIR)/include/OGLES2 \
		$(CMEM_PATH)/include \
		../../api/include )
LD_FLAGS := -L$(GFXSDKDIR)/gfx_rel_es5.x \
		-L$(CMEM_LIBDIR) \
		-Wl,--rpath-link,$(GFXSDKDIR)/gfx_rel_es5.x \
		-lEGL -lGLESv2
CC_FLAGS := -Wall -DRELEASE -O2 $(COMMON_INCLUDES) -D_ENABLE_CMEM


TEST_CPP_FILES := $(wildcard *.cpp)
TEST_OBJ_FILES := $(patsubst %.cpp,%.o,$(TEST_CPP_FILES))
v3dfx-base-test-imgstream: $(TEST_OBJ_FILES)
	$(PLAT_CPP) $(LD_FLAGS) -o $@ $^ $(CMEM_LIBDIR)/cmem.a470MV \
	 ../../api/build/v3dfx-base.a

%.o: %.cpp
	$(PLAT_CPP) $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf *.o
	rm v3dfx-base-test-imgstream

