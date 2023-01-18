SRC_DIR = src
INT_DIR = build
TARGET = phase_1

CXX = gcc

CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -g

ALL_FILES := $(wildcard $(SRC_DIR)/**/**/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
ALL_HEADER_FILES := $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/**/**/*.h)
CPP_FILES := $(filter-out %.test.cpp, $(ALL_FILES))

ALL_OBJ_FILES := $(ALL_FILES:$(SRC_DIR)/%.c=$(INT_DIR)/%.o)
C_OBJ_FILES := $(filter %.o, $(ALL_OBJ_FILES))

all: info $(TARGET)

info: 
	@echo "SRC_DIR: $(SRC_DIR)"
	@echo "INT_DIR: $(INT_DIR)"
	@echo "TARGET: $(TARGET)"
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "ALL_FILES: $(ALL_FILES)"
	@echo "ALL_HEADER_FILES: $(ALL_HEADER_FILES)"
	@echo "ALL_OBJ_FILES: $(ALL_OBJ_FILES)"

clean:
	@echo -e "RMRG\build $(TARGET)"
	@rm -rf build $(TARGET)

run: all
	@echo -e "RUN\t$(TARGET)"
	@./$

$(TARGET): $(C_OBJ_FILES)
	@echo -e "LD\t$@"
	@$(CXX) $^ $(LDFLAGS) -o $@