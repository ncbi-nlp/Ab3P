#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "MPtok.h"

// These options are probably compile time constants

static char option_tagsep = '_';	// The tagsep character
static char option_replacesep = '-';	// Replace tagsep with this

static void chomp(char *line)
{
	int     i;

	i = strlen(line) - 1;
	while (i >= 0 && line[i] == '\n' || line[i] == '\r')
		line[i--] = '\0';
}

// Data structure and algorithm for finding common pairs.

// read a file of pairs into a data structure,
// the file must be sorted first

void MPtok::init_pair(const string& file_name)
{
	filebuf fb;
	fb.open(file_name.c_str(), ios::in);
	istream is(&fb);
	string pair;

	while (1)
	{
		getline(is, pair);
		if (is.fail()) break;
		if (pair.size() > 0) common_pair.insert(pair);
	}

	fb.close();
}

// List of abbreviations in 3 categories
// ABB = can occur mid sentence
// EOS = can occur at end of sentence
// NUM = only used before numbers

void MPtok::init_abbr(const string& file_name)
{
	filebuf fb;
	fb.open(file_name.c_str(), ios::in);
	istream is(&fb);
	string typ, abb;
	map<string,int> val;
	val["ABB"] = ABB_ABB; val["EOS"] = ABB_EOS; val["NUM"] = ABB_NUM;

	while (is.good())
	{
		is >> typ;
		if (val.count(typ))
		{
			is >> abb;
			if (abb.size() > 0) common_abbr[abb] = val[typ];
		}
	}
	fb.close();
}

static char nextchar(const char *t, int i)
{
	while (isspace(t[i])) i++;
	return t[i];
}

// Look for a token at or prior to the text position

static int lookbehind(const char *t, int i, const char *s, int *tokflag)
{
	int	k = (int) strlen(s) - 1;

	while (i > 0 && isspace(t[i])) i--;

	while (k >= 0 && i >= 0)
	{
		if (k > 0 && tokflag[i]) break;

		if (tolower(s[k]) != tolower(t[i]))
			return -1;
		k--;
		i--;
	}

	return (k < 0 && tokflag[i+1]) ? i + 1 : -1;
}

// Look for a token at or following the text position

static int lookahead(const char *t, int i, const char *s, int *tokflag)
{
	int	k = 0;

	while (isspace(t[i])) i++;

	while (k < strlen(s) && i < strlen(t))
	{
		if (k > 0 && tokflag[i]) break;

		if (tolower(s[k]) != tolower(t[i]))
			return -1;
		k++;
		i++;
	}

	return (k == strlen(s) && tokflag[i]) ? i - (int) strlen(s) : -1;
}

// Set the initial tokens at spaces

void MPtok::tok_0()
{
	int i;

	tokflag[0] = 1;
	for (i = 1; i < text_len; i++)
	{
		tokflag[i] = isspace(text[i]) || (i > 0 && isspace(text[i - 1])) ? 1 : 0;
	}
	tokflag[i] = 1;
}

// Get quotes preceded by open parens
//
// A double quote, preceded by a space or open bracket is a separate token
//

void MPtok::tok_1()
{
	for (int i = 1; i < text_len; i++)
	{
		if (text[i] == '"' && strchr("([{<", text[i-1]))
		{
			tokflag[i] = 1;
			if (i + 1 < text_len) tokflag[i+1] = 1;
		}
	}
}

// Look for ellipses
//
// Three dots in a row is a separate token

void MPtok::tok_2()
{
	for (int i = 1; i + 2 < text_len; i++)
	{
		if (strncmp(&text[i], "...", 3) == 0)
		{
			tokflag[i] = 1;
			if (i + 3 < text_len) tokflag[i+3] = 1;
		}
	}
}

// Non-sentence-ending punctuation
//
// Certain punctuation characters are separate tokens

void MPtok::tok_3()
{
	for (int i = 0; i < text_len; i++)
	{
		// If it is a comma and the next char is not a space and option_comma = 0

		if (option_comma == 0 && text[i] == ',' && isspace(text[i + 1]) == 0)
		{
			// do nothing
		} else if (strchr(",;:@#$%&", text[i]))
		{
			tokflag[i] = 1;
			tokflag[i + 1] = 1;
		}
	}
}

// Separate the slashes
//
// Slashes are a separate token
// except for +/-, +/+, -/-, -/+, and and/or.

void MPtok::tok_5_6_7()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '/')
		{
			tokflag[i] = 1;
			if (i+1 < text_len) tokflag[i+1] = 1;

			// Put back +/-, etc, unless option_hyphen is 1

			if (i - 1 >= 0
			&& i + 1 < text_len
			&& ((option_new < 9
				&& text[i - 1] == '+' || (text[i - 1] == '-' && option_hyphen == 0)
				&& text[i + 1] == '+' || (text[i + 1] == '-' && option_hyphen == 0))
			|| (option_new >= 9
				&& (text[i - 1] == '+' || text[i - 1] == '-')
				&& (text[i + 1] == '+' || text[i + 1] == '-'))))
			{
				tokflag[i - 1] = 1;
				tokflag[i] = tokflag[i+1] = 0;
				tokflag[i + 2] = 1;
			}

			// Put back and/or, etc

			if (option_new <= 7)
			{
				if (i > 5 && strncmp(text + i - 5, " and/or ", 8) == 0)
				{
					for (int j = 1; j < 5; j++)
						tokflag[i - 2 + j] = 0;
				}
			} else
			{
				if (i > 4 && strncmp(text + i - 4, " and/or ", 8) == 0)
				{
					for (int j = 1; j < 6; j++)
						tokflag[i - 3 + j] = 0;
				}
			}
		}
	}
}

// All brackets
//
// Any open or closed bracket is a separate token
//
// Exclamation and question mark
//
// Any question or exclamation mark is a separate token

