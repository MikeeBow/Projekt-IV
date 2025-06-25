# Makefile - kompilacja jdprint.cpp z GDI+ (MSYS2 UCRT64)
CXX = g++
CXXFLAGS = -g -Wall -fdiagnostics-color=always -municode -DUNICODE -D_UNICODE
LDFLAGS = -lgdi32 -luser32 -lole32 -luuid -lgdiplus

SRC = jdprint.cpp
OUT = jdprint.exe

all:
    $(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
    rm -f $(OUT)