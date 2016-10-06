//
// Created by Malcolm Joseland on 3/08/2016.
//

#include "suggestion_generator.h"




SuggestionGenerator::SuggestionGenerator() {
	previous_word_ = '-';
}

string SuggestionGenerator::getSuggestions(string modified_word, string previous_word) {
	cout << "SuggestionGenerator::getSuggestions(): Retrieving suggestions for modified_word, "
		 "previous_word: " << modified_word << ", " << previous_word << endl;
	auto n = modified_word.size();

	// check for the case that word_list.addCompareChar() can't be used. if the only 
	// difference between modified_word and previous_word and their last_ equivalents is 
	// that modified word has a new char appended then the else branch should be executed
	if (previous_word_ != previous_word || 
			modified_word.size() != last_modified_word_.size() + 1 ||
			modified_word.substr(0, n - 1) != last_modified_word_) {

		cout << "SuggestionGenerator::getSuggestions(): Beginning comparison for new word\n";

		// tell the dictionary to reset its compare word
		dictionary_.resetCompareWord();

		// add each compare char to the dictionary
		for (auto c: modified_word) {
			dictionary_.addCompareChar(c);
		}
	} else {
		cout << "SuggestionGenerator::getSuggestions(): Adding only one compare char\n";

		dictionary_.addCompareChar(modified_word[n - 1]);
	}

	auto closest_words = dictionary_.getClosestWords();

	// the string of suggestions to be returned
	string suggestions_str;

	for (auto suggestion: closest_words) {
		suggestions_str += suggestion + ' ';
	}

	// remove the space at the end
	if (suggestions_str[suggestions_str.size() - 1] == ' ') {
		suggestions_str.pop_back();
	}

	previous_word_ = previous_word;
	last_modified_word_ = modified_word;

	return suggestions_str;
}

void SuggestionGenerator::useWords(string words) {
	stringstream file_ss;
    file_ss.str(words);

    string line;

	//size_t tab_indexes[3];
	size_t added_word_count = 0;

	string word_str;
	string word_pos_str;
	uint32_t word_count;
	float word_frequency;


    while (getline(file_ss, line, '\n')) {
		stringstream line_ss(line);

		line_ss >> word_str >> word_pos_str >> word_count >> word_frequency;

		dictionary_.addWord(word_str, word_pos_str, word_count, word_frequency);

		added_word_count++;
    }

	dictionary_.finalise();

	cout << "SuggestionGenerator::createWordList(): Added " << added_word_count << 
		" words to dictionary\n";
}


/* -------------------- private functions -------------------- */
