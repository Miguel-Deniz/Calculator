#ifndef TOKEN_STREAM_H
#define TOKE_STREAM_H

const char ABSOLUTE = 'A';
const char CLEAR = 'c';
const char CONST = 'C';
const char EXIT = 'e';
const char EXPONENTIAL = 'E';
const char HELP = 'h';
const char LET = 'L';
const char FACTORIAL = '!';
const char FRACTION = 'f';
const char NAME = 'a';
const char NEWLINE = 'n';
const char NUMBER = '8';
const char POW = 'p';
const char POWER = '^';
const char PRINT = ';';
const char PROMPT = '>';
const char RESULT = '=';
const char ROMAN_NUMERAL = 'r';
const char SQRT = 's';
const char VARIABLES = 'v';

const string ABS_KEY = "abs";
const string CLEAR_KEY = "clc";
const string CONST_KEY = "const";
const string DECL_KEY = "let";
const string EXPONENTIAL_KEY = "exp";
const string EXIT_KEY = "exit";
const string HELP_KEY = "help";
const string FRACTION_KEY = "frac";
const string SQRT_KEY = "sqrt";
const string POW_KEY = "pow";
const string VARS_KEY = "vars";

const double OPERATION_FAILED = -1.52945;

class Token
{
public:
	char kind;
	double value;
	string name;

	Token(char ch)
		:kind(ch), value(0) {}

	Token(char ch, double val)
		:kind(ch), value(val) {}

	Token(char ch, string n)
		:kind(ch), name(n) {}
};

class Token_stream
{
public:
	Token_stream();
	Token get();
	void putback(Token t);
	void ignore(char c);
	void clear();
private:
	bool full;
	Token buffer;
};

class Variable
{
public:
	string name;
	double value;
	bool is_const;

	Variable(string n, double v, bool is_constant)
		:name(n), value(v), is_const(is_constant)
	{}
};

static Token_stream ts;

class Symbol_Table
{
public:
	double get(string var);

	void set(string s, double d);

	bool is_declared(string var);

	double declare(string var, double val, bool is_constant);

	void display_vars();
	
private:
	vector<Variable> var_table;
};

static Symbol_Table st;

#endif TOKEN_STREAM_H