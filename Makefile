CC:=g++
LDFLAGS:=-shared
CPPFLAGS:=-Wall -Wextra -Werror -g -fPIC
RM:=rm
MKDIR:=mkdir
CURDIR:=${CURDIR}
INCLUDEDIR=$(CURDIR)/include
OBJDIR:=$(CURDIR)/obj
SRCDIR:=$(CURDIR)/src
SRCFILES:=$(shell find $(SRCDIR) -name "*.cpp")
OBJFILES:=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))

all: bin/libpbo.so

bin/libpbo.so: $(OBJFILES)
	@$(MKDIR) -p $(CURDIR)/bin
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) -p $(shell dirname $@)
	$(CC) $(CPPFLAGS) -I$(INCLUDEDIR)/libpbo -c $< -o $@

install: bin/libpbo.so
	@cp bin/libpbo.so /usr/lib/libpbo.so.0.0.1
	@ln -sf /usr/lib/libpbo.so.0.0.1 /usr/lib/libpbo.so
	@ln -sf /usr/lib/libpbo.so.0.0.1 /usr/lib/libpbo.so.1
	@ldconfig -n /usr/lib/

test: install bin/pboinfo bin/pbopack

bin/pboinfo:
	@$(MKDIR) -p $(CURDIR)/bin
	$(CC) -I$(INCLUDEDIR) -lpbo -lstdc++fs test/pboinfo/src/PBOInfo.cpp -o bin/pboinfo

bin/pbopack:
	@$(MKDIR) -p $(CURDIR)/bin
	$(CC) -I$(INCLUDEDIR) -lpbo -lstdc++fs test/pbopack/src/PBOPack.cpp -o bin/pbopack

clean:
	$(RM) -rf obj/* bin/* libpbo.so pboinfo pbopack

mrproper: clean
	$(RM) libpbo.so
