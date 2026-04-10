include config.mk

CPPFLAGS = -MMD -Isrc
# -ffunction-sections/-fdata-sections: put each function/variable in its own
# ELF section so the linker can discard unused ones individually.
# -Wl,--gc-sections: tell the linker to garbage-collect unreferenced sections.
# This is important for the tiny (driverless) aeolus variant: since all calls
# are direct (no vtable), the compiler inlines aggressively at -O2/-O3, but
# the original function bodies are kept unless gc-sections strips them.
# Without these flags tiny is ~7% larger than full; with them it's ~12% smaller.
CFLAGS = -Wall -Wextra -pedantic --std=c23 -ffunction-sections -fdata-sections
LDFLAGS = -L$(LIB_DIR) $(AEOLUS_LFLAG_$(AEOLUS_VARIANT)) -Wl,--gc-sections

SRCS = $(wildcard src/*.c src/*/*.c)
ROOT_SRCS = $(wildcard src/*.c)
EDITOR_SRCS = $(wildcard src/editor/*.c)

ROOT_OBJS = $(patsubst src/%.c, build/%.o, $(ROOT_SRCS))
EDITOR_OBJS = $(patsubst src/editor/%.c, build/editor_%.o, $(EDITOR_SRCS))
OBJS = $(ROOT_OBJS) $(EDITOR_OBJS)

# lib aeolus — built via sub-Makefile in src/aeolus/
# AEOLUS_VARIANT: "full" (default) or "tiny"
AEOLUS_VARIANT ?= full
AEOLUS_LIB_full  = build/libaeolus.a
AEOLUS_LIB_tiny  = build/libaeolus_tiny.a
AEOLUS_LFLAG_full = -laeolus
AEOLUS_LFLAG_tiny = -laeolus_tiny
LIBS = $(AEOLUS_LIB_$(AEOLUS_VARIANT))

DEPS = $(OBJS:.o=.d)

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
.PHONY: aeolus
aeolus: build
	$(MAKE) -C src/aeolus BUILD_DIR=../../build CC="$(CC)" AR="$(AR)" \
		CPPFLAGS="-I../" CFLAGS="$(CFLAGS)" $(AEOLUS_VARIANT)

$(LIBS): aeolus
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
