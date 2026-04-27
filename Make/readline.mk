LIB_TARGET := libreadline.a
READLINE_DIR := $(SRC_BASE)/readline
STATIC_LIBS += $(READLINE_DIR)/$(LIB_TARGET)
CFLAGS += -I$(READLINE_DIR)

.PHONY: all readline_clean readline_purge

$(READLINE_DIR)/Makefile:
	cd $(READLINE_DIR) && CC=$(CC) ./configure

$(READLINE_DIR)/$(LIB_TARGET): $(READLINE_DIR)/Makefile
	make -C $(READLINE_DIR) -j8

readline_clean: $(READLINE_DIR)/Makefile
	make -C $(READLINE_DIR) clean

readline_purge: clean
	rm -rf $(READLINE_DIR)/Makefile