antlr -gt numlistlang2_def.g
dlg -ci parser.dlg scan.c
g++ -o numlistlang2_def numlistlang2_def.c scan.c err.c -Wno-write-strings -std=c++11
