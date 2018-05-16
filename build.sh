antlr -gt numlistlang2_def2.g
dlg -ci parser.dlg scan.c
g++ -o numlistlang2_def2 numlistlang2_def2.c scan.c err.c -Wno-write-strings -std=c++11
