#ifndef CALCULATIONS_H
#define CALCULATIONS_H

double statement(Token_stream& ts, Symbol_Table& st);

double expression(Token_stream& ts, Symbol_Table& st);

double declaration(Token_stream& ts, Symbol_Table& st, bool is_constant);

double term(Token_stream& ts, Symbol_Table& st);

double primary(Token_stream& ts, Symbol_Table& st);

int min(int x, int y);

int gdc(int x, int y);

void convert_to_fraction(double ans);

void calculate(Token_stream& ts, Symbol_Table& st);

void clean_up_mess(Token_stream& ts);

#endif CALCULATIONS_H