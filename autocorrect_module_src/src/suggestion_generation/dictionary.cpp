#include "dictionary.h"

#include <iostream>
//TODO: remove

/* PUBLIC */

Dictionary::Dictionary() {
	pos_str_index_map_ = getPosIndexMap();
	words_ = vector<vector<Word>>(POS_COUNT);
}

// TODO: set words_ size in constructor or some other solution to manage words_'s size 
void Dictionary::addWord(string word_str, string pos_str, int count, float frequency) {
	if (finalised_) {
		cerr << "Dictionary::addWord(): Blocked attempt to add word to finalised dictionary"
			<< endl;
		return;
	}

	// find the index of the part of speech string
	auto pos_index_it = pos_str_index_map_.find(pos_str);

	// handle pos_str not recognised
	if (pos_index_it == pos_str_index_map_.end()) {
		cerr << "Dictionary::addWord(): Unclassified PoS: " << word_str << ' ' << pos_str 
			<< endl;
		return;
	}

	// add the word to pos index mapping
	word_pos_index_map_.emplace(word_str, pos_index_it->second);
	
	// add the word to the correct pos_list
    words_[pos_index_it->second].push_back(Word(word_str, count, frequency));
}

void Dictionary::finalise() {
	// shrink the capacity of all pos vectors in words_
	for (auto pos_vector: words_) {
		pos_vector.shrink_to_fit();
	}

	pos_str_index_map_.clear();

	finalised_ = true;
}

void Dictionary::addCompareChar(char c) {
	if (!finalised_) {
		cout << "Dictionary::addCompareChar(): Blocked adding char to unfinalised dictionary"
			<< endl;
		return;
	}

	// TODO: special handling for previous words not, to

    closest_word_indexes_.clear();

	Word *min_closest_word;

    size_t n;			// the number of words in a part of speech vector
	Word *word_ptr; 

    for (size_t i = 0; i < POS_COUNT; i++) {
		n = words_[i].size();

		for (size_t j = 0; j < n; j++) {
			// the current word the compare char is being added to
			// should be safe because words_ won't be reallocated in this method
			// TODO: maybe fix for good practice
			word_ptr = &words_[i][j];
			
			if (word_ptr->getWord() == "help") {
				cout << word_ptr->getWord() << " MED: "
					<< (int)word_ptr->getMinEditDistance() << endl;
			}

			if (word_ptr->getMinEditDistance() > MAX_ED) {
				continue;
			}

			word_ptr->addCompareChar(c);

			if (word_ptr->getWord() == "help") {
				cout << word_ptr->getWord() << " MED (after): " 
					<< (int)word_ptr->getMinEditDistance() << endl;
			}

			// if the word should be added as a new closest word, do so 
			if (word_ptr->getMinEditDistance() <= MAX_ED &&
						 (closest_word_indexes_.size() < CLOSEST_WORDS_N ||
						 min_closest_word == nullptr || 		//TODO: remove line
						 *word_ptr > *min_closest_word)) {
				
				insertNewClosestWordIndex(i, j);
				
				auto min_word_i = closest_word_indexes_.back(); 

				min_closest_word = &(words_[min_word_i.first][min_word_i.second]);
			}
		}
    }

	cout << "Dictionary::addCompareChar(): Char \"" << c << "\" added, new words: ";
	auto closest_words = getClosestWords();
	for (auto &word: closest_words) {
		cout << word << ' ';
	}
	cout << endl;
}

vector<string> Dictionary::getClosestWords() {
    vector<string> closest_word_strings;

    for (auto word_i: closest_word_indexes_) {
        closest_word_strings.push_back(words_[word_i.first][word_i.second].getWord());
    }

    return closest_word_strings;
}


void Dictionary::resetCompareWord() {
	for (auto &pos_vector: words_) {
		for (auto &word: pos_vector) {
			word.resetCompareWord();

			if (word.getWord() == "help") {
				cerr << "reset help " << endl;
				cerr << "new MED: " << (int)word.getMinEditDistance() << endl;
			}
		}
	}
}

/* -------------------- private functions -------------------- */

// TODO: make more efficient
void Dictionary::insertNewClosestWordIndex(size_t new_i, size_t new_j) {
    // if we're adding the first new closest word to the vector, do so and return
    if (closest_word_indexes_.empty()) {
        closest_word_indexes_.push_back({new_i, new_j});
        return;
    }

    // insert the new word at the correct position
    auto it = closest_word_indexes_.begin();
    for (; it != closest_word_indexes_.end() && words_[new_i][new_j] < 
			words_[it->first][it->second]; it++);
    closest_word_indexes_.insert(it, {new_i, new_j});

    // if vector of closest word indexes is now too big, remove the last element
    if (closest_word_indexes_.size() > CLOSEST_WORDS_N) {
        closest_word_indexes_.pop_back();
    }
}


unordered_map<string, size_t> Dictionary::getPosIndexMap() {
	unordered_map<string, size_t> pos_indexes;

	pos_indexes.emplace(make_pair("Verb", 0));
	pos_indexes.emplace(make_pair("VerbUs", 1));
	pos_indexes.emplace(make_pair("VerbPa", 2));
	pos_indexes.emplace(make_pair("VerbGe", 3));
	pos_indexes.emplace(make_pair("NoC", 4));
	pos_indexes.emplace(make_pair("NoCPo", 5));
	pos_indexes.emplace(make_pair("NoCPl", 6));
	pos_indexes.emplace(make_pair("NoP", 7));
	pos_indexes.emplace(make_pair("NoPPl", 8));
	pos_indexes.emplace(make_pair("NoPPo", 9));
	pos_indexes.emplace(make_pair("Adv", 10));
	pos_indexes.emplace(make_pair("Adj", 11));
	pos_indexes.emplace(make_pair("VMod", 12));
	pos_indexes.emplace(make_pair("DetP", 13));
	pos_indexes.emplace(make_pair("Prep", 14));
	pos_indexes.emplace(make_pair("Pron", 15));
	pos_indexes.emplace(make_pair("Conj", 16));
	pos_indexes.emplace(make_pair("Int", 17));
	pos_indexes.emplace(make_pair("Fore", 18));
	pos_indexes.emplace(make_pair("Det", 19));
	pos_indexes.emplace(make_pair("Num", 20));
	pos_indexes.emplace(make_pair("Inf", 21));
	pos_indexes.emplace(make_pair("Neg", 22));
	pos_indexes.emplace(make_pair("Uncl", 23));

	return pos_indexes;
}

