/*
 * File: generateLexer.h
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the generateLexer function
 */
#ifndef GENERATELEXER_H
#define GENERATELEXER_H
#include "lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>
std::string charToOut(char c);
std::string generateLexer(const Lexer &lexer);
void generateLexerToFile(const Lexer &lexer, const std::string &filename);
#endif