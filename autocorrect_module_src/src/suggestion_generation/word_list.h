//
// Created by Malcolm Joseland on 23/07/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_WORDLIST_H
#define GLOBAL_AUTOCOMPLETE_WORDLIST_H

#define CLOSEST_WORDS_N 5   // the max number of words to add to _closest_words
#define MAX_ED 15            // the max ed for a word to be added to _closest_words

#include <vector>
#include <list>

#include "word.h"

using namespace std;

class WordList {
public:
    WordList();

    vector<string> getClosestWords();

    void addWord(string word, int count);
    void addCompareChar(char c);

	// resets all scoring for word suggestions, ie. gets ready to start providing suggestions for
	// a new word
    void resetCompareWord();

private:
    vector<Word> words_;
    list<size_t> closest_word_indexes_;   // descending order of suitability

    void insertNewClosestWordIndex(size_t i);

};


#endif //GLOBAL_AUTOCOMPLETE_WORDLIST_H
