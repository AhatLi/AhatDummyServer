CC = g++

INCLUDEDIR = -I/usr/include/python3.8
LIBDIR = -L/usr/lib/python3.8/config-x86_64-linux-gnu 
LIBS = -lpthread -lpython3.8
ETC = -std=gnu++0x -g
OBJS = main.o DummyServer.o HTTPMessage.o
TARGET = AhatDummyServer

all : ${TARGET}

$(TARGET) : ${OBJS}
	$(CC) -o $@ $(OBJS) $(INCLUDEDIR) $(LIBDIR) $(LIBS) $(ETC)

%.o : %.cpp
	$(CC) -o $@ -c $(@:%.o=%.cpp) $(INCLUDEDIR) $(LIBDIR) $(LIBS) $(ETC)


clean :
	rm -f $(OBJS) ${TARGET}
