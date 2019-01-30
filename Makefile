CC = g++

INCLUDEDIR = 
LIBDIR = 
LIBS = -pthread
ETC = -std=gnu++0x -g
OBJS = main.o DummyServer.o
TARGET = AhatDummyServer

all : ${TARGET}

$(TARGET) : ${OBJS}
	$(CC) -o $@ $(OBJS) $(INCLUDEDIR) $(LIBDIR) $(LIBS) $(ETC)

%.o : %.cpp
	$(CC) -o $@ -c $(@:%.o=%.cpp) $(INCLUDEDIR) $(LIBDIR) $(LIBS) $(ETC)


clean :
	rm -f $(OBJS) ${TARGET}