void MPtok::tok_8_9()
{
	for (int i = 0; i < text_len; i++)
	{
		if (strchr("[](){}<>", text[i])
		|| strchr("?!", text[i]))
		{
			tokflag[i] = 1;
			if (i + 1 < text_len) tokflag[i+1] = 1;
		}
	}
}

// Period at the end of a string may be followed by closed-bracket or quote
//
// A period that is preceded by a non-period
// and optionally followed by a close paren
// and any amount of space at the end of the string
// is a separate token.

void MPtok::tok_10()
{
	for (int i = text_len - 1; i >= 0; i--)
	{
		if (isspace(text[i])) continue;
		if (strchr("])}>\"'", text[i])) continue;
		if (text[i] != '.') break;
		if (text[i] == '.' && (i - 1 < 0 || text[i-1] != '.'))
		{
			tokflag[i] = 1;
			if (i + 1 < text_len) tokflag[i+1] = 1;
		}
	}
}

// Period followed by a capitalized word
//
// A period preceded by a character that is not another period and not a space
// and followed by a space then an upper case letter is a separate token

void MPtok::tok_11()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& (i + 1 < text_len && isspace(text[i+1]))
		&& (i - 1 < 0 || text[i - 1] != '.' || isspace(text[i-1]) == 0)
		&& isupper(nextchar(text, i + 1)))
			tokflag[i] = 1;
	}
}

// A normal word followed by a period
//
// A period followed by a space
// and preceded by 2 or more alphabetic characters or hyphens
// is a separate token

void MPtok::tok_12()
{
	int wcnt = 0;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& tokflag[i + 1]
		&& wcnt >= 2)
			tokflag[i] = 1;

		if (isalpha(text[i]) || text[i] == '-')
			++wcnt;
		else
			wcnt = 0;
	}
}

// A non-normal token (that has no lower case letters) followed by a period
//
// A period at the end of a token made of characters excluding lower case
// is a separate token

void MPtok::tok_13()
{
	int	stok = 0;
	int	wcnt = 0;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& tokflag[i + 1]
		&& wcnt >= 2)
			tokflag[i] = 1;

		if (tokflag[i] == 1) stok = 1;

		if (islower(text[i]) || text[i] == '.')
		{
			stok = 0;
			wcnt = 0;
		}

		if (stok)
			wcnt++;
	}
}

// put some periods with single-letter abbreviations
//
// A single alphabetic token followed by a period followed
// by a token that does not begin with an upper case letter
// or number is taken to be an abbreviation and the period
// does not start a new token.
//
// NOTE: This does not recognize initials in people's names,
//	 that problem is not simply solved.

void MPtok::tok_14()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& i - 1 >= 0 && isalpha(text[i - 1]) && tokflag[i - 1]
		&& tokflag[i + 1]
		&& isupper(nextchar(text, i + 1)) == 0
		&& isdigit(nextchar(text, i + 1)) == 0
		&& nextchar(text, i + 1) != '('
		)
		{
			tokflag[i] = 0;
		}
	}
}

void MPtok::tok_15()
{
	int	i, j, k, a;
	char	buff[MAX_ABB + 1];

	for (i = 0; i < text_len; i++)
	{
		// only start at a current token

		if (! tokflag[i]) continue;

		// find alphabetic followed by period

		buff[0] = '\0';
		for (k = 0; i + k < text_len && k < MAX_ABB; k++)
		{
			buff[k] = text[i+k]; buff[k+1] = '\0';
			if (k > 0 && buff[k] == '.') break; // this is good
			if (! isalpha(buff[k])) { buff[0] = '\0'; break; } // this is not good
		}

		if (buff[0] == '\0' || i + k == text_len || k == MAX_ABB) continue;

		// at this point, buff[k] == '.' add 1 to make it the length

		k++;

		// if not found, try finding a concatenated abbrev

		if (! common_abbr.count(buff))
		{
			for (; i + k < text_len && k < MAX_ABB; k++)
			{
				buff[k] = text[i+k]; buff[k+1] = '\0';
				if (k > 0 && buff[k] == '.') break; // this is good
				if (! isalpha(buff[k])) { buff[0] = '\0'; break; } // this is not good
			}

			if (buff[0] == '\0' || i + k == text_len || k == MAX_ABB) continue;

			// at this point, buff[k] == '.' add 1 to make it the length

			k++;
		}

		// if not found, give up

		if (! common_abbr.count(buff)) continue;

		if (common_abbr[buff] == ABB_NUM)
		{
			for (j = i + k; j < text_len && isspace(text[j]); j++) ;	// next must be a number
			if (! isdigit(text[j])) continue;				// go to next abbreviation
		} else if (common_abbr[buff] == ABB_EOS)
		{
			for (j = i + k; j < text_len && isspace(text[j]); j++) ;	// if next token is upper case letter
			if (isupper(text[j])) tokflag[i + (--k)] = 1;			// tokenize the final period of this abbreviation
		}

		// clear all token flags

		for (j = 1; j < k; j++) tokflag[i + j] = 0;
	}
}

// Check for common pairs that should not be considered sentence breaks

void MPtok::tok_15_1()
{
	int	i, j, k, tnum, p;
	char	buff[MAX_ABB + 1];

	for (i = 0; i < text_len; i++)
	{
		if (! tokflag[i]) continue;

		// must be alphanumeric token followed by period token followed by space followed by alphanumeric token

		tnum = 0;
		buff[0] = '\0';
		for (p = k = 0; i + k < text_len && k < MAX_ABB; k++)
		{
			buff[k] = text[i+k]; buff[k+1] = '\0';

			if (isspace(buff[k]))
			{
				if (tnum == 2) break;					// this is good
				else if (tnum == 1) continue;				// ok
				else { buff[0] = '\0'; break; }				// this shouldn't happen
			}

			if (tokflag[i+k])
			{
				if (tnum > 2) break;					// done
				else tnum++;
			}

			if (tnum == 1 && buff[k] == '.') p = k;
			if (tnum == 1 && buff[k] != '.') { buff[0] = '\0'; break; }	// nope
			if (! isalnum(buff[k])) { buff[0] = '\0'; break; }		// nope
		}

		if (buff[0] == '\0' || i + k == text_len || k == MAX_ABB) continue;

		// at this point buff is a potential pair, so untokenize the period, that's all

		if (common_pair.count(buff))
			tokflag[p] = 0;
	}
}

