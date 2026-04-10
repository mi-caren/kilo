# Aeolus — builds libaeolus.a (full) and libaeolus_tiny.a separately.
#
# Full and tiny variants share result.c and shared/*.c but have
# different vec and string implementations. Building them as separate
# libraries avoids duplicate symbols (e.g. str_new defined in both
# full/string.c and tiny/string.c) and prevents full Vecchar_*
# functions from leaking into the tiny binary.
#
# Called from the root Makefile:
#   make -C src/aeolus BUILD_DIR=../../build CPPFLAGS=... CFLAGS=... full
#   make -C src/aeolus BUILD_DIR=../../build CPPFLAGS=... CFLAGS=... tiny

BUILD_DIR ?= ../../build

# Shared sources (variant-independent)
ROOT_SRCS   = $(wildcard *.c)
SHARED_SRCS = $(wildcard shared/*.c)
SHARED_OBJS = $(patsubst %.c, $(BUILD_DIR)/aeolus_%.o, $(ROOT_SRCS)) \
              $(patsubst shared/%.c, $(BUILD_DIR)/aeolus_shared_%.o, $(SHARED_SRCS))

# Full variant
FULL_SRCS = $(wildcard full/*.c)
FULL_OBJS = $(patsubst full/%.c, $(BUILD_DIR)/aeolus_full_%.o, $(FULL_SRCS))

# Tiny variant
TINY_SRCS = $(wildcard tiny/*.c)
TINY_OBJS = $(patsubst tiny/%.c, $(BUILD_DIR)/aeolus_tiny_%.o, $(TINY_SRCS))

.PHONY: full tiny

full: $(BUILD_DIR)/libaeolus.a
tiny: $(BUILD_DIR)/libaeolus_tiny.a

$(BUILD_DIR)/libaeolus.a: $(FULL_OBJS) $(SHARED_OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/libaeolus_tiny.a: $(TINY_OBJS) $(SHARED_OBJS)
	$(AR) rcs $@ $^

# Shared (root .c files like result.c)
$(BUILD_DIR)/aeolus_%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/aeolus_shared_%.o: shared/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Full variant
$(BUILD_DIR)/aeolus_full_%.o: full/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Tiny variant
$(BUILD_DIR)/aeolus_tiny_%.o: tiny/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
