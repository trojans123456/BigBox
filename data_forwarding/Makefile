#
#  "$Id: Makefile 378 2016-08-06 18:01:34Z hong_tao $"
#
#  Copyright (c)2015-2016, Beijing HuTong Technology CO.LTD.
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
CFLAGS += -D_REENTRANT -DNDEBUG

#LDFLAGS +=	-lpthread -static
#LDFLAGS += "-Wl"

AFLAGS += -r

LIBDIR = ./lib/arm-linux-gnueabihf

BINDIR = ./bin/arm-linux-gnueabihf

OBJDIR=./obj/arm-linux-gnueabihf

SRCDIR = src
TESTDIR = demo

###################include头文件路径##################################
CFLAGS	+= -I ./include/litepi
CFLAGS += -fpic
LFLAGS += -shared

###################lib库文件路径##################################
LDFLAGS +=  -lpthread

###################OBJ文件及路径############################################

LIB_SRCS_C = 		\
		src/gettimeofday.c			\
		src/usbcom.c				\
		src/flexible_buffer.c			\
		src/ioengine.c			\
		src/ios_epoll.c			\
		src/tcpserver.c		\
		src/base64.c		\
		src/md5.c			\
		src/sha1.c			\
		src/serv_common.c			\
		src/sox.c			\
		src/log.c			\
		src/debug.c			\
		src/litepi.c

LIB_SRCS_CPP = 		\
		src/timer.cpp	\
		src/confile.cpp		
	 	 
LIB_OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C))
LIB_OBJS+=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(LIB_SRCS_CPP))




###################编译目标###########################################
#LIB_TARGET = $(LIBDIR)/liblitepi.a
#LIB_TARGET_SO = $(LIBDIR)/liblitepi.so

TEST_TARGET = $(BINDIR)/main


TARGET = $(TEST_TARGET)

.PHONY: all clean rebuild

all: $(TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@) 
	$(AR) $(AFLAGS) $@ $^


$(LIB_TARGET_SO): $(LIB_OBJS)
	$(RM) $@;
	@[ ! -e $@ ] && mkdir -p $(dir $@)
	$(CPP) $(LFLAGS) -o $@ $^
	$(STRIP) $(LIB_TARGET_SO)

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

