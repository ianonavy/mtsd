VERSION = 0.1

# Compiler flags
CC ?= gcc
LIBS = -lgeis
CFLAGS = -std=c99 -pedantic -Wall -Werror -Wextra -I$(PREFIX)/include
CFLAGS += -D_POSIX_C_SOURCE=200112L -DVERSION=\"$(VERSION)\"
LDFLAGS = -L$(PREFIX)/lib

BUILDDIR = build

# Install directories
PREFIX ?= /usr/local
BINPREFIX = $(PREFIX)/bin
MANPREFIX = $(PREFIX)/share/man
BASHCPL = $(PREFIX)/share/bash-completion/completions
ZSHCPL = $(PREFIX)/share/zsh/site-functions

# Source files
SRC = mtsd.c helpers.c geis_helpers.c
OBJ = $(addprefix $(BUILDDIR)/,$(SRC:.c=.o))

# Dependencies and targets
all: CFLAGS += -Os
all: LDFLAGS += -s
all: $(BUILDDIR)/mtsd

debug: CFLAGS += -O0 -g -DDEBUG
debug: $(BUILDDIR)/mtsd

include Sourcedeps

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(OBJ): Makefile config.h helpers.h | $(BUILDDIR)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -c -o $@ $<

$(BUILDDIR)/mtsd: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

install:
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p mtsd "$(DESTDIR)$(BINPREFIX)"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)"/mtsd

clean:
	rm -rf $(BUILDDIR) $(OBJ) mtsd

.PHONY: all debug install uninstall clean
