# Configuration for a native build on a generic Unix-like system.

# Build directory.
BUILD = build

# Extension for executable files.
E =

# Extension for object files.
O = .o

# Prefix for library file name.
LP = lib

# Extension for library file name.
L = .a

# Prefix for DLL file name.
DP = lib

# Extension for DLL file name.
D = .so

# Output file names can be overridden directly. By default, they are
# assembled using the prefix/extension macros defined above.
# CTTKLIB = libcttk.a
# CTTKDLL = libcttk.so
# TESTCTTK = testcttk

# File deletion tool.
RM = rm -f

# Directory creation tool.
MKDIR = mkdir -p

# C compiler and flags.
CC = cc
CFLAGS = -W -Wall -O2 -fPIC
CCOUT = -c -o 

# Static library building tool.
AR = ar
ARFLAGS = -rcs
AROUT =

# DLL building tool.
LDDLL = cc
LDDLLFLAGS = -shared
LDDLLOUT = -o 

# Static linker.
LD = cc
LDFLAGS = 
LDOUT = -o 

# Set the values to 'no' to disable building of the corresponding element
# by default. Building can still be invoked with an explicit target call
# (e.g. 'make dll' to force build the DLL).
#STATICLIB = no
#DLL = no
#TESTS = no