// Get cases where a space after a sentence has been omitted
//
// A period that occurs in a token consisting of alphabetic
// letters with a vowel to the left and the right is a
// separate token.

void MPtok::tok_16()
{
	int	j;
	int	has_vowel;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.' && tokflag[i] == 0)
		{
			has_vowel = 0;
			for (j = i - 1; j >= 0; --j)
			{
				if (isalpha(text[j]) == 0)
					break;
				if (strchr("aeiouAEIOU", text[j]))
					has_vowel = 1;
				if (tokflag[j])
					break;
			}
			if ((j >= 0 && tokflag[j] == 0) || has_vowel == 0)
				continue;

			j = i + 1;

			has_vowel = 0;
			for (; j < text_len && tokflag[j] == 0; ++j)
			{
				if (isalpha(text[j]) == 0)
					break;
				if (strchr("aeiouAEIOU", text[j]))
					has_vowel = 1;
			}

			if ((j < text_len && tokflag[j] == 0) || has_vowel == 0)
				continue;

			tokflag[i] = 1;
			tokflag[i + 1] = 1;
		}
	}
}

// Correction to tok_16,
// Don't count if the token before is a single letter
// or the token following is a single letter other than 'a'.
// Also, don't count if the token to the right is gov, com, edu, etc.
// because those are web addresses!

#define COMPLEX_WINDOW 40

enum {COMPLEX_NOT = 0, COMPLEX_YES, COMPLEX_DONE};

struct _complex {
	int	flag;
	int	offset;
	const char *str;
	int	len;
} complex[] = {
	COMPLEX_YES, 0, "complex", 7,
	COMPLEX_NOT, 0, "complexi", 8,
	COMPLEX_NOT, 0, "complexed", 9,
	COMPLEX_NOT, 0, "complexa", 8,
	COMPLEX_NOT, 0, "complex-", 8,
	COMPLEX_NOT, 0, "complexl", 8,
	COMPLEX_NOT, 0, "complexu", 8,
	COMPLEX_NOT, -1, "-complex", 7,
	COMPLEX_NOT, -2, "nocomplex", 9,
	COMPLEX_NOT, -3, "subcomplex", 10,
	COMPLEX_YES, 0, "hybrid", 6,
	COMPLEX_NOT, 0, "hybridi", 7,
	COMPLEX_NOT, 0, "hybrido", 7,
	COMPLEX_NOT, 0, "hybrida", 7,
	COMPLEX_NOT, 0, "hybrid-", 7,
	COMPLEX_NOT, -1, "-hybrid", 7,
	COMPLEX_YES, 0, "duplex", 6,
	COMPLEX_NOT, -1, "oduplex", 7,
	COMPLEX_DONE, 0, NULL, 0,
};

int MPtok::complex_check()
{
	int	last_period = -2*COMPLEX_WINDOW;
	int	last_complex = -2*COMPLEX_WINDOW;
	int	i, j;
	int	complex_match;

	for (i = 0; i < text_len; i++)
	{
		if (text[i] == '.')
		{
			if (i - last_complex <= COMPLEX_WINDOW)
				return 1;
			last_period = i;
		}

		complex_match = 0;
		for (j = 0; complex[j].str; j++)
		{
			if (complex[j].flag == COMPLEX_NOT)
			{
				if (i + complex[j].offset >= 0
				&& strncmp(text+i+complex[j].offset, complex[j].str, complex[j].len) == 0)
				{
					// don't match here
					complex_match = 0;
				}
			} else if (complex[j].flag == COMPLEX_YES)
			{
				if (i + complex[j].offset >= 0
				&& strncmp(text+i+complex[j].offset, complex[j].str, complex[j].len) == 0)
				{
					// match here
					complex_match = 1;
				}
			}
		}

		if (complex_match)
		{
			if (i - last_period <= COMPLEX_WINDOW)
				return 1;
			last_complex = i;
		}
	}
	return 0;
}

void MPtok::tok_16_1()
{
	int	i, j;
	char	v1, v2;
	int	c1, c2;

	if (option_new == 3 && strstr(text, "complex"))
		return;

	if (option_new >= 4 && complex_check())
		return;

	for (i = 0; i < text_len; i++)
	{
		if (text[i] == '.' && tokflag[i] == 0)
		{
			char	suffix[10];
			int	s_i;

			v1 = '\0';
			c1 = 0;
			for (j = i - 1; j >= 0; --j)
			{
				if (isalpha(text[j]) == 0)
					break;
				if (strchr("aeiouAEIOU", text[j]))
					v1 = tolower(text[j]);
				c1++;
				if (tokflag[j])
					break;
			}
			if ((j >= 0 && tokflag[j] == 0)
			|| v1 == '\0'
			|| c1 == 1)
				continue;

			j = i + 1;

			v2 = '\0';
			c2 = 0;
			s_i = 0;
			for (; j < text_len && tokflag[j] == 0; ++j)
			{
				if (isalpha(text[j]) == 0)
					break;
				if (strchr("aeiouAEIOU", text[j]))
					v2 = tolower(text[j]);
				if (s_i < 3)
					suffix[s_i++] = tolower(text[j]); suffix[s_i] = '\0';
				c2++;
			}

			if ((j < text_len && tokflag[j] == 0)
			|| v2 == '\0'
			|| (c2 == 1 && v2 != 'a')
			|| (c2 == 3 && tokflag[j] == 1 && s_i == 3
				&& (strcmp(suffix, "gov") == 0
					|| strcmp(suffix, "edu") == 0
					|| strcmp(suffix, "org") == 0
					|| strcmp(suffix, "com") == 0)))
				continue;

			tokflag[i] = 1;
			tokflag[i + 1] = 1;
		}
	}
}


