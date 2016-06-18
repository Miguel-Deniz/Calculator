
#include "std_lib_facilities.h"
#include "Roman_INT.h"
#include "TokenStream.h"

Token_stream::Token_stream()
	:full(false), buffer(0)
{}

Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}

	char ch = 'x';
	ch = cin.get();

	while (isspace(ch)) {
		if (ch == '\n') return Token(PRINT); // if newline detected, return print Token
		cin.get(ch);
	}

	switch (ch)
	{
	case PRINT:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
	case ',':
	case '^':
	case '!':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		cin.putback(ch);
		double val;
		cin >> val;
		return Token(NUMBER, val);
	}
	case 'I':
	case 'V':
	case 'X':
	case 'L':
	case 'C':
	case 'D':
	case 'M':
	{
		cin.putback(ch);
		Roman_INT rint("");
		cin >> rint;
		return Token(NUMBER, rint.as_int());
	}

	default:
	{
		if (isalpha(ch) || ch == '_')
		{
			string s;
			s += ch;

			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;

			cin.putback(ch);

			if (s == ABS_KEY) return Token(ABSOLUTE);
			if (s == CLEAR_KEY) return Token(CLEAR);
			if (s == CONST_KEY) return Token(CONST);
			if (s == DECL_KEY) return Token(LET);
			if (s == HELP_KEY) return Token(HELP);
			if (s == EXIT_KEY) return Token(EXIT);
			if (s == EXPONENTIAL_KEY) return Token(EXPONENTIAL);
			if (s == FRACTION_KEY) return Token(FRACTION);
			if (s == SQRT_KEY) return Token(SQRT);
			if (s == POW_KEY) return Token(POW);
			if (s == VARS_KEY) return Token(VARIABLES);

			return Token(NAME, s);
		}

		error("Bad token", ch);
	}
	}
}

void Token_stream::putback(Token t)
{
	if (full) error("putback() into a full buffer");
	buffer = t;
	full = true;
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind)
	{
		full = false;
		return;
	}

	full = false;

	char ch = 0;
	while (cin >> ch)
	{
		if (ch == c) return;
	}
}

void Token_stream::clear()
{
	full = false;
	cin.clear();
	fflush(stdin);
}

double Symbol_Table::get(string var)
{
	for (int i = 0; i < var_table.size(); ++i)
	{
		if (var_table[i].name == var) return var_table[i].value;
	}

	ts.clear();
	ts.putback(Token(PRINT));
	error("variable not defined");
}

void Symbol_Table::set(string s, double d)
{
	for (int i = 0; i < var_table.size(); ++i)
	{
		if (var_table[i].name == s)
		{
			// Check if the variable is a constant
			if (var_table[i].is_const == true) error(s, " is a constant");

			var_table[i].value = d;
			return;
		}
	}

	// Instead of an error we declare the variable
	//error("Set: Undefined variable: ", s);
	declare(s, d, false);
}

bool Symbol_Table::is_declared(string var)
{
	for (int i = 0; i < var_table.size(); ++i)
	{
		if (var_table[i].name == var) return true;
	}

	return false;
}

double Symbol_Table::declare(string var, double val, bool is_constant)
{
	if (is_declared(var)) error(var, " declared twice");

	var_table.push_back(Variable(var, val, is_constant));
	return val;
}

void Symbol_Table::display_vars()
{
	for (int i = 0; i < var_table.size(); i++)
	{
		cout << "\t";
		if (var_table[i].is_const)
			cout << "*";
		else
			cout << " ";
		cout << var_table[i].name << "\t=\t" << var_table[i].value << endl;
	}
}
