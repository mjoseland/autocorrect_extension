#include "word_list.h"

#include <iostream>
//TODO: remove

/* PUBLIC */

WordList::WordList() {
    // nothing here for now
}

vector<string> WordList::getClosestWords() {
    vector<string> closest_word_strings;

    for (auto close_word_i: closest_word_indexes_) {
        closest_word_strings.push_back(words_[close_word_i].getWord());
    }

    return closest_word_strings;
}

// TODO: set words_ size in constructor or some other solution to manage words_'s size
void WordList::addWord(string word, int count) {
    words_.push_back(Word(word, count));
}

void WordList::addCompareChar(char c) {
    closest_word_indexes_.clear();

    auto n = words_.size();

    for (size_t i = 0; i < n; i++) {
        if (words_[i].getMinEditDistance() > MAX_ED) {
            continue;
        }

        words_[i].addCompareChar(c);

        if (words_[i].getMinEditDistance() <= MAX_ED &&
                    (closest_word_indexes_.size() < CLOSEST_WORDS_N ||
                    words_[i] > words_[closest_word_indexes_.back()])) {
            insertNewClosestWordIndex(i);
        }
    }

	cout << "WordList::addCompareChar(): Char \"" << c << "\" added, new words: ";
	auto closest_words = getClosestWords();
	for (auto word: closest_words) {
		cout << word << ' ';
	}
	cout << endl;
}

void WordList::resetCompareWord() {
    auto n = words_.size();

    for (size_t i = 0; i < n; i++) {
        words_[i].resetCompareWord();
    }
}

/* -------------------- private functions -------------------- */

// TODO: make more efficient
void WordList::insertNewClosestWordIndex(size_t new_i) {
    // if we're adding the first new closest word to the vector, do so and return
    if (closest_word_indexes_.empty()) {
        closest_word_indexes_.push_back(new_i);
        return;
    }

    // insert the new word at the correct position
    auto it = closest_word_indexes_.begin();
    for (; it != closest_word_indexes_.end() && words_[new_i] < words_[*it]; it++);
    closest_word_indexes_.insert(it, new_i);

    // if _closestwords_ is now too big, remove the last element
    if (closest_word_indexes_.size() > CLOSEST_WORDS_N) {
        closest_word_indexes_.pop_back();
    }
}