// Numeric endings of sentences
//
// A period after a numeric token followed by a token that starts
// with an alphabetic character, is a separate token.
//
// This should be covered already by tok_13

void MPtok::tok_17()
{
	int	j;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& tokflag[i] == 0
		&& tokflag[i + 1])
		{
			for (j = i - 1; j >= 0 && isdigit(text[j]) && tokflag[j] == 0; --j)
				;
			if (j >= 0 && j < i - 1 && tokflag[j] && isalpha(nextchar(text, i + 1)))
				tokflag[i] = 1;
		}
	}
}

// period at end of string is a token

void MPtok::tok_20()
{
	for (int i = text_len - 1; i >= 0; --i)
	{
		if (isspace(text[i]))
			continue;

		if (strchr(".!?", text[i]))
			tokflag[i] = 1;

		break;
	}
}

// a period that follows a non-common word, and that is
// followed by a lower case common word is probably not a token

void MPtok::tok_20_1()
{
	int	j;

	for (int i = 0; i < text_len; ++i)
	{
		if (text[i] == '.' && tokflag[i] == 1)
		{
			int tcnt, lcnt, ocnt;
			tcnt = lcnt = ocnt = 0;

			// make sure the previous word was *not* common

			for (j = i - 1; j >= 0; j--)
			{
				if (isspace(text[j])) continue;
				if (option_new >= 2)
				{
					if (islower(text[j]) == 0 && text[j] != '-') ocnt++;
				} else
				{
					if (! islower(text[j])) ocnt++;
				}

				if (tokflag[j] || j == 0)
				{
					if (ocnt == 0)
					{
						goto nexti;
					}
					break;
				}
			}

			tcnt = lcnt = ocnt = 0;

			// make sure the next word is common

			for (j = i + 1; j < text_len; j++)
			{
				if (isspace(text[j])) continue;
				if (tokflag[j]) tcnt++;

				if (tcnt == 2 || j == text_len - 1)
				{
					if (lcnt > 0 && ocnt == 0) tokflag[i] = 0;
					break;
				}

				if (islower(text[j])) lcnt++;
				else ocnt++;
			}
		}
nexti:		;
	}
}

// tokenized period followed by non-space other than close paren
// is not a token

void MPtok::tok_20_2()
{
	int	j;

	for (int i = 0; i < text_len - 1; ++i)
	{
		if (text[i] == '.' && tokflag[i] == 1
		&& strchr(" ()[]\"\'\n\t\r", text[i+1]) == 0)
		{
			tokflag[i] = 0;
		}
	}
}


// long dash
//
// A pair of hyphens is a complete token

void MPtok::tok_21()
{
	for (int i = 0; i + 1 < text_len; i++)
	{
		if (strncmp(&text[i], "--", 2) == 0)
		{
			tokflag[i] = 1;
			if (i + 2 < text_len)
			{
				i += 2;
				tokflag[i] = 1;
			}
		}
	}
}

// hyphens
//
// If specified as an option, a hyphen between letters is a complete token

void MPtok::tok_21a()
{
	if (option_hyphen == 0) return;

	for (int i = 0; i + 1 < text_len; i++)
	{
		if (text[i] == '-'
		&& (i == 0 || text[i-1] != '-')
		&& text[i+1] != '-')
		{
			tokflag[i] = 1;
			tokflag[i+1] = 1;
		}
	}
}


// quote
//
// Any double quote is a separate token

void MPtok::tok_22()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '"')
		{
			tokflag[i] = 1;
			if (i + 1 < text_len)
			{
				i += 1;
				tokflag[i] = 1;
			}
		}
	}
}

// possessive
//
// Any single quote at the end of a token that is not
// preceded by a single quote is a separate token

void MPtok::tok_23()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '\''
		&& (i - 1 >= 0 && text[i - 1] != '\'')
		&& tokflag[i + 1])
		{
			tokflag[i] = 1;
		}
	}
}


// quote
//
// If a single quote starts a token, or is preceded by a
// single quote, and followed by a character
// that is not a single quote, then
// the character to it's right is the start of a new token

void MPtok::tok_24()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '\''
		&& (tokflag[i] == 1 || (i - 1 >= 0 && text[i - 1] == '\''))
		&& (i + 1 < text_len && text[i + 1] != '\''))
		{
			tokflag[i + 1] = 1;
		}
	}
}

// put back possessive
//
// A single quote that is a whole token followed by a lower case s
// that is also a whole token (without space between them)
// should be merged into a single token

void MPtok::tok_25()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '\''
		&& tokflag[i] == 1
		&& i + 1 < text_len && text[i + 1] == 's'
		&& tokflag[i+1] == 1
		&& (i + 2 >= text_len || isspace(text[i + 2]) || tokflag[i + 2] == 1))
		{
			tokflag[i + 1] = 0;
		}
	}
}

// quote
//
// A pair of single quotes is a separate token

void MPtok::tok_26()
{
	for (int i = 0; i < text_len; i++)
	{
		if (strncmp(&text[i], "''", 2) == 0
		|| strncmp(&text[i], "``", 2) == 0)
		{
			tokflag[i] = 1;
			if (i + 2 < text_len) tokflag[i + 2] = 1;
		}
	}
}

// possessive
//
// A single quote followed by a letter s is a possessive

void MPtok::tok_27()
{
	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '\''
		&& i + 1 < text_len
		&& tolower(text[i + 1]) == 's'
		&& (i + 2 >= text_len || tokflag[i + 2]))
		{
			tokflag[i] = 1;
		}
	}
}

// split "cannot" to "can not"
//
// A single token that is the word cannot (in any case)
// is split into two words

void MPtok::tok_28()
{
	for (int i = 0; i < text_len; i++)
	{
		if ((strncmp(&text[i], "cannot", 6) == 0
		|| strncmp(&text[i], "Cannot", 6) == 0)
		&& tokflag[i + 6])
		{
			tokflag[i + 3] = 1;
		}
	}
}

