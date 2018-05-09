antlr -gt numlistlang2_interpret.g
dlg -ci parser.dlg scan.c
g++ -o numlistlang2_interpret numlistlang2_interpret.c scan.c err.c -Wno-write-strings
