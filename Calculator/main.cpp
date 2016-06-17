
/*
Simple Calculator

Revision History:
Miguel Deniz 6/14/16 2:11PM
Miguel Deniz 6/8/16 8:10pm
Miguel Deniz 6/8/16 2:06PM

This program implements a basic expression calculator.
Input from cin; output to cout.

The grammar for input is:

Calculation:
Statement
Clear
Print
Quit
Help
Calculation Statement

Statement:
Declaration
Expression

Declaration:
"let" "name" "=" Expression
"const" "name" "=" Expression

Variables:
vars

Clear:
clc

Print:
;

Quit:
q

Expression:
Term
Expression + Term
Expression - Term

Term:
Primary
Term * Primary
Term / Primary
Term % Primary

Primary:
Number
(Expression)
- Primary
+ Primary
sqrt(expression)
pow(expression, integer)
Name
Name "=" Expression

Number:
Floating-point-literal

Input comes from cin though the Token_stream called ts.
*/


#include "std_lib_facilities.h"

namespace Calculator
{
	// Constants
	const char CLEAR = 'c';
	const char CONST = 'C';
	const char EXIT = 'e';
	const char HELP1 = 'h';
	const char HELP2 = 'H';
	const char LET = 'L';
	const char FRACTION = 'f';
	const char NAME = 'a';
	const char NEWLINE = 'n';
	const char NUMBER = '8';
	const char POW = 'p';
	const char PRINT = ';';
	const char PROMPT = '>';
	const char RESULT = '=';
	const char ROMAN_NUMERAL = 'r';
	const char SQRT = 's';
	const char VARIABLES = 'v';

	const string CLEAR_KEY = "clc";
	const string CONST_KEY = "const";
	const string DECL_KEY = "let";
	const string EXIT_KEY = "exit";
	const string FRACTION_KEY = "frac";
	const string SQRT_KEY = "sqrt";
	const string POW_KEY = "pow";
	const string VARS_KEY = "vars";

	const double OPERATION_FAILED = -1.52945;

	//----- Prototypes -----

	class Token;
	class Token_stream;
	class Variable;

	double statement();
	double expression();
	double declaration(bool);
	double term();
	double primary();
	void clean_up_mess();

	//----------------------

	class Roman_INT
	{
	public:
		Roman_INT(string num);
		bool is_valid(string num) const;
		bool valid_roman_numeral(char ch) const;
		int as_int() const;
		int get_value(char ch) const;

		friend ostream& operator<<(ostream& os, const Roman_INT& rint);
		friend istream& operator>>(istream& is, Roman_INT& rint);
	private:
		const string VALID_ROMAL_NUMERALS = "IVXLCDM";
		string number;
	};

	class Token
	{
	public:
		char kind;		// What kind of token
		double value;	// For numbers; a value
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
		Token_stream()
			:full(false), buffer(0)
		{}

		Token get()
		{
			if (full)
			{
				full = false;
				return buffer;
			}

			char ch = 'x';
			//cin >> ch;	// this does not read whitespace
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

					if (s == CLEAR_KEY) return Token(CLEAR);
					if (s == CONST_KEY) return Token(CONST);
					if (s == DECL_KEY) return Token(LET);
					if (s == EXIT_KEY) return Token(EXIT);
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

		void putback(Token t)
		{
			if (full) error("putback() into a full buffer");
			buffer = t;
			full = true;
		}

		void ignore(char c)
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

		void clear()
		{
			full = false;
			cin.clear();
			fflush(stdin);
		}
	private:
		bool full;
		Token buffer;
	};

	//---------------------

	Token_stream ts;

	//---------------------

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

	class Symbol_Table
	{
	public:
		double get(string var)
		{
			for (int i = 0; i < var_table.size(); ++i)
			{
				if (var_table[i].name == var) return var_table[i].value;
			}

			ts.clear();
			ts.putback(Token(PRINT));
			error("variable not defined");
		}

		void set(string s, double d)
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

			error("Set: Undefined variable: ", s);
		}

		bool is_declared(string var)
		{
			for (int i = 0; i < var_table.size(); ++i)
			{
				if (var_table[i].name == var) return true;
			}

			return false;
		}

		double declare(string var, double val, bool is_constant)
		{
			if (is_declared(var)) error(var, " declared twice");

			var_table.push_back(Variable(var, val, is_constant));
			return val;
		}

		void display_vars()
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

