//
// Created by Malcolm Joseland on 3/08/2016.
//

#include "suggestion_generator.h"




SuggestionGenerator::SuggestionGenerator() {
	last_previous_word_ = '-';
}

string SuggestionGenerator::getSuggestions(string modified_word, string previous_word) {
	cout << "SuggestionGenerator::getSuggestions(): Retrieving suggestions for modified_word, "
		 "previous_word: " << modified_word << ", " << previous_word << endl;
	auto n = modified_word.size();

	// check for the case that word_list.addCompareChar() can't be used. if the only 
	// difference between modified_word and previous_word and their last_ equivalents is 
	// that modified word has a new char appended then the else branch should be executed
	if (last_previous_word_ != previous_word || 
			modified_word.size() != last_modified_word_.size() + 1 ||
			modified_word.substr(0, n - 1) != last_modified_word_) {

		word_list_.resetCompareWord();

		for (auto c: modified_word) {
			word_list_.addCompareChar(c);
		}
	} else {
		cout << "SuggestionGenerator::getSuggestions(): Adding only one compare char\n";
		word_list_.addCompareChar(modified_word[n - 1]);
	}

	auto closest_words = word_list_.getClosestWords();

	// the string of suggestions to be returned
	string suggestions_str;

	for (auto suggestion: closest_words) {
		suggestions_str += suggestion + ' ';
	}

	// remove the space at the end
	if (suggestions_str[suggestions_str.size() - 1] == ' ') {
		suggestions_str.pop_back();
	}

	last_previous_word_ = previous_word;
	last_modified_word_ = modified_word;

	return suggestions_str;
}

void SuggestionGenerator::useWords(string words) {
	stringstream ss;
    ss.str(words);
    string line;

	size_t tab_i;
	size_t word_count = 0;

    while (getline(ss, line, '\n')) {
		tab_i = line.find('\t');
		word_list_.addWord(line.substr(0, tab_i), stoul(line.substr(tab_i + 1)));
		word_count++;
    }

	cout << "SuggestionGenerator::createWordList(): Added " << word_count << 
		" words to wordList\n";
}


/* -------------------- private functions -------------------- */

