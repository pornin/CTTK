# Automatically generated rules. Use 'mkrules.sh' to modify/regenerate.

OBJ = \
 $(OBJDIR)$Pbase64$O \
 $(OBJDIR)$Phex$O \
 $(OBJDIR)$Pint31$O \
 $(OBJDIR)$Pmul$O \
 $(OBJDIR)$Poram1$O
OBJTESTCTTK = \
 $(OBJDIR)$Ptestcttk$O
HEADERSPUB = inc$Pcttk.h
HEADERSPRIV = $(HEADERSPUB) src$Pconfig.h src$Pinner.h

all: $(STATICLIB) $(DLL) $(TESTS)

no:

lib: $(CTTKLIB)

dll: $(CTTKDLL)

tests: $(TESTCTTK)

clean:
	-$(RM) $(OBJDIR)$P*$O
	-$(RM) $(CTTKLIB) $(CTTKDLL) $(TESTCTTK)

$(OBJDIR):
	-$(MKDIR) $(OBJDIR)

$(CTTKLIB): $(OBJDIR) $(OBJ)
	$(AR) $(ARFLAGS) $(AROUT)$(CTTKLIB) $(OBJ)

$(CTTKDLL): $(OBJDIR) $(OBJ)
	$(LDDLL) $(LDDLLFLAGS) $(LDDLLOUT)$(CTTKDLL) $(OBJ)

$(TESTCTTK): $(CTTKLIB) $(OBJTESTCTTK)
	$(LD) $(LDFLAGS) $(LDOUT)$(TESTCTTK) $(OBJTESTCTTK) $(CTTKLIB)

$(OBJDIR)$Pbase64$O: src$Pbase64.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Pbase64$O src$Pbase64.c

$(OBJDIR)$Phex$O: src$Phex.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Phex$O src$Phex.c

$(OBJDIR)$Pint31$O: src$Pint31.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Pint31$O src$Pint31.c

$(OBJDIR)$Pmul$O: src$Pmul.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Pmul$O src$Pmul.c

$(OBJDIR)$Poram1$O: src$Poram1.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Poram1$O src$Poram1.c

$(OBJDIR)$Ptestcttk$O: test$Ptestcttk.c $(HEADERSPRIV)
	$(CC) $(CFLAGS) $(INCFLAGS) $(CCOUT)$(OBJDIR)$Ptestcttk$O test$Ptestcttk.c
