CC      = gcc
CFLAGS  = -g
RM      = rm -f

default: all

all: customShell

customShell: customShell.c
	$(CC) $(CFLAGS) -o customShell customShell.c -lreadline -lm

clean veryclean:
	$(RM) cutsomShell
