mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

SRCDIR := $(current_dir)/src
OBJDIR := $(current_dir)/out
TESTSRCDIR := $(current_dir)/test
TESTOBJDIR := $(current_dir)/testout

SRC_CFLAGS  := -Wall -Wextra -Winline -pedantic -g -O0 -std=gnu23 -march=native -fno-omit-frame-pointer -MMD -MP
SRC_LDFLAGS :=
SRC_LDLIBS  := -lmagic
SRC_DEFINES := -D_GNU_SOURCE -D__USE_FILE_OFFSET64

CFLAGS   ?= $(SRC_CFLAGS)
LDFLAGS  ?= $(SRC_LDFLAGS)
LDLIBS   ?=
DEFINES  ?= $(SRC_DEFINES)


# ---- sources/objects (include top-level and one-level subdirs) --------------
SRC  := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c)
OBJ  := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
DEPS := $(OBJ:.o=.d)
TESTSRC  := $(wildcard $(TESTSRCDIR)/*.c) $(wildcard $(TESTSRCDIR)/*/*.c)
TESTOBJ  := $(patsubst $(TESTSRCDIR)/%.c,$(TESTOBJDIR)/%.o,$(TESTSRC))
TESTDEPS := $(TESTOBJ:.o=.d)

# Final binary
BIN := main
TESTBIN := a.out

.PHONY: all tests clean temps strip echopath
all: $(BIN)

# ---- link --------------------------------------------------------------------
$(BIN): $(OBJ)
	$(CC) $(SRC_LDFLAGS) -o $@ $^ $(SRC_LDLIBS)

# ---- compile (ensure OBJDIR exists first) -----------------------------------
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(INC) $(SRC_CFLAGS) $(SRC_DEFINES) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $@

tests: $(TESTBIN)

# ---- link --------------------------------------------------------------------
$(TESTBIN): $(TESTOBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# ---- compile (ensure OBJDIR exists first) -----------------------------------
$(TESTOBJDIR)/%.o: $(TESTSRCDIR)/%.c | $(TESTOBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(INC) $(CFLAGS) $(DEFINES) -c -o $@ $<

$(TESTOBJDIR):
	@mkdir -p $@

strip: $(BIN)
	@if [ -n "$(STRIP)" ]; then $(STRIP) -s $(BIN); else strip -s $(BIN); fi

clean:
	rm -rf $(BIN) $(OBJDIR)
	rm -rf $(TESTBIN) $(TESTOBJDIR)

temps:
	@$(MAKE) clean
	@$(MAKE) SRC_CFLAGS="$(SRC_CFLAGS) -save-temps=obj"

echopath:
	echo $(current_dir)

# include auto-generated header dependency files
-include $(DEPS)