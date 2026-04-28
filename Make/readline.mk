READLINE_DIR := $(SRC_BASE)/readline
READLINE_TARGET = $(READLINE_DIR)/libreadline.a
STATIC_LIBS += $(READLINE_TARGET)
CFLAGS += -I$(READLINE_DIR)

.PHONY: readline_clean readline_purge

$(READLINE_DIR)/Makefile:
	cd $(READLINE_DIR) && CC=$(CC) ./configure

$(READLINE_DIR)/$(READLINE_TARGET): $(READLINE_DIR)/Makefile
	make -C $(READLINE_DIR) -j8

readline_clean: $(READLINE_DIR)/Makefile
	make -C $(READLINE_DIR) clean

readline_purge: clean
	rm -rf $(READLINE_DIR)/Makefile