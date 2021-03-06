//
// Created by Malcolm Joseland on 3/08/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_SUGGESTIONGENERATOR_H
#define GLOBAL_AUTOCOMPLETE_SUGGESTIONGENERATOR_H

#include <iostream>
//#include <fstream>
#include <sstream>

#include "dictionary.h"

// line format for file: [word][tab][word_frequency_count][newline]
#define WORD_FILE_DIR "../resources/top_50000_words.txt"

class AutocorrectInstance;

using namespace std;

class SuggestionGenerator {
public:
	SuggestionGenerator();

	// finds and returns the suggestions for modified_word
	// return fommat: [word1] [word2] [word3]...
	// edge cases:
	// 		suggestion same as modified_word: return "-"
	string getSuggestions(string modified_word, string previous_word);

	// TODO: fix all the code related to this
	// part of the temp solution to get the words from the text file
	void useWords(const string &words);

	// returns true if the suggestion generator is ready to generate suggestions
	bool ready();

private:
	// the WordList object used to generate suggestions
	Dictionary dictionary_;

	// the previous word (modified_word in getSuggestions()) suggestions were provided for
	string last_modified_word_;

	// the previous word that was paired with last_modified_word_
	string previous_word_;

	//bool finalised_ = false;
};


#endif //GLOBAL_AUTOCOMPLETE_SUGGESTIONGENERATOR_H
