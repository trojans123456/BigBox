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
CROSS = arm-linux-gnueabihf-

CPP	= @echo " g++ $@"; $(CROSS)g++
CC	= @echo " gcc $@"; $(CROSS)gcc
LD	= @echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
RM	= @echo " RM	$@"; rm -rf
STRIP	= @echo " strip  $@"; $(CROSS)strip

###################项目路径和程序名称#################################
CFLAGS += -Wall -O -Os
CFLAGS += -D_REENTRANT -DNDEBUG

# LDFLAGS +=	-lpthread -static
#LDFLAGS += "-Wl"

AFLAGS += -r

LIBDIR = .

#BINDIR = ./bin/arm-linux-gnueabihf

OBJDIR=./obj/x86_64

SRCDIR = .
#TESTDIR = demo

###################include头文件路径##################################
CFLAGS	+= -I ../plugin.h
CFLAGS += -fpic
LFLAGS += -shared

###################lib库文件路径##################################
#LDFLAGS +=  -lpthread

###################OBJ文件及路径############################################

LIB_SRCS_C1 = 		\
                plugin_test1.c

LIB_SRCS_C2 = \
            plugin_test2.c

LIB_SRCS_C3 = \
            plugin_test3.c


#LIB_SRCS_CPP = 		\
#		src/timer.cpp	\
#		src/confile.cpp
	 	 
LIB_OBJS1=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C1))
LIB_OBJS2=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C2))
LIB_OBJS3=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C3))
#LIB_OBJS+=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(LIB_SRCS_CPP))


TEST_SRCS = \
		$(TESTDIR)/main.cpp
		
TEST_OBJS = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_SRCS))

###################编译目标###########################################
#LIB_TARGET = $(LIBDIR)/liblitepi.a
LIB_TARGET_SO1 = $(LIBDIR)/libplugin_test1.so
LIB_TARGET_SO2 = $(LIBDIR)/libplugin_test2.so
LIB_TARGET_SO3 = $(LIBDIR)/libplugin_test3.so

TEST_TARGET = $(BINDIR)/liblitepi_test

#TARGET	= $(LIB_TARGET) $(TEST_TARGET)
#TARGET	= $(LIB_TARGET) $(LIB_TARGET_SO)   $(TEST_TARGET)
TARGET = $(LIB_TARGET_SO1) $(LIB_TARGET_SO2) $(LIB_TARGET_SO3)

.PHONY: all clean rebuild

all: $(TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(AR) $(AFLAGS) $@ $^


$(LIB_TARGET_SO1): $(LIB_OBJS1)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@)
        $(CC) $(LFLAGS) -o $@ $^
        $(STRIP) $(LIB_TARGET_SO1)

$(LIB_TARGET_SO2): $(LIB_OBJS2)
        $(RM) $@;
        @[ ! -e $@ ] && mkdir -p $(dir $@)
        $(CC) $(LFLAGS) -o $@ $^
        $(STRIP) $(LIB_TARGET_SO2)

$(LIB_TARGET_SO3): $(LIB_OBJS3)
        $(RM) $@;
        @[ ! -e $@ ] && mkdir -p $(dir $@)
        $(CC) $(LFLAGS) -o $@ $^
        $(STRIP) $(LIB_TARGET_SO3)

$(TEST_TARGET):  $(EXTLIBS) $(TEST_OBJS) $(LIB_TARGET)
	$(CPP) -o $@ $^ $(LDFLAGS)
	

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)
		
$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(CPP) -c $(CFLAGS) $^ -o $@	 

$(OBJDIR)/%.o:$(TESTDIR)/%.cpp
	@[ ! -e $@ ] && mkdir -p $(dir $@)
	$(CPP) -c $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS)
	
rebuild: clean all

clean:
	$(RM) $(TARGET) $(OBJDIR);

