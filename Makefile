CC = g++
GCCVERSION = $(shell gcc --version | grep 4\\.4\\.)

INCDIR = inc 
LIBINCDIR = lib/ahatlogger/AhatLogger/inc 
OBJDIR = obj
SRCDIR = src
INCLUDEDIR = -I$(INCDIR) -I$(LIBINCDIR)
LIBDIR = lib/ahatlogger/AhatLogger/lib
LINKLIB = -L$(LIBDIR)
LIBS = -lpthread -lahatlogger
ETC = -std=gnu++0x -g
TARGET = AhatDummyServer

INCS := $(wildcard $(INCDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

ifeq "$(GCCVERSION)" ""
	ETC += -std=c++11
else
	ETC += -std=c++0x -D_GLIBCXX_USE_NANOSLEEP
endif

all : ${TARGET}

$(TARGET) : $(OBJDIR) $(OBJS) $(LIBDIR)
	$(CC) -o $@ $(OBJS) $(INCLUDEDIR) $(LINKLIB) $(LIBS) $(ETC)

$(OBJS) :  $(INCS) $(SRCS)
	$(CC)  -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@ $(INCLUDEDIR) $(LINKLIB) $(LIBS) $(ETC)

$(OBJDIR) :
	mkdir -p $(OBJDIR)

$(LIBDIR) :
	(cd lib/ahatlogger/AhatLogger && make)

clean :
	rm -rf $(OBJDIR) ${TARGET}