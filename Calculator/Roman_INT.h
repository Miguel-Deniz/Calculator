#ifndef ROMAN_INT
#define ROMAN_INT

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

ostream& operator<<(ostream& os, const Roman_INT& rint);

istream& operator>>(istream& is, Roman_INT& rint);

#endif