// put list item elements back at sentence end
//
// A period that is preceded by an alphanumeric (no space)
// and any amount of preceding space and an end-mark
// stays with the alphanumeric.

void MPtok::tok_29()
{
	int	j;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '.'
		&& tokflag[i] && tokflag[i + 1]
		&& i - 1 >= 0 && isalnum(text[i - 1])
		&& tokflag[i - 1]
		&& ((j = lookbehind(text, i-2, ".", tokflag)) >= 0
		||  (j = lookbehind(text, i-2, "?", tokflag)) >= 0
		||  (j = lookbehind(text, i-2, "!", tokflag)) >= 0)
		&& tokflag[j])
		{
			tokflag[i] = 0;
		}
	}
}

// attach list elements to the beginnings of their sentences
// this means, attach the period to the list element
//
// a list element is a single letter or a one or two digits
// which is preceded by an end of sentence ".!?;"
// or colon (provided it doesn't belong to a proportion construct)

void MPtok::tok_29a()
{
	int	i, j;

	for (i = 0; i < text_len; i++)
	{
		if (text[i] == '.' && tokflag[i])
		{
			// Look back, make sure the token before the period
			// is either single alphanumeric, or at most a two digit number
			// and the character before that is a punctuation ".?!:,"

			int tcnt, acnt, dcnt, pcnt, ocnt, scnt;
			tcnt = acnt = dcnt = pcnt = ocnt = scnt = 0;
			char p;

			for (j = i - 1; j >= 0; j--)
			{
				if (isspace(text[j])) { scnt++; continue; }
				else if (tcnt == 0 && isalpha(text[j])) ++acnt;
				else if (tcnt == 0 && isdigit(text[j])) ++dcnt;
				else if (tcnt == 1 && strchr(".!?:;,", text[j])) { pcnt++; p = text[j]; }
				else ocnt++;

				if (tokflag[j] || j == 0)
				{
					tcnt++;
					if (tcnt == 1 && ocnt == 0 && scnt == 0
					&& ((acnt == 1 && dcnt == 0) || (acnt == 0 && dcnt > 0 && dcnt <= 2)))
					{
						// This is acceptable
					} else if (tcnt == 2 && pcnt <= 1 && ocnt == 0 && scnt > 0)
					{
						if (p == ':')
						{
							while (--j >= 0 && isspace(text[j]))
								;
							if (j >= 0 && isdigit(text[j]))
							{
								// It's probably a proportion
								break;
							}
						}
						// Jackpot
						tokflag[i] = 0;
					} else
					{
						// This is not
						break;
					}
					scnt = 0;
				}
			}
		}
	}
}

// list elements at the beginning of a string
//
// An alphanumeric token followed by a period
// at the beginning of the line stays with the
// alphanumeric

void MPtok::tok_30()
{
	int	i = 0;

	while (isspace(text[i])) i++;

	if (isalnum(text[i])
	&& tokflag[i]
	&& i + 1 < text_len
	&& text[i + 1] == '.'
	&& tokflag[i + 1])
	{
		tokflag[i + 1] = 0;
	}
}

// process American style numbers

void MPtok::tok_31()
{
	int	j;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == ','
		&& i + 3 < text_len
		&& tokflag[i] && tokflag[i + 1]
		&& isdigit(text[i + 1])
		&& isdigit(text[i + 2])
		&& isdigit(text[i + 3])
		&& i - 1 >= 0 && isdigit(text[i - 1])
		)
		{
			tokflag[i] = 0;
			tokflag[i + 1] = 0;
		}
	}
}

// process British style numbers

void MPtok::tok_32()
{
	int	j;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == ' '
		&& i + 3 < text_len
		&& tokflag[i] && tokflag[i + 1]
		&& isdigit(text[i + 1])
		&& isdigit(text[i + 2])
		&& isdigit(text[i + 3])
		&& i - 1 >= 0 && isdigit(text[i - 1])
		)
		{
			tokflag[i] = 0;
			tokflag[i + 1] = 0;
		}
	}
}

// tokenize unicode escapes
//
// Added

void MPtok::tok_33()
{
	int	j;

	for (int i = 0; i < text_len; i++)
	{
		if (text[i] == '&')
		{
			if (text[i + 1] == '#')
			{
				for (j = i + 2; isdigit(text[j]); j++)
					;
			} else
			{
				for (j = i + 1; isalpha(text[j]); j++)
					;
			}

			if (text[j] == ';')
			{
				// Tokenize the escape, untokenize everything inside

				tokflag[i] = 1;
				for (i++; i <= j; i++) tokflag[i] = 0;
				tokflag[i] = 1;
			}
		}
	}
}

// Remove tags if they are present

void MPtok::tok_un()
{
	int untok = 0;
	for (int i = 0; text[i]; ++i)
	{
		if (isspace(text[i])) untok = 0;
		if (text[i] == option_tagsep) untok = 1;
		if (untok) text[i] = ' ';
	}
}


void MPtok::set_tokflag()
{
	int	i;

	tok_0();
	tok_1();
	tok_2();
	tok_3();

	// step 4 replaces tag char, this is done at output

	tok_5_6_7();
	tok_8_9();

	tok_10();
	tok_11();
	if (option_new >= 1)
	{
		tok_21();
		tok_21a();
		tok_22();
		tok_23();
		tok_24();
		tok_25();
		tok_26();
		tok_27();
	}
	tok_12();
	tok_13();
	tok_14();
	if (option_new <= 5)
		tok_15();
	if (option_new < 2)
		tok_16();
	tok_17();

	// steps 18 and 19 recognize periods within parens,
	// and this is moved to the segmentation section

	tok_20();
	if (option_new >= 1)
	{
		tok_20_1();
		tok_20_2();
		if (option_new >= 2)
			tok_16_1();
		if (option_new >= 6)
			tok_15();
		if (option_new >= 7)
			tok_15_1();
	}
	if (option_new < 1)
	{
		tok_21();
		tok_21a();
		tok_22();
		tok_23();
		tok_24();
		tok_25();
		tok_26();
		tok_27();
	}
	tok_28();
	if (option_new >= 1)
		tok_29a();
	else
		tok_29();
	tok_30();
	tok_31();
	tok_32();

	tok_33();
}

