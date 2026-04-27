PROJECT := cint

CC := gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror \
         -Wconversion -Wsign-conversion \
         -Wshadow -Wformat=2 -Wundef -std=gnu11

target ?= debug
TARGET := $(target)
include $(PWD)/Make/$(TARGET).mk

BUILD_PATH := $(PWD)/Build
BUILD_DIR := $(BUILD_PATH)/$(TARGET)
SRC_BASE := $(PWD)/Code
OBJ_BASE := $(BUILD_DIR)/Modules
OUTPUT_PROJECT_PATH := $(BUILD_DIR)/$(PROJECT)

MODULES :=  Entrypoint \
			Lexer \
			Prompt

SRC_DIRS := $(addprefix $(SRC_BASE)/,$(MODULES))
SRCS     := $(shell find $(SRC_DIRS) -name "*.c")
HDRS     := $(shell find $(SRC_DIRS) -name "*.h")

OBJS     := $(patsubst $(SRC_BASE)/%.c,$(OBJ_BASE)/%.o,$(SRCS))

PREPS    := $(patsubst $(SRC_BASE)/%.c,$(OBJ_BASE)/%.c.i,$(SRCS)) \
            $(patsubst $(SRC_BASE)/%.h,$(OBJ_BASE)/%.h.i,$(HDRS))

CFLAGS += -I$(SRC_BASE)

all: $(OUTPUT_PROJECT_PATH)
prep: $(PREPS)

include $(PWD)/Make/static_libraries.mk

$(OUTPUT_PROJECT_PATH): $(OBJS) $(STATIC_LIBS)
	$(LD_ECHO)
	$(CC) $(CFLAGS) -l:libncurses.so $^ -o $@

$(OBJ_BASE)/%.o: $(SRC_BASE)/%.c
	$(CC_ECHO)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OBJ_BASE)/%.i: $(SRC_BASE)/%
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -E -P -x c -Wno-pragma-once-outside-header $< -o $@

clean:
	rm -rf $(BUILD_PATH)

purge: clean
	make readline_clean

.PHONY: all clean prep