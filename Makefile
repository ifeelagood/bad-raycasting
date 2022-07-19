CC = g++
CCFLAGS = -O3 -lSDL2 -fopenmp -lpng -lz

SRC_PATH = src
BIN_PATH = bin
INC_PATH = include

TARGET_NAME = raycaster
TARGET = $(BIN_PATH)/$(TARGET_NAME)

SRC = $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cpp)))

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) -I$(INC_PATH) $(CCFLAGS)
