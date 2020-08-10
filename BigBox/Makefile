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
# CFLAGS += -D_REENTRANT -DNDEBUG

#LDFLAGS +=	-lpthread -static
#LDFLAGS += "-Wl"

AFLAGS += -r

LIBDIR = ./lib/x86_64

BINDIR = ./bin/x86_64

OBJDIR = ./obj/x86_64

SRCDIR = src
TESTDIR = demo

###################include头文件路径##################################
CFLAGS	+= -I ./include
CFLAGS += -fpic
LFLAGS += -shared

###################lib库文件路径##################################
#LDFLAGS +=  -lpthread

###################OBJ文件及路径############################################

LIB_SRCS_C = 		\
                src/avl.c \
                src/alloc.c \
                src/list_safe.c \
                src/kvlist.c \
		src/event.c

	 	 
LIB_OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C))



TEST_SRCS = \
                $(TESTDIR)/main.c
		
TEST_OBJS = $(patsubst $(TESTDIR)/%.c, $(OBJDIR)/%.o, $(TEST_SRCS))

###################编译目标###########################################
LIB_TARGET_A = $(LIBDIR)/libBigBox.a
LIB_TARGET_SO = $(LIBDIR)/libBigBox.so

TEST_TARGET = $(BINDIR)/main

#TARGET	= $(LIB_TARGET) $(TEST_TARGET)
TARGET	= $(LIB_TARGET_A) $(LIB_TARGET_SO)  $(TEST_TARGET)

.PHONY: all clean rebuild

all: $(TARGET)

$(LIB_TARGET_A): $(LIB_OBJS)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(AR) $(AFLAGS) $@ $^


$(LIB_TARGET_SO): $(LIB_OBJS)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@)
	$(CPP) $(LFLAGS) -o $@ $^
	$(STRIP) $(LIB_TARGET_SO)

$(TEST_TARGET):  $(EXTLIBS) $(TEST_OBJS) $(LIB_TARGET_A)
	$(CC) -o $@ $^ $(LDFLAGS)
	

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)
		
$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(CPP) -c $(CFLAGS) $^ -o $@	 

$(OBJDIR)/%.o:$(TESTDIR)/%.c
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)
	
rebuild: clean all

clean:
	$(RM) $(TARGET) $(OBJDIR);

