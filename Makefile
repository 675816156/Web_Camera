CC := arm-linux-gcc
CXX := arm-linux-g++
STRIP := arm-linux-strip
OBJ :=main.o ./rtpsend/rtp_send.o ./x264encode/x264encode.o ./v4lcap/v4l.o
LIBS := -lx264 -ljrtp -ljthread -lm -lpthread
#LDFLAGS := 
CFLAGS := -g -c -Os -Wall
INCLUDES := -I/usr/local/include/jrtplib3/
TARGET := app-v4l
export CC CXX LIBS CFLAGS INCLUDES
unexport STRIP OBJ TARGET
all:$(TARGET)
${TARGET}:$(OBJ)
	@echo make target.
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)
#	$(STRIP) $(TARGET)
rtpsend/rtp_send.o:
	@echo make rtp model.
	cd ./rtpsend;$(MAKE)
x264encode/x264encode.o:
	@echo make x264 encode model.
	cd ./x264encode;$(MAKE)
v4lcap/v4l.o:
	@echo make capture model.
	cd ./v4lcap;$(MAKE)
main.o:main.c main.h
	$(CC) $(CFLAGS) $< -o $@
.PHONY:clean
clean:
	@echo make clean.
	rm -rf $(OBJ) $(TARGET)
	
