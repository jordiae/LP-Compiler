antlr -gt numlistlang2_interpret2.g
dlg -ci parser.dlg scan.c
g++ -o numlistlang2_interpret2 numlistlang2_interpret2.c scan.c err.c -Wno-write-strings -std=c++11
