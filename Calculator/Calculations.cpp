#include "std_lib_facilities.h"
#include "TokenStream.h"
#include "Calculations.h"

double statement(Token_stream& ts, Symbol_Table& st)
{
	try
	{
		Token t = ts.get();

		switch (t.kind)
		{
		case LET:
			return declaration(ts, st, false);
		case CONST:
			return declaration(ts, st, true);
		default:
			ts.putback(t);
			return expression(ts, st);
		}
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << endl;
		clean_up_mess(ts);
		return OPERATION_FAILED;
	}
	catch (...)
	{
		cerr << "Unknown exception in statement()" << endl;
		clean_up_mess(ts);
		return OPERATION_FAILED;
	}
}

double expression(Token_stream& ts, Symbol_Table& st)
{
	double left = term(ts, st);
	Token t = ts.get();

	while (true)
	{
		switch (t.kind)
		{
		case '+':
			left += term(ts, st);
			t = ts.get();
			break;
		case '-':
			left -= term(ts, st);
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(Token_stream& ts, Symbol_Table& st, bool is_constant)
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

	double d = expression(ts, st);
	st.declare(var_name, d, is_constant);
	return d;
}

double term(Token_stream& ts, Symbol_Table& st)
{
	double left = primary(ts, st);
	Token t = ts.get();

	while (true)
	{
		switch (t.kind)
		{
		case '(':
		{
			// Ex. 2(x)=2x or 8(9)=72
			double d = expression(ts, st);
			Token t2 = ts.get();
			if (t2.kind != ')') error("expected )");
			return left * d;
		}
		case '*':
			left *= primary(ts, st);
			t = ts.get();
			break;
		case '/':
		{
			double d = primary(ts, st);
			if (d == 0)
			{
				ts.putback(Token(PRINT));
				error("divide by zero");
			}
			left /= d;
			t = ts.get();
			break;
		}
		case '^':
		{
			double d = primary(ts, st);
			return pow(left, d);
		}
		case '!':
		{
			if (left == 0) return 0;

			int result = left;
			if (result != left)
			{
				ts.putback(Token(PRINT));
				error("Factorial primary must be an int: x!");
			}

			for (int i = left -1; i >= 1; --i)
				result *= i;

			return result;
		}
		case '%':
		{
			double d = primary(ts, st);

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

double primary(Token_stream& ts, Symbol_Table& st)
{
	Token t = ts.get();

	switch (t.kind)
	{
	case '(':
	{
		double d = expression(ts, st);
		t = ts.get();
		if (t.kind != ')')
		{
			ts.putback(t);
			error("')' expected");
		}
		return d;
	}
	case NAME:
	{
		Token t2 = ts.get();

		if (t2.kind == '=')
		{
			double d = expression(ts, st);
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
		if (t2.kind != '(')
		{
			ts.putback(Token(PRINT));
			error("expected ( in pow(x,y) call");
		}

		double base = expression(ts, st);
		t2 = ts.get();
		if (t2.kind != ',')
		{
			ts.putback(Token(PRINT));
			error("expected , in pow(x,y) call");
		}

		double d = expression(ts, st);
		// Checking to see if pow is an integer
		int power = (int)d;
		if (power != d)
		{
			ts.putback(Token(PRINT));
			error("expected an int-power in pow(x,y) call");
		}
		t2 = ts.get();
		if (t2.kind != ')')
		{
			ts.putback(Token(PRINT));
			error("expected ) in pow(x,y) call");
		}

		return pow(base, power);
	}
	case ABSOLUTE:
	{
		// abs(x)
		Token t2 = ts.get();
		if (t2.kind != '(') error("expected ( in abs(x) call");
		double x = expression(ts, st);
		t2 = ts.get();
		if (t2.kind != ')') error("expected ) in abs(x) call");

		return (x >= 0) ? x : -x;
	}
	case EXPONENTIAL:
	{
		// e(x)
		Token t2 = ts.get();
		if (t2.kind != '(')
		{
			ts.putback(Token(PRINT));
			error("expected ( in exp(x) call");
		}
		double x = expression(ts, st);
		t2 = ts.get();
		if (t2.kind != ')')
		{
			ts.putback(Token(PRINT));
			error("expected ) in exp(x) call");
		}
		return pow(2.718281828, x);
	}
	case SQRT:
	{
		Token t2 = ts.get();
		if (t2.kind != '(')
		{
			ts.putback(Token(PRINT));
			error("expected ( in sqrt(x) call");
		}

		double d = expression(ts, st);
		if (d < 0)
		{
			ts.clear();
			ts.putback(Token(PRINT));
			error("sqrt of negative number is invalid");
		}

		t2 = ts.get();
		if (t2.kind != ')')
		{
			ts.putback(Token(PRINT));
			error("expected ) in sqrt(x) call");
		}

		return sqrt(d);
	}
	case '-':
		return -primary(ts, st);
	case '+':
		return primary(ts, st);
	default:
	{
		ts.clear();
		ts.putback(Token(PRINT));
		error("primary expected");
	}
	}
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

	cout << "= " << num / GDC << "/" << den / GDC << endl;
}

void print(string str)
{
	cout << str << endl;
}

void println()
{
	cout << endl;
}

void display_help()
{
	print("-------------------- Welcome to Console Calculator --------------------");
	print("\tAuthor: Miguel Deniz Lopez");
	print("\tVersion: 2.0");
	print("\tGithub: https://github.com/Miguel-Deniz/Calculator");
	println();

	print("Commands:");
	print("help - Display system commands");
	print("vars - Displays a list of current variables in scope");
	println();

	print("FUNCTIONS:");
	print("abs(x):   Returns the absolute value of 'x'");
	print("exp(x):   Returns e^x");
	print("pow(x,y): Takes base 'x' to the power of 'y'");
	print("sqrt(x):  Takes the square root of the number 'x'");
	println();

	print("OPERANDS:");
	print("+: Formats: +x, x + y");
	print("-: Formats: -x, x - y");
	print("*: Formats: x * y");
	print("/: Formats: x / y");
	print("!: Formats: x!");
	print("^: Formats: x^y");
	print("(): Formats: (expression), -(), +(), ()!, (expression) <+-*/> (expression)");
	println();

	print("VARIABLE DECLARATIONS");
	print("Normal variable format:   [let] <var_name> = <expression>");
	print("Constant variable format: const <var_name> = <expression>");
	println();

	print("ROMAN NUMERALS");
	print("This calculator supports basic roman numerals:");
	print("Roman numerals can be used just like regular numbers.");
	print("As of version 2.0, there are some bugs with roman numerals");
	print("I = 1, V = 5, X = 10, L 50, C = 100, D = 500, M = 1000");
	print("Formats: III = 3, XX = 20, LI = 51");
	println();

	print("BUG REPORTS");
	print("Please submit any found bugs to the github repository issues list.");
	println();
}

void calculate(Token_stream& ts, Symbol_Table& st)
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
			else if (t.kind == FRACTION)
			{
				convert_to_fraction(st.get("ans"));
				continue;
			}
			else if (t.kind == VARIABLES)
			{
				st.display_vars();
				continue;
			}
			else if (t.kind == HELP)
			{
				system("CLS");
				display_help();
				continue;
			}

			ts.putback(t);
			double result = statement(ts, st);
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
			clean_up_mess(ts);
		}
	}
}

void clean_up_mess(Token_stream& ts)
{
	ts.ignore(PRINT);
}