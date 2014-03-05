# authsqlite
# adjust settings here to fit your system
CC = gcc
RM = rm -f

# you may override these values on the command line, e.g. make INCS='-I/usr/local/sqlite/include' LIBS='-L/usr/local/sqlite/lib'
# if not overridden, use defaults for most systems
LIBS = -L/usr/lib -L/usr/local/lib
INCS = -I/usr/include -I/usr/local/include

# there should be no need to adjust lines beyond this one
CFLAGS = $(INCS) -O3
LIBS_EXTRA = --no-export-all-symbols --add-stdcall-alias -lsqlite3

OBJ      = authsqlite.o ovpnauth-db.o ovpnauth.o sha256.o
LINKOBJ  = authsqlite.o sha256.o
BIN      = authsqlite.so

LINKPRG  = ovpnauth-db.o ovpnauth.o sha256.o
BINPRG   = ovpnauth

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) $(BINPRG) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN) $(BINPRG)

$(BIN): $(LINKOBJ)
	$(CC) -fPIC -shared -Wl,-soname,$(BIN) -o $(BIN) $(LINKOBJ) -lc $(LIBS) $(LIBS_EXTRA)

$(BINPRG): $(LINKPRG)
	$(CC) -o $(BINPRG) $(LINKPRG) -lc $(LIBS) $(LIBS_EXTRA)

authsqlite.o: authsqlite.c openvpn-plugin.h
	$(CC) -fPIC -c authsqlite.c -o authsqlite.o $(CFLAGS)

ovpnauth.o: ovpnauth.c ovpnauth.h
	$(CC) -c ovpnauth.c -o ovpnauth.o $(CFLAGS)

ovpnauth-db.o: ovpnauth-db.c
	$(CC) -c ovpnauth-db.c -o ovpnauth-db.o $(CFLAGS)

sha256.o: sha256.c sha256.h
	$(CC) -c sha256.c -o sha256.o $(CFLAGS)

