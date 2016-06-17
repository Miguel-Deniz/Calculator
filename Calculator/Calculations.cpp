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
		if (t.kind != ')') error("')' expected");
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
		if (t2.kind != '(') error("expected ( in pow(x,y) call");

		double base = expression(ts, st);
		t2 = ts.get();
		if (t2.kind != ',') error("expected , in pow(x,y) call");

		double d = expression(ts, st);
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

		double d = expression(ts, st);
		if (d < 0) error("sqrt of negative number is invalid");

		t2 = ts.get();
		if (t2.kind != ')') error("expected ) in sqrt(...) call");

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
			clean_up_mess();
		}
	}
}

void clean_up_mess()
{
	ts.ignore(PRINT);
}