/* set_endflag
** 
** After tokflag has been set, find the possible sentence endings.
*/

void MPtok::set_endflag()
{
	int	i;

	// The following tests look for end-stops and label them.
	// They include steps 18 and 19

	for (i = 0; i <= text_len; i++)
		endflag[i] = 0;

	// Count the number of unmatched parens

	int up = 0;	// unmatched round parens
	int ub = 0;	// unmatched brackets

	for (i = 0; i < text_len; i++)
	{
		if (text[i] == '(') ++up;
		if (text[i] == ')') --up;
		if (text[i] == '[') ++ub;
		if (text[i] == ']') --ub;
		if (up < 0) up = 0;
		if (ub < 0) ub = 0;
	}

	// Now find the end-of-sentence marks

	// tok_18: periods within parentheses, allow for nesting
	// tok_19: periods within brackets, allow for nesting
	//	the perl version solves this by putting the period
	//	back with the previous token, but a better solution
	//	is to allow it to be tokenized but just don't
	// 	allow it to be an end-of-sentence.
	//	Therefore, these are moved to the segmentation
	//	section

	int p = 0;	// round parens
	int b = 0;	// brackets

	for (i = 0; i < text_len; i++)
	{
		if (text[i] == '(') ++p;
		if (text[i] == ')') --p;
		if (text[i] == '[') ++b;
		if (text[i] == ']') --b;
		if (p < 0) p = 0;
		if (b < 0) b = 0;

		if (strchr(".!?", text[i])
		&& tokflag[i]
		&& tokflag[i + 1])
		{
			if (option_segment && p <= up && b <= ub)
				endflag[i] = 1;

			// This is optional to join periods with
			// probable abbreviations

			if (p > up || b > ub)
				tokflag[i] = 0;
		}
	}

	// endtokens followed by a single or double quote, which matches
	// a single or double quote in the previous sentence

	if (option_new >= 1)
	{
		int	dquo, squo;
		dquo = squo = 0;

		for (i = 0; i < text_len; i++)
		{
			if (text[i] == '"') dquo = ! dquo;
			else if (text[i] == '\'') squo = ! squo;
			else if (endflag[i])
			{
				if ((text[i+1] == '"' && dquo) || (text[i+1] == '\'' && squo))
				{
					endflag[i] = 0;

					// But don't end at all if the next token is something
					// other than an upper case letter.

					if (option_new >= 2)
					{
						int	j;
						int	ok = 0;

						for (j = i + 2; j < text_len; j++)
						{
							if (isspace(text[j])) continue;
							// if (isupper(text[j]))
							if (isupper(text[j]) || text[j] == '(')
							{
								ok = 1;
								break;
							}
							if (tokflag[j]) break;
						}

						if (ok)
							endflag[i+1] = 1;
					} else
					{
						endflag[i+1] = 1;
					}
				}
				dquo = squo = 0;
			}
		}
	}
}


/* set_endflag_01
** 
** After tokflag has been set, find the possible sentence endings.
** This has improved paren matching.
*/

#define MAX_MATCH 500		// Maximum length to get a paren match

void MPtok::set_endflag_01()
{
	int	match[text_len];
	int	i, j;

	// The following tests look for end-stops and label them.
	// They include steps 18 and 19

	for (i = 0; i <= text_len; i++)
		endflag[i] = 0;

	for (i = 0; i < text_len; i++)
		match[i] = 0;

	for (i = text_len - 1; i >= 0; i--)
	{
		if (text[i] == '(' || text[i] == '[')
		{
			for (j = i + 1; text[j] && j - i <= MAX_MATCH; j++)
			{
				// Skip parens that are already matched

				if (match[j] > j)
				{
					j = match[j];
					continue;
				}

				// Look for a matching close paren

				if (match[j] == 0
				&& ((text[i] == '(' && text[j] == ')')
				|| (text[i] == '[' && text[j] == ']')))
				{
					match[i] = j;
					match[j] = i;
					break;
				}
			}
		}
	}

	int next_match = 0;
	for (i = 0; i < text_len; i++)
	{
		if (match[i] > next_match)
			next_match = match[i];

		if (strchr(".!?", text[i])
		&& tokflag[i]
		&& tokflag[i + 1]
		&& (option_new <= 4 || option_doteos == 1 || (i > 0 && isspace(text[i-1]) == 0)))
		{
			if (i <= next_match)
				tokflag[i] = 0;
			else if (option_segment)
				endflag[i] = 1;
		}
	}

	// endtokens followed by a single or double quote, which matches
	// a single or double quote in the previous sentence

	int	dquo, squo;
	dquo = squo = 0;

	for (i = 0; i < text_len; i++)
	{
		if (option_new <= 7 && text[i] == '"') dquo = ! dquo;
		else if (option_new >= 8 && text[i] == '"' && tokflag[i] && tokflag[i+1]) dquo = ! dquo;
		else if (option_new <= 7 && text[i] == '\'') squo = ! squo;
		else if (option_new >= 8 && text[i] == '\''
		&& tokflag[i] && (tokflag[i+1] || (text[i+1] == '\'' && tokflag[i+2]))) squo = ! squo;
		else if (endflag[i])
		{
			if ((text[i+1] == '"' && dquo) || (text[i+1] == '\'' && squo))
			{
				endflag[i] = 0;

				// But don't end at all if the next token is something
				// other than an upper case letter.

				if (option_new >= 2)
				{
					int	j;
					int	ok = 0;

					for (j = i + 2; j < text_len; j++)
					{
						if (isspace(text[j])) continue;
						// if (isupper(text[j]))
						if (isupper(text[j]) || text[j] == '(')
						{
							ok = 1;
							break;
						}
						if (tokflag[j]) break;
					}

					if (ok)
						endflag[i+1] = 1;
				} else
				{
					endflag[i+1] = 1;
				}
			}
			dquo = squo = 0;
		}
	}
}


