include out/config.mk

ARCH          ?= x64
SUFFIX        ?= $(ARCH)
HOST_OS       ?= $(shell uname|tr '[A-Z]' '[a-z]')
BRAND         ?=
SHARED        ?=
OS            ?= $(HOST_OS)
BUILDTYPE     ?= Release
V             ?= 0
CXX           ?= g++
LINK          ?= g++
ANDROID_SDK   ?= $(ANDROID_HOME)
ANDROID_LIB   ?= $(ANDROID_SDK)/platforms/android-24/android.jar
ANDROID_JAR   = out/android.classs.ngui.jar
JAVAC         ?= javac
JAR           = jar
ENV           ?=
NXP           = ./libs/nxp
NXP_OUT       = out/nxp
GYP           = $(NXP)/gyp/gyp
OUTPUT        ?= $(OS).$(SUFFIX).$(BUILDTYPE)
LIBS_DIR      = out/$(OUTPUT)
BUILD_STYLE   = make

#######################

STYLES		= make xcode msvs make-linux cmake-linux cmake
GYPFILES	= Makefile ngui.gyp tools/common.gypi out/config.gypi trial/trial.gypi
GYP_ARGS	= -Goutput_dir="out" \
-Iout/var.gypi -Iout/config.gypi -Itools/common.gypi -S.$(OS).$(SUFFIX) --depth=.

ifeq ($(V), 1)
	V_ARG = "V=1"
endif

ifeq ($(OS), android)
	BUILD_STYLE = make-linux
endif

gen_project=\
	echo "{'variables':{'project':'$(1)'}}" > out/var.gypi; \
	GYP_GENERATORS=$(1) $(GYP) -f $(1) $(2) --generator-output="out/$(1)" $(GYP_ARGS)

make_compile=\
	$(ENV) $(1) -C "out/$(BUILD_STYLE)" -f Makefile.$(OS).$(SUFFIX) \
	CXX="$(CXX)" LINK="$(LINK)" $(V_ARG) BUILDTYPE=$(BUILDTYPE) \
	builddir="$(shell pwd)/$(LIBS_DIR)"

.PHONY: $(STYLES) all build test2 clean

.SECONDEXPANSION:

###################### Build ######################

all: build # compile

# GYP file generation targets.
$(STYLES): $(GYPFILES)
	@$(call gen_project,$@,ngui.gyp)

build: $(BUILD_STYLE) # out/$(BUILD_STYLE)/Makefile.$(OS).$(SUFFIX)
	@$(call make_compile,$(MAKE))

test2: $(GYPFILES)
	@#make -C test -f test2.mk
	@$(call gen_project,$(BUILD_STYLE),test2.gyp)
	@$(call make_compile,$(MAKE))

$(ANDROID_JAR): android/org/ngui/*.java
	@mkdir -p out/android.classs
	@rm -rf out/android.classs/*
	@$(JAVAC) -bootclasspath $(ANDROID_LIB) -d out/android.classs android/org/ngui/*.java
	@cd out/android.classs; $(JAR) cfv ngui.jar .
	@mkdir -p $(NXP_OUT)/product/android/libs
	@cp out/android.classs/ngui.jar $(NXP_OUT)/product/android/libs

clean:
	@rm -rfv $(LIBS_DIR)
	@rm -rfv out/nxp/product/$(OS)
