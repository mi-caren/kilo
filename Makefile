include config.mk

CPPFLAGS = -MMD -Isrc
CFLAGS = -Wall -Wextra -pedantic --std=c23 -ffunction-sections -fdata-sections
LDFLAGS = -L$(LIB_DIR) -laeolus -Wl,--gc-sections 

SRCS = $(wildcard src/*.c src/*/*.c)
ROOT_SRCS = $(wildcard src/*.c)
EDITOR_SRCS = $(wildcard src/editor/*.c)

ROOT_OBJS = $(patsubst src/%.c, build/%.o, $(ROOT_SRCS))
EDITOR_OBJS = $(patsubst src/editor/%.c, build/editor_%.o, $(EDITOR_SRCS))
OBJS = $(ROOT_OBJS) $(EDITOR_OBJS)

# lib aeolus
AEOLUS_ROOT_SRCS = $(wildcard src/aeolus/*.c)
AEOLUS_VEC_SRCS = $(wildcard src/aeolus/vec/*.c)
AEOLUS_STRING_SRCS = $(wildcard src/aeolus/string/*.c)
AEOLUS_TINY_SRCS = $(wildcard src/aeolus/tiny/*.c)
AEOLUS_ROOT_OBJS = $(patsubst src/aeolus/%.c, build/aeolus_%.o, $(AEOLUS_ROOT_SRCS))
AEOLUS_VEC_OBJS = $(patsubst src/aeolus/vec/%.c, build/aeolus_vec_%.o, $(AEOLUS_VEC_SRCS))
AEOLUS_STRING_OBJS = $(patsubst src/aeolus/string/%.c, build/aeolus_string_%.o, $(AEOLUS_STRING_SRCS))
AEOLUS_TINY_OBJS = $(patsubst src/aeolus/tiny/%.c, build/aeolus_tiny_%.o, $(AEOLUS_TINY_SRCS))
AEOLUS_OBJS = $(AEOLUS_ROOT_OBJS) $(AEOLUS_VEC_OBJS) $(AEOLUS_STRING_OBJS) $(AEOLUS_TINY_OBJS)
LIBS = build/libaeolus.a

DEPS = $(OBJS:.o=.d) $(AEOLUS_OBJS:.o=.d)

.PHONY: release
release: CPPFLAGS += -DNDEBUG
release: CFLAGS += -O3
release: loki

.PHONY: debug
debug: CFLAGS += -g
debug: loki


loki: build $(OBJS) $(LIBS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

build:
	mkdir -p build

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/editor_%.o: src/editor/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


# ----- libaeolus -----
build/aeolus_%.o: src/aeolus/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/aeolus_vec_%.o: src/aeolus/vec/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/aeolus_string_%.o: src/aeolus/string/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/aeolus_tiny_%.o: src/aeolus/tiny/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/libaeolus.a: $(AEOLUS_OBJS)
	$(AR) rcs $@ $^
# ---------------------


.PHONY: install
install: loki
	mkdir -p $(INSTALL_DIR)
	cp -f loki $(INSTALL_DIR)

.PHONY: uninstall
uninstall:
	rm -f $(INSTALL_DIR)/loki

# ----- Just preprocessing -----
build/aeolus_pre_%.c: src/aeolus/%.c
	$(CC) $(CFLAGS) -g -E $< -o $@

build/pre_%.c: src/%.c
	$(CC) $(CFLAGS) -g -E $< -o $@
# ------------------------------


.PHONY: test
test: tests/*.c
	zig run -I inc --library c $< $(filter-out src/loki.c, $(SRCS))


.PHONY: clean
clean:
	rm -f $(EXE)
	rm -f build/*

-include $(DEPS)
