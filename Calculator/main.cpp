/*
Simple Calculator

Revision History:
Miguel Deniz 6/17/16 10:43PM
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
#include "TokenStream.h"
#include "Calculations.h"

int main()
{
	try
	{
		st.declare("ans", 0, false);
		st.declare("pi", 3.1415926535, true);
		st.declare("e", 2.7182818284, true);
		st.declare("k", 1000, true);

		calculate(ts, st);

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