#
#  "$Id: Makefile 378 2015-02-14 09:01:34Z hong_tao $"
#
#  Copyright (c)2014-2015, Hangzhou Ihardy Technology CO.LTD.
#  All Rights Reserved.
#
#	Description:
#	Revisions:		Year-Month-Day  SVN-Author  Modification
#


CROSS =

CPP	= @echo " g++ $@"; $(CROSS)g++
CC	= @echo " gcc $@"; $(CROSS)gcc
LD	= @echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
RM	= @echo " RM	$@"; rm -rf
STRIP	= @echo " strip  $@"; $(CROSS)strip

CFLAGS += -Wall -O -Os

ROOT_DIR = ../..

AFLAGS += -r

LIBDIR = ./lib/x86_64

BINDIR = ./bin/x86_64

OBJDIR = ./obj/x86_64

SRCDIR = ./
TESTDIR = ./

###################includeͷ�ļ�·��##################################
CFLAGS	+= -I ./



###################lib���ļ�·��##################################
#LDFLAGS +=  -lpthread

###################OBJ�ļ���·��############################################

LIB_SRCS_C = 		\
                i2c.c \
                test.c

LIB_OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C))



###################����Ŀ��###########################################
LIB_TARGET_A = $(LIBDIR)/libBigBox.a
LIB_TARGET_SO = $(LIBDIR)/libBigBox.so

TEST_TARGET = $(BINDIR)/test


TARGET	= $(TEST_TARGET)

.PHONY: all clean rebuild

all: $(TARGET)


$(TEST_TARGET):  $(LIB_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


$(OBJDIR)/%.o:$(SRCDIR)/%.c
	@[ ! -e $@ ] && mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)


$(OBJDIR)/%.o:$(TESTDIR)/%.c
	@[ ! -e $@ ] && mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)

rebuild: clean all

clean:
	$(RM) $(TARGET) $(OBJDIR);

