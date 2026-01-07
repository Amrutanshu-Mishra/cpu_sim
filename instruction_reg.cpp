#include <bits/stdc++.h>
using namespace std;

map<string, string>instruction_register;
instruction_register["NOP"]="0x0";
instruction_register["LOAD"]="0x1";
instruction_register["STORE"]="0x2";
instruction_register["MOVI"]="0x3";
instruction_register["ADD"]="0x4";
instruction_register["SUB"]="0x5";
instruction_register["MUL"]="0x6";
instruction_register["DIV"]="0x7";
instruction_register["POW"]="0x8";
instruction_register["CMP"]="0x9";
instruction_register["JMP"]="0xA";
instruction_register["JZ"]="0xB";
instruction_register["JC"]="0xC";
instruction_register["HALT"]="0xF";


