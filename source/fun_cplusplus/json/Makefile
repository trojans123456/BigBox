
CFLAGS += -Wall -O -Os
CFLAGS += -D_REENTRANT -DNDEBUG

LDFLAGS +=	-lpthread -static
#LDFLAGS += "-Wl"

AFLAGS += -r

LIBDIR = $(INSTALLDIR)

OBJDIR= $(OUTDIR)

SRCDIR = $(ROOTDIR)/core


###################include头文件路径##################################
CFLAGS	+= -I ./
CFLAGS += -fPIC
LFLAGS += -fPIC -shared

###################lib库文件路径##################################
LDFLAGS +=  -lpthread

###################OBJ文件及路径############################################

LIB_SRCS_C = 		\
                cJSON_p.c

LIB_SRCS_CPP = 		\
                jsoncpp.cpp
	 	 
LIB_OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SRCS_C))
LIB_OBJS+=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(LIB_SRCS_CPP))


TEST_SRCS = \

		
TEST_OBJS = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_SRCS))

###################编译目标###########################################
LIB_TARGET = $(LIBDIR)/libjson.a
LIB_TARGET_SO = $(LIBDIR)/libjson.so

TEST_TARGET =


TARGET	= $(LIB_TARGET) $(LIB_TARGET_SO)

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

