#! /bin/sh

# ========================================================================
#
# Copyright (c) 2018 Thomas Pornin <pornin@bolet.org>
#
# Permission is hereby granted, free of charge, to any person obtaining 
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be 
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ========================================================================
#
# This script is used to generate the 'Rules.mk' file from the list
# of source file included below. If the list changes (e.g. to add a
# new source file), then add it here and rerun this script.
#
# ========================================================================

# Solaris compatibility: switch to a more POSIX-compliant /bin/sh.
if [ -z "$CT_SCRIPT_LOOP" ] ; then
	CT_SCRIPT_LOOP=yes
	export CT_SCRIPT_LOOP
	if [ -x /usr/xpg6/bin/sh ] ; then
		exec /usr/xpg6/bin/sh "$0" "$@"
	fi
	if [ -x /usr/xpg4/bin/sh ] ; then
		exec /usr/xpg4/bin/sh "$0" "$@"
	fi
fi

# Exit on first error.
set -e

# Source files. Please keep in alphabetical order.
coresrc=" \
	src/base64.c \
	src/copy.c \
	src/hex.c \
	src/int31.c \
	src/mul.c \
	src/swap.c"

# Source files the the 'testcttk' command-line tool.
testcttksrc=" \
	test/testcttk.c"

# Public header files.
headerspub=" \
	inc/cttk.h"

# Private header files.
headerspriv=" \
	src/config.h \
	src/inner.h"

# Function to turn slashes into $P (macro for path separator).
escsep() {
	printf '%s' "$1" | sed 's/\//$P/g'
}

# Create rules file.
rm -f Rules.mk
cat > Rules.mk <<EOF
# Automatically generated rules. Use 'mkrules.sh' to modify/regenerate.
EOF

(printf "\nOBJ ="
for f in $coresrc ; do
	printf ' \\\n $(OBJDIR)$P%s' "$(basename "$f" .c)\$O"
done
printf "\nOBJTESTCTTK ="
for f in $testcttksrc ; do
	printf ' \\\n $(OBJDIR)$P%s' "$(basename "$f" .c)\$O"
done
printf "\nHEADERSPUB ="
for f in $headerspub ; do
	printf " %s" "$(escsep "$f")"
done
printf "\nHEADERSPRIV = %s" '$(HEADERSPUB)'
for f in $headerspriv ; do
	printf " %s" "$(escsep "$f")"
done
printf "\n") >> Rules.mk

cat >> Rules.mk <<EOF

all: \$(STATICLIB) \$(DLL) \$(TESTS)

no:

lib: \$(CTTKLIB)

dll: \$(CTTKDLL)

tests: \$(TESTCTTK)

clean:
	-\$(RM) \$(OBJDIR)\$P*\$O
	-\$(RM) \$(CTTKLIB) \$(CTTKDLL) \$(TESTCTTK)

\$(OBJDIR):
	-\$(MKDIR) \$(OBJDIR)

\$(CTTKLIB): \$(OBJDIR) \$(OBJ)
	\$(AR) \$(ARFLAGS) \$(AROUT)\$(CTTKLIB) \$(OBJ)

\$(CTTKDLL): \$(OBJDIR) \$(OBJ)
	\$(LDDLL) \$(LDDLLFLAGS) \$(LDDLLOUT)\$(CTTKDLL) \$(OBJ)

\$(TESTCTTK): \$(CTTKLIB) \$(OBJTESTCTTK)
	\$(LD) \$(LDFLAGS) \$(LDOUT)\$(TESTCTTK) \$(OBJTESTCTTK) \$(CTTKLIB)
EOF

(for f in $coresrc ; do
	b="$(basename "$f" .c)\$O"
	g="$(escsep "$f")"
	printf '\n$(OBJDIR)$P%s: %s $(HEADERSPRIV)\n\t$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$P%s %s\n' "$b" "$g" "$b" "$g"
done

for f in $testcttksrc ; do
	b="$(basename "$f" .c)\$O"
	g="$(escsep "$f")"
	printf '\n$(OBJDIR)$P%s: %s $(HEADERSPRIV)\n\t$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$P%s %s\n' "$b" "$g" "$b" "$g"
done) >> Rules.mk
