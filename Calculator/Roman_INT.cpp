#include "std_lib_facilities.h"
#include "Roman_INT.h"

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