	private:
		vector<Variable> var_table;
	};

	Symbol_Table st;

	ostream& operator<<(ostream& os, const Roman_INT& rint)
	{
		return os << rint.number << " = " << rint.as_int();
	}

	istream& operator>>(istream& is, Roman_INT& rint)
	{
		char ch;
		ch = is.get();
		while (rint.valid_roman_numeral(ch))
		{
			rint.number += ch;
			ch = is.get();
		}

		cin.unget();

		return is;
	}

	Roman_INT::Roman_INT(string num)
	{
		if (is_valid(num))
			number = num;
		else
			cout << "ERROR: invalid roman numeral\n";
	}

	bool Roman_INT::is_valid(string num) const
	{
		for (int i = 0; i < num.length(); ++i)
		{
			char ch = num.at(i);
			if (!valid_roman_numeral(ch))
				return false;
		}

		bool subtract_mode = false;
		int val = 0;
		int prev_val = 0;

		for (int i = num.length() - 1; i >= 0; --i)
		{
			val = get_value(num.at(i));

			// Check to see if we are in "subtraction mode"
			// If we are, and we see another "subtraction mode", 
			// then the number becomes invalid
			if (val < prev_val)
			{
				if (subtract_mode)
					return false;
				else
					subtract_mode = true;
			}
			else
				subtract_mode = false;

			prev_val = val;
		}

		return true;
	}

	bool Roman_INT::valid_roman_numeral(char ch) const
	{
		bool valid_char = false;

		for (int j = 0; j < VALID_ROMAL_NUMERALS.length(); ++j)
		{
			if (ch == VALID_ROMAL_NUMERALS.at(j))
				valid_char = true;
		}

		return valid_char;
	}

	int Roman_INT::get_value(char ch) const
	{
		// Valid Roman numerals:
		// I = 1
		// V = 5
		// X = 10
		// L = 50
		// C = 100
		// D = 500
		// M = 1000

		switch (ch)
		{
		case 'I':
			return 1;
		case 'V':
			return 5;
		case 'X':
			return 10;
		case 'L':
			return 50;
		case 'C':
			return 100;
		case 'D':
			return 500;
		case 'M':
			return 1000;
		default:
			return 0;
		}
	}

	int Roman_INT::as_int() const
	{
		int total = 0;
		int prev_val = 0;
		int val = 0;

		for (int i = number.length() - 1; i >= 0; --i)
		{
			val = get_value(number.at(i));

			if (val < prev_val)
				total -= val;
			else
				total += val;

			prev_val = val;
		}

		return total;
	}

	double statement()
	{
		try
		{
			Token t = ts.get();

			switch (t.kind)
			{
			case LET:
				return declaration(false);
			case CONST:
				return declaration(true);
			default:
				ts.putback(t);
				return expression();
			}
		}
		catch (runtime_error& e)
		{
			cerr << e.what() << endl;
			clean_up_mess();
			return OPERATION_FAILED;
		}
		catch (...)
		{
			cerr << "Unknown exception in statement()" << endl;
			clean_up_mess();
			return OPERATION_FAILED;
		}
	}

	double expression()
	{
		double left = term();
		Token t = ts.get();

		while (true)
		{
			switch (t.kind)
			{
			case '+':
				left += term();
				t = ts.get();
				break;
			case '-':
				left -= term();
				t = ts.get();
				break;
			default:
				ts.putback(t);
				return left;
			}
		}
	}

	double declaration(bool is_constant)
	{
		// Assume we have seen "let" or "const"
		// Handle: name = expression
		// Declare a variable called "name" with the initial value "expression"

		Token t = ts.get();

		if (t.kind != NAME)
		{
			//ts.putback(t);
			ts.putback(Token(PRINT));
			error("name expected in declaration");
		}
		string var_name = t.name;

		Token t2 = ts.get();
		if (t2.kind != '=')
		{
			ts.putback(t2);
			error("= missing in declaration of ", var_name);
		}

		double d = expression();
		st.declare(var_name, d, is_constant);
		return d;
	}

