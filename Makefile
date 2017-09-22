
CURDIR := .
CC=gcc

TARGET=step_count

CFLAGS := -g
LFLAGS := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib
#LFLAGS += -lavcodec -lavformat -lavutil -lswscale
LFLAGS += -lpthread
#-lavdevice -lavfilter -lswresample
#LFLAGS += -lx264

SRC := $(wildcard $(CURDIR)/*.c)

all:$(TARGET)

$(TARGET):$(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LFLAGS)

clean:
	rm -rf *.o $(TARGET)
