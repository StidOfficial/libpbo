CC=g++
LDFLAGS=-shared -lcrypto -lstdc++fs
CPPFLAGS=-Wall -Wextra -Werror -g -fPIC
RM=rm
MKDIR=mkdir
INCLUDEDIR=$(CURDIR)/include
OBJDIR=$(CURDIR)/obj
SRCDIR=$(CURDIR)/src
SRCFILES=$(shell find $(SRCDIR) -name "*.cpp")
OBJFILES=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))
VERSION=0.0.1

all: bin bin/libpbo.so

bin/libpbo.so: $(OBJFILES)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) -p $(shell dirname $@)
	$(CC) $(CPPFLAGS) -I$(INCLUDEDIR)/libpbo -c $< -o $@

install: bin/libpbo.so
	@cp bin/libpbo.so /usr/lib/libpbo.so.$(VERSION)
	@ln -sf /usr/lib/libpbo.so.$(VERSION) /usr/lib/libpbo.so
	@ln -sf /usr/lib/libpbo.so.$(VERSION) /usr/lib/libpbo.so.$(firstword $(subst ., ,$(VERSION)))
	@ldconfig -n /usr/lib/

test: bin bin/pboinfo bin/pbopack

bin:
	@$(MKDIR) -p ${CURDIR}/bin

bin/pboinfo: bin/libpbo.so
	$(CC) -I$(INCLUDEDIR) -Lbin/ -lpbo -lstdc++fs test/pboinfo/src/pboinfo.cpp -o bin/pboinfo

bin/pbopack: bin/libpbo.so
	$(CC) -I$(INCLUDEDIR) -Lbin/ -lpbo -lstdc++fs test/pbopack/src/pbopack.cpp -o bin/pbopack

clean:
	$(RM) -rf obj/* bin/* libpbo.so pboinfo pbopack

mrproper: clean
	$(RM) libpbo.so
