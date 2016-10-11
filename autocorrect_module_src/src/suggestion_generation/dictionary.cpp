#include "dictionary.h"

#include <iostream>	// TODO remove


Dictionary::Dictionary() {
	constructPspeechMap();
	constructPspeechOrderVector();
	correct_pspeech_vector_ = pspeech_order_vector_[UNCL_PSPEECH_INDEX];

	words_ = vector<vector<Word>>(PSPEECH_COUNT);
}

// TODO: set words_ size in constructor or some other solution to manage words_'s size 
void Dictionary::addWord(string word_str, string pos_str, int count, float frequency) {
	if (finalised_) {
		cerr << "Dictionary::addWord(): Blocked attempt to add word to finalised dictionary"
			<< endl;
		return;
	}

	// find the index of the part of speech string
	auto pspeech_it = pspeech_map_.find(pos_str);

	// handle pos_str not recognised
	if (pspeech_it == pspeech_map_.end()) {
		cerr << "Dictionary::addWord(): Unclassified PoS: " << word_str << ' ' << pos_str 
			<< endl;
		return;
	}

	// add the word to pos index mapping
	word_pspeech_map_.emplace(word_str, pspeech_it->second);
	
	// add the word to the correct pos_list
    words_[pspeech_it->second].push_back(Word(word_str, count, frequency));
}

void Dictionary::finalise() {
	// shrink the capacity of all pos vectors in words_
	for (auto pos_vector: words_) {
		pos_vector.shrink_to_fit();
	}

	finalised_ = true;
}

