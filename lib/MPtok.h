#ifndef _MPTOK_H
#define _MPTOK_H

#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

#define MPTOK_VERSION 11			// The latest version

// Maximum number of words in a sentence

#define MAX_WORDS 10000

enum { ABB_ABB, ABB_EOS, ABB_NUM };
#define MAX_ABB 100

/*! \brief A class to perform tokenization.
 *
 * The MPtag class can be used to perform tokenization and segmentation
 * of strings into tokens or sentences. It is inherited and used by MPtag
 * so if the user is only interested in tagging, this class does not
 * need to be referenced.
 */

class MPtok
{
public:
	/// \brief A MPtok object, giving the install directory \p idir where data files can be found
	MPtok(string idir = "", const string& cnam = "");
	~MPtok();

	void init();				// Initialize (call only once)
	void init(const string& idir) { option_dir = idir; init(); } // Initialize using specified install directory

	string option_pretag;			// The tag to use on tokens
	int option_segment;			// Segment into sentences
	int option_hyphen;			// Hyphens are separate tokens
	int option_comma;			// Commas are always tokenized
	int option_pretok;			// The text is pre-tokenized
	int option_new;				// Use new algorithms, used in development only
	int option_doteos;			// If " . " occurs, it's an end EOS (new >= 5)

	void set_segment(int i);		///< \brief Sentences are broken up during tokenization (default 1)
	void set_token(int i);			///< \brief Break tokens apart with white space (default 1)
	void set_hyphen(int i);			///< \brief Hyphens are separate tokens (default 0)
	void set_comma(int i);			///< \brief Commas are separate tokens (default 1)
	void set_pretag(char *a);		///< \brief Use this tag on all tokens (default empty string)
	void set_pretok(int i);			///< \brief Assume string is already tokenized using spaces (default 0)
	void set_new(int i);			///< \brief Use a previous algorithm (defaults to most recent)
	void set_doteos(int i);			///< \brief Ignore abbreviations, and always assume a period ends a sentence (default 0)

	void merge_words(int s, int e);		// merge words between s and e (idiom)
	void split_words(void);			// split all merged words

	string tokenize(const string&);		///< \brief Tokenize, save (in \p word), and return space delimited tokens
	string segment(const string&);		///< \brief Segment, save (in \p sent), and return newline delimited sentences

	string save_string(const string&);	// save a buffer
	string tokenize_nosave(const string&);	// tokenize without saving
	string tokenize(const string&,int);	// do tokenization with or without inserting spaces between them

	void print(int);			///< \brief Print tokens/tags with given verbosity

	vector<string> word;			///< \brief Vector of words (tokens) of most recently tagged (or tokenized) text
	vector<string> tag;			///< \brief Vector of tags of most recently tagged (or tokenized) text
	vector<string> sent;			///< \brief Vector of sentences of most recently sentence-segmented text

	char	*text;				// Input text arg
	int	text_len;			// It's length
	int	*tokflag;			// token flags
	int	*endflag;			// end-sentence flags

	string option_cnam;			// A suffix, for opening variant support files
	string option_dir;			// Directory to find things

protected:

	void set_tokflag();
	void set_endflag();
	void set_endflag_01();
	int size_buff();

	void init_pair(const string& file_name);	// read a file of common pairs
	void init_abbr(const string& file_name);	// read a file of abbreviations

	void tok_0();
	void tok_1();
	void tok_2();
	void tok_3();
	void tok_5_6_7();
	void tok_8_9();
	void tok_10();
	void tok_11();
	void tok_12();
	void tok_13();
	void tok_14();
	void tok_15();
	void tok_15_1();
	void tok_16();
	void tok_16_1();
	void tok_17();
	void tok_20();
	void tok_20_1();
	void tok_20_2();
	void tok_21();
	void tok_21a();
	void tok_22();
	void tok_23();
	void tok_24();
	void tok_25();
	void tok_26();
	void tok_27();
	void tok_28();
	void tok_29();
	void tok_29a();
	void tok_30();
	void tok_31();
	void tok_32();
	void tok_33();
	int complex_check();
	void map_escapes();
	void tok_un();

	void append_token(string&, int&, char*, int);
	string token_string();

	set<string> common_pair;
	map<string,int> common_abbr;

private:
	int option_token;			// Output tokenized text (only use internally)
	int tok_initialized;			// is it inited?
};

#endif