// Size buffer: return the size of the buffer required to hold all of the tokenized text.
// It can be simply estimated by a formula that depends only on the length of text and number of tokens.

int MPtok::size_buff()
{
	int	size = 1;			// Start with null terminator
	int	t = option_pretag.size();	// for each tag, the length of the UNTAG string

	if (t <= 0) t = 1;			// Make sure there is at least one
	t += 2;					// Add one for underscore and one for space

	for (int i = 0; i < text_len; i++)
	{
		size++;				// Count all characters
		if (tokflag[i]) size += t;	// Count token delimiters (may overcount)
		if (endflag[i]) size++;		// Add one for newline
	}
	return size;
}


/* append_token
** 
** Save a single token to a buffer.
*/

void MPtok::append_token(string& buff, int& sp, char *tok, int ef)
{
	// Convert tag separator chars and back quotes (?)

	for (int i = 0; tok[i]; i++)
	{
		if (tok[i] == option_tagsep) tok[i] = option_replacesep;
		if (tok[i] == '`') tok[i] = '\'';
	}

	// Skip whitespace if tokens are being output
	// Otherwise, skip whitespace at the start of a sentence

	if (option_token || ! sp) while (isspace(*tok)) ++tok;

	// Save the token

	if (strlen(tok) > 0)
	{
		// Add delimiter if needed

		if (option_token && sp) buff += ' ';

		// Append token to output

		if (option_new < 9)
		{
			while (*tok && (! option_token || ! isspace(*tok)))
				buff += *(tok++);
		} else
		{
			while (*tok)
				buff += *(tok++);
		}

		sp = 1;

		// Add tag holders

		if (option_token && option_pretag.size() > 0)
		{
			buff += option_tagsep;
			buff += option_pretag;
		}

		// If it was end of sentence, then add newline

		if (ef)
		{
			buff += '\n';
			sp = 0;
		}
	}
}

// Strip whitespace after sentences

static void adjust_space(string& buff)
{
	while (buff.size() > 0 && isspace(buff[0])) buff.erase(0, 1);

	// delete two spaces in a row, but keep newlines

	for (int i = 1; i < buff.size(); i++)
	{
		if (isspace(buff[i]) && isspace(buff[i-1]))
			buff.erase((buff[i] == '\n')?(--i):(i--), 1);
	}

	for (int i = buff.size() -  1; i >= 0 && isspace(buff[i]); i--)
		buff.erase(i, 1);
}

/* token_string
**
** After the tokflag and endflag have been set, copy the tokens to the buffer.
*/

string MPtok::token_string()
{
	string buff;

	int	i;

	// Move token starts to non-whitespace chars

	int last_tok = 0;
	for (i = 0; i < text_len; i++)
	{
		if (tokflag[i] == 1 && isspace(text[i]))
		{
			tokflag[i] = 0;
			last_tok = 1;
		} else if (isspace(text[i]) == 0 && last_tok)
		{
			tokflag[i] = 1;
			last_tok = 0;
		}
	}

	// Extract the tokens and print them out now

	char	*tok = new char[text_len + 1];
	int	pos = 0;
	int	sp = 0;
	int	ef = 0;

	tok[pos] = '\0';

	for (i = 0; i <= text_len; i++)
	{
		// The start of a new token

		if (tokflag[i])
		{
			// Print the current token

			append_token(buff, sp, tok, ef);

			// Start a new token

			pos = 0;
			tok[pos] = '\0';

			ef = 0;
		}

		// Append to the current token

		tok[pos++] = text[i];
		tok[pos] = '\0';

		// If any of the characters in the token are endflagged,
		// Then pass this information along for end-of-sentence

		if (endflag[i]) ef = 1;
	}

	// Print the last token

	append_token(buff, sp, tok, ef);

	delete[] tok;

	// Adjust the end of sentence boundaries

	adjust_space(buff);

	return buff;
}

void MPtok::map_escapes()
{
	char	*s;
	int	j, k, ch;
	char	buff[10];

	k = 0;
	for (int i = 0; text[i]; i++)
	{
		if (text[i] == '&' && text[i + 1] == '#')
		{
			for (s = &buff[0], j = 2; j <= 4 && i + j < text_len && isdigit(text[i + j]); j++)
				*s++ = text[i + j];
			*s = '\0';
			ch = atoi(buff);
			if (strlen(buff) > 0 && text[i + j] == ';' && ch > 0 && ch <= 256)
			{
				text[k] = ch;
				if (! text[k]) text[k] = ' ';
				k++;
				i = i + j;
				continue;
			}
		}
		text[k++] = text[i];
	}
	text[k] = '\0';
	text_len = k;
}

MPtok::MPtok(string idir, const string& cnam)
{
	tok_initialized = 0;

	if (idir.size() == 0)
	{
		char	*p = getenv("MEDPOST_HOME");
		if (p && strlen(p))
		{
			idir = p;

			int found = idir.find("=");
			if (found != string::npos)
				idir = idir.substr(found + 1);
		}
	}


	if (idir.size() == 0)
	{
		char    buff[1000];
		FILE *fp = fopen("path_medpost", "r");
		if (fp)
		{
			if (fgets(buff, 1000, fp))
			{
				chomp(buff);
				idir = &buff[0];
			}
			fclose(fp);
		}
	}

	if (idir.size() == 0)
		idir = "/home/natxie/CPP64/lib/FIXED_DATA/";

	option_dir = idir;

	option_token = 1;
	option_segment = 1;
	option_hyphen = 0;
	option_comma = 1;
	option_pretok = 0;
	option_new = MPTOK_VERSION;
	option_doteos = 0;

	if (cnam.size() > 0)
	{
		option_cnam = "_";
		option_cnam += cnam;
	}

	init();
}