void Dictionary::addCompareChar(char c, string &previous_word) {
	if (!finalised_) {
		cerr << "Dictionary::addCompareChar(): Blocked adding char to unfinalised dictionary"
			<< endl;
		return;
	}

	if (previous_word_ != previous_word) {
		previous_word_ = previous_word;
		updatePspeechPenalties();
	}
		
    closest_word_indexes_.clear();

	Word *min_closest_word;// = nullptr;

    size_t n;			// the number of words in a part of speech vector
	Word *word_ptr; 

    for (size_t i = 0; i < PSPEECH_COUNT; i++) {
		n = words_[i].size();

		for (size_t j = 0; j < n; j++) {
			// the current word the compare char is being added to
			// should be safe because words_ won't be reallocated in this method
			// TODO: maybe fix for good practice
			word_ptr = &words_[i][j];

			if (word_ptr->getCost() > MAX_ED) {
				continue;
			}

			word_ptr->addCompareChar(c);

			// if the word should be added as a new closest word, do so 
			if (word_ptr->getCost() <= MAX_ED && !in_closest_words(word_ptr->getWord()) &&
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


void Dictionary::constructPspeechMap() {
	pspeech_map_.emplace(make_pair("Verb", 0));
	pspeech_map_.emplace(make_pair("VerbTPP", 1));
	pspeech_map_.emplace(make_pair("VerbPa", 2));
	pspeech_map_.emplace(make_pair("VerbGe", 3));
	pspeech_map_.emplace(make_pair("NoC", 4));
	pspeech_map_.emplace(make_pair("NoCPl", 5));
	pspeech_map_.emplace(make_pair("NoCPo", 6));
	pspeech_map_.emplace(make_pair("NoP", 7));
	pspeech_map_.emplace(make_pair("NoPPl", 8));
	pspeech_map_.emplace(make_pair("NoPPo", 9));
	pspeech_map_.emplace(make_pair("Adv", 10));
	pspeech_map_.emplace(make_pair("Adj", 11));
	pspeech_map_.emplace(make_pair("VMod", 12));
	pspeech_map_.emplace(make_pair("DetP", 13));
	pspeech_map_.emplace(make_pair("Prep", 14));
	pspeech_map_.emplace(make_pair("Pron", 15));
	pspeech_map_.emplace(make_pair("Conj", 16));
	pspeech_map_.emplace(make_pair("Int", 17));
	pspeech_map_.emplace(make_pair("Fore", 18));
	pspeech_map_.emplace(make_pair("Det", 19));
	pspeech_map_.emplace(make_pair("Num", 20));
	pspeech_map_.emplace(make_pair("Inf", 21));
	pspeech_map_.emplace(make_pair("Neg", 22));
	pspeech_map_.emplace(make_pair("Uncl", UNCL_PSPEECH_INDEX));
}

// TODO: review 
// 		could -> problem, problems are first suggestion
//		my -> home doesn't suggest home
void Dictionary::constructPspeechOrderVector() {
	pspeech_order_vector_ = 
		vector<vector<bool>>(PSPEECH_COUNT, vector<bool>(PSPEECH_COUNT, false));

	// all elements in this vector represent a correct pspeech order
	// ie NoC at index 0 indicates that common nouns can follow verbs
	vector<vector<string>> pspeech_str_orders(PSPEECH_COUNT);

	// Verb
	pspeech_str_orders[pspeech_map_["Verb"]] = {"NoCPl", "NoPPl", "Adv", "Adj",
			"Pron", "Prep", "Conj", "Det", "Num", "Inf", "Neg"};

	// VerbTPP
	pspeech_str_orders[pspeech_map_["VerbTPP"]] = {"NoC", "NoP", "Adv", "Adj", "DetP", "Prep",
		"Pron", "Conj", "Int", "Det", "Num", "Inf", "Neg"};

	// VerbPa
	pspeech_str_orders[pspeech_map_["VerbPa"]] = {"NoC", "NoCPl", "NoCPo", "NoP", "NoPPl", 
		"NoPPo", "Adv", "Adj", "DetP", "Prep", "Pron", "Conj", "Int", "Det", "Num", "Inf", 
		"Neg"};

	// VerbGe
	pspeech_str_orders[pspeech_map_["VerbGe"]] = {"NoC", "NoCPl", "NoCPo", "NoP", "NoPPl", 
		"NoPPo", "Adv", "DetP", "Prep", "Conj", "Det", "Num", "Neg"};

	// NoC
	pspeech_str_orders[pspeech_map_["NoC"]] = {"VerbTPP", "VerbPa", "VerbGe", "NoC", "NoCPl", 
		"NoP", "NoPPl", "VMod", "Adv", "Adj", "DetP", "Prep", "Pron", "Conj", "Det", "Num", 
		"Neg"};

	// NoCPl
	pspeech_str_orders[pspeech_map_["NoCPl"]] = {"VerbPa", "VerbGe", "NoCPl", "NoP", "Adv", 
			"Adj", "VMod", "DetP", "Prep", "Pron", "Conj", "Det", "Num", "Inf", "Neg"};

	// NoCPo
	pspeech_str_orders[pspeech_map_["NoCPo"]] = {"VerbTPP", "VerbPa", "NoC", "NoCPl", "NoP", 
		"NoPPl", "Adj", "DetP", "Det", "Num", "Inf", "Neg"};

	// NoP
	pspeech_str_orders[pspeech_map_["NoP"]] = {"VerbTPP", "VerbPa", "VerbGe", "NoC", "NoCPl", 
		"NoP", "NoPPl", "VMod", "Adv", "Adj", "DetP", "Prep", "Pron", "Conj", "Det", "Num", 
		"Inf", "Neg"};

	// NoPPl
	pspeech_str_orders[pspeech_map_["NoPPl"]] = {"VerbPa", "VerbGe", "NoCPl", "NoP", "Adv", 
			"Adj", "VMod", "DetP", "Prep", "Pron", "Conj", "Det", "Num", "Inf", "Neg"};

	// NoPPo
	pspeech_str_orders[pspeech_map_["NoPPo"]] = {"VerbTPP", "VerbPa", "NoC", "NoCPl", "NoP", 
		"NoPPl", "Adj", "DetP", "Det", "Num", "Inf", "Neg"};

	// Adv
	pspeech_str_orders[pspeech_map_["Adv"]] = {"Verb", "VerbTPP", "VerbPa", "VerbGe", "VMod", 
		"Conj", "Det", "Num", "Inf", "Neg"};

	// Adj
	pspeech_str_orders[pspeech_map_["Adj"]] = {"NoC", "NoCPl", "NoCPo", "NoP", 
		"NoPPl", "NoPPo", "Adj", "DetP", "Det", "Conj", "Neg"};

	// VMod
	pspeech_str_orders[pspeech_map_["VMod"]] = {"Verb", "NoC", "NoCPl", "NoCPo", "NoP", 
		"NoPPl", "NoPPo", "Adj", "Pron", "Num", "Neg"};

	// DetP
	pspeech_str_orders[pspeech_map_["DetP"]] = {"VerbGe", "NoC", "NoCPl", "NoCPo", "NoP", 
		"NoPPl", "NoPPo", "Adj", "Prep", "Det", "Inf", "Neg"};

	// Prep
	pspeech_str_orders[pspeech_map_["Prep"]] = {"VerbGe", "NoC", "NoCPl", "NoCPo", "NoP", 
		"NoPPl", "NoPPo", "Adv", "DetP", "Num", "Neg"};

	// Pron
	pspeech_str_orders[pspeech_map_["Pron"]] = {"Verb", "VerbPa", "VerbGe", "NoC", "NoCPl", 
		"NoP", "NoPPl", "VMod", "Adv", "Adj", "DetP", "Prep", "Pron", "Conj", "Num", 
		"Inf", "Neg"};

	// Conj
	pspeech_str_orders[pspeech_map_["Conj"]] = {"Verb", "VerbTPP", "VerbPa", "VerbGe", "NoC", 
		"NoCPl", "NoCPo", "NoP", "NoPPl", "NoCPo", "Adv", "Adj", "VMod", "DetP", "Prep", 
		"Pron", "Num", "Inf", "Neg"};

	// Int
	//pspeech_str_orders[pspeech_map_["Int"]];

	// Det
	pspeech_str_orders[pspeech_map_["Det"]] = {"NoC", "NoCPl", "NoCPo", "NoP", "NoPPl", 
		"NoCPo", "Adj", "Det", "Num"};

	// Num
	pspeech_str_orders[pspeech_map_["Num"]] = {"NoCPl", "NoPPl", "Adj", 
			"VMod", "DetP", "Det", "Num", "Inf", "Neg"};

	// Inf
	pspeech_str_orders[pspeech_map_["Inf"]] = {"NoC", "NoCPl", "NoCPo", "NoP", "NoPPl", 
		"NoCPo", "Adv", "Adj", "VMod", "DetP", "Prep", 
		"Pron", "Num", "Inf", "Neg"};

	// Neg
	pspeech_str_orders[pspeech_map_["Neg"]] = {"NoC", "NoCPl", "NoCPo", "NoP", "NoPPl", 
		"NoCPo", "Adv", "Adj", "VMod", "DetP", "Prep", 
		"Pron", "Det", "Num", "Inf", "Neg"};



	// set the correct indexes in the 2D bool vector pspeech_order_vector_ to true
	for (size_t i = 0; i < pspeech_str_orders.size(); i++) {
		auto &pspeech_str_order_vector = pspeech_str_orders[i];

		for (string &correct_following_pspeech_str: pspeech_str_order_vector) {
			pspeech_order_vector_[i][pspeech_map_[correct_following_pspeech_str]] = true;
		}

		// no penalty for words following unclassified/nonexistent words
		pspeech_order_vector_[i][UNCL_PSPEECH_INDEX] = true;
	}
}


void Dictionary::updatePspeechPenalties() {
	vector<bool> new_correct_pspeech_vector(PSPEECH_COUNT, false);

	vector<size_t> possible_prevword_pspeeches;

	auto range = word_pspeech_map_.equal_range(previous_word_);

	cout << "Dictionary::updatePspeechPenalties(): \"" << previous_word_ <<
		"\" pspeech list: ";

	for (auto it = range.first; it != range.second; it++) {
		auto &word_pspeech_order = pspeech_order_vector_[it->second];
		cout << it->second << ' ';

		for (size_t i = 0; i < PSPEECH_COUNT; i++) {
			if (word_pspeech_order[i]) {
				new_correct_pspeech_vector[i] = true;
			}
		}
	}

	cout << endl;

	for (size_t i = 0; i < PSPEECH_COUNT; i++) {
		// if there is a mismatch in the new and old correct pspeech order, update the
		// correctness of the part of speech of all required words
		if (correct_pspeech_vector_[i] ^ new_correct_pspeech_vector[i]) {
			for (Word &word: words_[i]) {
				word.setCorrectPspeech(new_correct_pspeech_vector[i]);
			}

			correct_pspeech_vector_[i] = new_correct_pspeech_vector[i];
		}
	}
}


uint8_t Dictionary::get_pspeech(string word) {
	cerr << word_pspeech_map_.size() << endl; // TODO remove

	auto map_entry_it = word_pspeech_map_.find(word);
	size_t pspeech;

	if (map_entry_it == word_pspeech_map_.end()) {
		pspeech = UNCL_PSPEECH_INDEX;
	} else {
		pspeech = map_entry_it->second;
	}

	cerr << "Dictionary::get_pspeech(): [word] [pspeech]: " << word << ' ' << 
		pspeech << endl;

	return pspeech;
}

bool Dictionary::in_closest_words(string word_str) {
	for (auto close_word_index: closest_word_indexes_) {
		if (words_[close_word_index.first][close_word_index.second].getWord() == word_str) {
			return true;
		}
	}
	
	return false;
}
