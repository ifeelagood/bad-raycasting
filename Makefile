CC = g++
CCFLAGS := -lGL -lGLU -lglut -fopenmp

SRC_PATH := src
BIN_PATH := bin
INC_PATH := include

TARGET_NAME := raycaster
TARGET := $(BIN_PATH)/$(TARGET_NAME)

SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cpp)))

default: makedir all

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CCFLAGS) -I$(INC_PATH)


.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)