void MPtok::init(void)
{
	if (tok_initialized) return;

	string fname;

	fname = option_dir + "/medpost" + option_cnam + ".pairs";
	init_pair(fname);

	fname = option_dir + "/medpost" + option_cnam + ".abbr";
	init_abbr(fname);

	tok_initialized = 1;
}

MPtok::~MPtok()
{
}

// Global tokenizer

string MPtok::tokenize(const string& txt, int mt)
{
	if (option_pretok) return save_string(txt);

	option_token = mt;
	text_len = txt.size();
	if (text_len == 0) return string("");

	text = new char[text_len + 1];
	strcpy(text, txt.c_str());

	map_escapes();

	if (text_len == 0) return NULL;

	tokflag = new int[text_len + 1];
	endflag = new int[text_len + 1];

	set_tokflag();
	if (option_new < 3)
		set_endflag();
	else
		set_endflag_01();

	string buff = token_string();
	save_string(buff);

	delete[] text; text = NULL;
	delete[] tokflag; tokflag = NULL;
	delete[] endflag; endflag = NULL;

	return buff;
}

string MPtok::tokenize(const string& text)
{
	return tokenize(text, 1);
}

string MPtok::segment(const string& text)
{
	sent.clear();

	// tokenize the text

	int save_option_segment = option_segment;
	option_segment = 1;
	string buff = tokenize(text, 0);
	option_segment = save_option_segment;

	if (buff.size() == 0) return text;

	int found = 0;
	int pos = 0;

	while (pos < buff.size())
	{
		found = buff.find('\n', pos);
		if (found == string::npos)
		{
			sent.push_back(buff.substr(pos));
			pos = buff.size();
		} else
		{
			sent.push_back(buff.substr(pos, found - pos));
			pos = found + 1;
		}
	}

	return buff;
}

string MPtok::save_string(const string& s)
{
	stringstream ss (stringstream::in | stringstream::out);
	string	w, t;
	int	found;
	string	ret;

	word.clear();
	tag.clear();

	ss << s;
	while (ss.good())
	{
		ss >> w;
		if (w.size() == 0) break;

		found = w.find('_');

		if (found != string::npos)
		{
			t = w.substr(found + 1);
			w.resize(found);
			word.push_back(w);
			tag.push_back(t);
		} else
		{
			word.push_back(w);
			tag.push_back(option_pretag);

		}
		if (ret.size() > 0) ret += " ";
		ret += w;
	}

	// now look for continuation tags...

	for (int i = 0; i < word.size(); i++)
	{
		int j = tag[i].size() - 1;
		if (j >= 0 && tag[i][j] == '+' && i < tag.size() - 1)
		{
			word[i] = word[i] + " " + word[i + 1];
			tag[i] = tag[i + 1];
			word.erase(word.begin() + i + 1, word.begin() + i + 2);
			tag.erase(tag.begin() + i + 1, tag.begin() + i + 2);
			i--;
		}
	}

	return ret;
}


static int count_words(const char *s)
{
	int     i;

	i = 1;
	for (; *s; ++s)
	{
		if (*s == ' ') ++i;
	}
	return i;
}

static void print_word(const char *s, int i)
{
	for (; i > 0 && *s; ++s) { if (*s == ' ') --i; }
	while (*s && *s != ' ') { printf("%c", *s); ++s; }
}

void MPtok::print(int how)
{
	int     i, j, w;

	if (how != 0 && how != 2)
	{
		printf("print(%d) not defined\n", how);
		return;
	}

	for (i = 0; i < word.size(); ++i)
	{
		// Get the words from an idiom

		for (w = 0; w < count_words(word[i].c_str()); ++w)
		{
			if (how == 2 && i + w > 0) printf(" ");

			print_word(word[i].c_str(), w);

			if (how == 0)
			{
				printf(" tagged %s", tag[i].c_str());
				if (w < count_words(word[i].c_str()) - 1) printf("+");
				printf("\n");
			} else if (how == 2)
			{
				printf("%s%s", "_", tag[i].c_str());
				if (w < count_words(word[i].c_str()) - 1) printf("+");
			}
		}
	}
	if (how == 2)
		printf("\n");
}

void MPtok::merge_words(int s, int n)
{
	string	tmp = word[s];

	for (int i = s + 1; i < s + n; i++)
	{
		tmp += " ";
		tmp += word[i];
	}

	// printf("merging words : '%s' n = %d\n", tmp.c_str(), n);

	for (int k = s; k + n < word.size(); k++)
	{
		word[k+1] = word[k+n];
		tag[k+1] = tag[k+n];
	}

	// Fixup the remaining array

	word.resize(word.size() - n + 1); 
	tag.resize(word.size());

	word[s] = tmp;
}

void MPtok::split_words()
{
	for (int i = 0; i < word.size(); i++)
	{
		int found = word[i].find(' ');

		if (found != string::npos)
		{
			string tmp1(word[i], 0, found);
			string tmp2(word[i], found + 1, string::npos);

			// Move all the words and tags down

			word.resize(word.size() + 1);
			tag.resize(tag.size() + 1);

			for (int j = word.size() - 1; j > i; j--)
			{
				word[j] = word[j - 1];
				tag[j] = tag[j - 1];
			}

			word[i] = tmp1;
			tag[i] = tag[i+1];
			tag[i] += "+";

			word[i+1] = tmp2;
		}
	}
}

// Callable functions to set internal options

void MPtok::set_segment(int i) { option_segment = i; }
void MPtok::set_hyphen(int i) { option_hyphen = i; }
void MPtok::set_comma(int i) { option_comma = i; }
void MPtok::set_pretag(char *a) { option_pretag = a; }
void MPtok::set_pretok(int i) { option_pretok = i; }
void MPtok::set_new(int i) { option_new = i; }
void MPtok::set_doteos(int i) { option_doteos = i; }
