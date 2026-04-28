LIBFFI_TARGET = $(LIBFFI_BUILD_DIR)/lib64/libffi.a
LIBFFI_DIR := $(SRC_BASE)/libffi
LIBFFI_BUILD_DIR := $(LIBFFI_DIR)/build

STATIC_LIBS += $(LIBFFI_TARGET)
CFLAGS += -I$(LIBFFI_BUILD_DIR)/include

.PHONY: libffi_clean libffi_purge

$(LIBFFI_DIR)/configure:
	cd $(LIBFFI_DIR) && ./autogen.sh

$(LIBFFI_DIR)/Makefile: $(LIBFFI_DIR)/configure
	cd $(LIBFFI_DIR) && CC=$(CC) ./configure --prefix=$(LIBFFI_BUILD_DIR)

$(LIBFFI_TARGET): $(LIBFFI_DIR)/Makefile
	mkdir -p $(LIBFFI_BUILD_DIR)
	make -C $(LIBFFI_DIR) -j8 && make -C $(LIBFFI_DIR) install

libffi_clean: $(LIBFFI_DIR)/Makefile
	make -C $(LIBFFI_DIR) clean
	rm -rf $(LIBFFI_BUILD_DIR)

libffi_purge: clean
	rm -rf $(LIBFFI_DIR)/Makefile
	rm -rf $(LIBFFI_DIR)/configure