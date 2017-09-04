#**************************************************************************
#
#	Copyright (c) 2017  QNAP Electronics Inc.  All Rights Reserved.
#
#	FILE:
#		Makefile
#
#	Abstract:
#		Makefile for Qboost Library
#
#	HISTORY:
#**************************************************************************

ifdef NAS_LIB_PATH
$(shell export > ".build_env")
endif

ifeq (${NASWARE_PATH},)
$(error "NASWARE_PATH not defined")
endif

ifeq (${ROOT_PATH},)
$(error "ROOT_PATH not defined")
endif

CC = ${CROSS_COMPILE}gcc
CXX = ${CROSS_COMPILE}g++
CPP = ${CROSS_COMPILE}c++
STRIP = ${CROSS_COMPILE}strip
OBJCOPY = $(CROSS_COMPILE)objcopy

NAS_LIB_PATH = ${NASWARE_PATH}/NasLib
SYS_LIB_PATH = ${NASWARE_PATH}/SysLib
SYS_UTIL_PATH = ${NASWARE_PATH}/SysUtil

MARIADB_VERSION ?= 5.5.57
MARIADB_PATH := ../../../DataService/DBMS/mariadb-$(MARIADB_VERSION)

CFLAGS = -Wall -O2 -D${TARGET_PLATFORM}
INCLUDES = -I. -I${NAS_LIB_PATH}/include -I${NAS_LIB_PATH}/uLinux -I${NAS_LIB_PATH}/libnaslog-2.0.0 -I${NAS_LIB_PATH}/statistics -I${NAS_LIB_PATH}/common -I${SYS_TARGET_PREFIX}/usr/include -I${NAS_LIB_PATH}/ini_config -I$(MARIADB_PATH)/include
LIBS= -L${ROOT_PATH}/lib -L${ROOT_PATH}/usr/lib -L${ROOT_PATH}/usr/local/lib -L${TARGET_PREFIX}/usr/lib -luLinux_Util -luLinux_config -luLinux_NAS -luLinux_Storage -lssl -lcrypto -lpthread -L${MARIADB_PATH}/libmysql -lmysqlclient

ifeq ($(RECYCLE_EX),yes)
CFLAGS += -DRECYCLE_EX
endif

ifeq (${QNAP_HAL_SUPPORT},yes)
LIBS += -luLinux_ini -luLinux_hal
CFLAGS += -DQNAP_HAL_SUPPORT
ifeq (${STORAGE_V2},yes)
CFLAGS += -DSTORAGE_V2
INCLUDES += -I${NAS_LIB_PATH}/storage_man_v2
ifeq (${FOLDER_ENCRYPTION},ecryptfs)
CFLAGS += -DFOLDER_ENCRYPTION_ECRYPTFS -DFOLDER_ENCRYPTION
else ifeq (${FOLDER_ENCRYPTION},encfs)
CFLAGS += -DFOLDER_ENCRYPTION_ENCFS -DFOLDER_ENCRYPTION
endif
ifeq (${VIRTUAL_JBOD},yes)
CFLAGS += -DVIRTUAL_JBOD
endif
else
INCLUDES += -I${NAS_LIB_PATH}/storage_man
endif
endif

CFLAG = -fPIC -Wall -c -O2 -Wno-return-type

TARGET = libqrmh
OBJS = qrmh_mariadb.o

all: $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,${TARGET}.so.1 -o ${TARGET}.so.1.0.0 ${OBJS} ${LIBS}
	${STRIP} ${TARGET}.so.1.0.0
	ln -sf $(TARGET).so.1.0.0  $(TARGET).so

%.o: %.c *.h
	$(CC) $(CFLAGS) $(CFLAG) $< -o $@ $(INCLUDES)

.PHONY: clean

clean:
	sudo rm -f *so*
	sudo rm -f *.o
	sudo rm -f $(TARGET).so*

test_clean:
	sudo rm -f *.o
