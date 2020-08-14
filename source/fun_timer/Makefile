#
#  "$Id: Makefile 378 2015-02-14 09:01:34Z hong_tao $"
#
#  Copyright (c)2014-2015, Hangzhou Ihardy Technology CO.LTD.
#  All Rights Reserved.
#
#	Description:	
#	Revisions:		Year-Month-Day  SVN-Author  Modification
#


###################编译选项及编译器###################################
CROSS =

CPP	= @echo " g++ $@"; $(CROSS)g++
CC	= @echo " gcc $@"; $(CROSS)gcc
LD	= @echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
RM	= @echo " RM	$@"; rm -rf
STRIP	= @echo " strip  $@"; $(CROSS)strip

###################项目路径和程序名称#################################
CFLAGS += -Wall -O -Os

ROOT_DIR = ../..

AFLAGS += -r

LIBDIR = ./lib/x86_64

BINDIR = ./bin/x86_64

OBJDIR = ./obj/x86_64

SRCDIR = ./
TESTDIR = ./

###################include头文件路径##################################
CFLAGS	+= -I ./



###################lib库文件路径##################################
#LDFLAGS +=  -lpthread

###################OBJ文件及路径############################################

LIB_SRCS_C = 		\
                minheap.c \
                timer_list.c \
                timerfd.c \
                timerfd_hash.c \
                test.c
	 	 
LIB_OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C))



###################编译目标###########################################
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