	double term()
	{
		double left = primary();
		Token t = ts.get();

		while (true)
		{
			switch (t.kind)
			{
			case '(':
			{
				// Ex. 2(x)=2x or 8(9)=72
				double d = expression();
				Token t2 = ts.get();
				if (t2.kind != ')') error("expected )");
				return left * d;
			}
			case '*':
				left *= primary();
				t = ts.get();
				break;
			case '/':
			{
				double d = primary();
				if (d == 0)
				{
					ts.putback(Token(PRINT));
					error("divide by zero");
				}
				left /= d;
				t = ts.get();
				break;
			}
			case '%':
			{
				double d = primary();

				int i1 = int(left);
				if (i1 != left) error("left-hand operand of % not int");

				int i2 = int(d);
				if (i2 != d) error("right-hand operant of % not int");
				if (i2 == 0) error("%: divide by zero");

				left = i1 % i2;
				t = ts.get();
				break;
			}
			default:
				ts.putback(t);
				return left;
			}
		}
	}

	double primary()
	{
		Token t = ts.get();

		switch (t.kind)
		{
		case '(':
		{
			double d = expression();
			t = ts.get();
			if (t.kind != ')') error("')' expected");
			return d;
		}
		case NAME:
		{
			Token t2 = ts.get();

			if (t2.kind == '=')
			{
				double d = expression();
				st.set(t.name, d);
				return d;
			}
			else
			{
				ts.putback(t2);
				return st.get(t.name);
			}
		}
		case NUMBER:
			return t.value;
		case POW:
		{
			// pow(base, power)
			Token t2 = ts.get();
			if (t2.kind != '(') error("expected ( in pow(x,y) call");

			double base = expression();
			t2 = ts.get();
			if (t2.kind != ',') error("expected , in pow(x,y) call");

			double d = expression();
			// Checking to see if pow is an integer
			int power = (int)d;
			if (power != d) error("expected an int-power in pow(x,y) call");
			t2 = ts.get();
			if (t2.kind != ')') error("expected ) in pow(x,y) call");

			return pow(base, power);
		}
		case SQRT:
		{
			Token t2 = ts.get();
			if (t2.kind != '(') error("expected ( in sqrt(...) call");

			double d = expression();
			if (d < 0) error("sqrt of negative number is invalid");

			t2 = ts.get();
			if (t2.kind != ')') error("expected ) in sqrt(...) call");

			return sqrt(d);
		}
		case '-':
			return -primary();
		case '+':
			return primary();
		default:
		{
			ts.clear();
			ts.putback(Token(PRINT));
			error("primary expected");
		}
		}
	}

	void clean_up_mess()
	{
		ts.ignore(PRINT);
	}

	int min(int x, int y)
	{
		if (x < y)
			return x;
		else
			return y;
	}

	int gdc(int x, int y)
	{
		int ans = 1;
		int num = 1;

		int m = min(x, y);

		while (num <= m)
		{
			if (x % num == 0 && y % num == 0) ans = num;
			num++;
		}

		return ans;
	}

	void convert_to_fraction(double ans)
	{
		int precision = 1000000;
		int num = ans * precision;
		int den = precision;
		int GDC = gdc(num, den);

		//cout << "num = " << num << endl;
		//cout << "den = " << den << endl;
		//cout << "gdc = " << GDC << endl;

		cout << "= " << num / GDC << "/" << den / GDC << endl;
	}

	void calculate()
	{
		while (cin)
		{
			try
			{
				cout << PROMPT;
				Token t = ts.get();

				while (t.kind == PRINT || t.kind == NEWLINE) t = ts.get();	// Removes excess ';' or printlines 

				if (t.kind == EXIT) return;
				if (t.kind == CLEAR) {
					system("cls");
					continue;
				}
				if (t.kind == FRACTION)
				{
					convert_to_fraction(st.get("ans"));
					continue;
				}
				if (t.kind == VARIABLES)
				{
					st.display_vars();
					continue;
				}

				ts.putback(t);
				double result = statement();
				if (result != OPERATION_FAILED)
				{
					st.set("ans", result);
					cout << RESULT << result << endl;
				}
				else
				{
					//cout << "Invalid operation...\n";
				}
			}
			catch (exception& e)
			{
				cerr << e.what() << endl;
				clean_up_mess();
			}
		}
	}
} // End of Namespace::Calculator

using namespace Calculator;

int main()
{
	try
	{
		st.declare("ans", 0, false);
		st.declare("pi", 3.1415926535, true);
		st.declare("e", 2.7182818284, true);
		st.declare("k", 1000, true);

		calculate();

		keep_window_open();
		return 0;
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << endl;
		keep_window_open("~~");
		return 1;
	}
	catch (...)
	{
		cerr << "exception\n";
		keep_window_open("~~");
		return 2;
	}
}