export CC = g++
export MAKE = /usr/bin/make
export User = $(shell whoami)
export USER_HOME = /home/$(USER)
export CLPIM_BASE = $(shell pwd)
export CLPIM_COMMON = $(CLPIM_BASE)/common
export CLPIM_INCLUDE = $(CLPIM_BASE)/include
export CLPIM_LIB = $(CLPIM_BASE)/lib
export XIAODB_INCLUDE = $(CLPIM_BASE)/XiaoDB
export CXXFLAGS_DEBUG = -Wall -g -D_DEBUG
export CXXFLAGS_RELEASE	= -Wall -DNDEBUG
# Enable Debug Mode
export CXXFLAGS	= $(CXXFLAGS_DEBUG)
export CXXFLAGS_SHARED_LIB = $(CXXFLAGS) -fPIC -c
export LIBS = -L./ -L$(CLPIM_LIB) -lclpim
export LIB_PATH = -L./ -L$(CLPIM_LIB)
export INCLUDE = -I$(CLPIM_INCLUDE) -I$(CLPIM_COMMON) -I$(XIAODB_INCLUDE)
export PLUGIN_DIR = $(USER_HOME)/.CLPIM/plugin


ALL: BIN COMMON PLUGIN LIB XIAODB

BIN: COMMON PLUGIN LIB XIAODB
	$(MAKE) -C bin

COMMON:
	$(MAKE) -C common

PLUGIN: LIB
	$(MAKE) -C plugins/RSSReader

LIB:
	$(MAKE) -C lib

XIAODB:
	$(MAKE) -C XiaoDB

clean:
	$(MAKE) -C $(CLPIM_BASE)/common clean
	$(MAKE) -C $(CLPIM_BASE)/lib clean
	$(MAKE) -C plugins/RSSReader clean
	$(MAKE) -C XiaoDB clean

tests: COMMON LIB
	$(MAKE) -C $(CLPIM_BASE)/common tests
	$(MAKE) -C XiaoDB tests
