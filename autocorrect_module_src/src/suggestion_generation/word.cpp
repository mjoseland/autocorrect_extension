#include "word.h"

/* PUBLIC */

Word::Word(string word, uint32_t count) : word_(word), word_len_(word_.size()), count_(count) {
    j_ = 0;
    current_column_ = false;
	min_ed_ = 0;

    ed_matrix_ = vector<vector<uint8_t>>(word_len_, vector<uint8_t>(2));
}

string Word::getWord() {
    return word_;
}

size_t Word::getLength() {
    return word_len_;
}

uint8_t Word::getMinEditDistance() {
    return min_ed_;
}

uint32_t Word::getCount() {
    return count_;
}

void Word::addCompareChar(char c) {
    uint8_t edit_distance;
    min_ed_ = UINT8_MAX;

    // find ED for all elements in the column
    for (size_t i = 0; i < word_len_; i++) {
        edit_distance = get_element_ed(c, i);

        ed_matrix_[i][current_column_] = edit_distance;
        min_ed_ = MIN(min_ed_, edit_distance);
    }

    // increment the column index and update current_column_
    j_++;
    current_column_ = j_ & 1;

	previous_char_ = c;
}


void Word::resetCompareWord() {
    j_ = 0;
    current_column_ = false;
	min_ed_ = 0;
}

/* Return true if this word is a less suitable match than the word it is being compared to */
bool Word::operator<(Word other_word) {
    return compare_suitability(other_word) <= -1;
}

/* Return true if this word's suitability is equal to or less than compareword_ */
bool Word::operator<=(Word other_word) {
    return compare_suitability(other_word) <= 0;
}

/* Return true if this word is a more suitable match than the other word */
bool Word::operator>(Word other_word) {
    return compare_suitability(other_word) >= 1;
}

/* Return true if this word's suitability is equal to or more than the other word */
bool Word::operator>=(Word other_word) {
    return compare_suitability(other_word) >= 0;
}

/* Return true if this word has an equal match suitability to the other word */
bool Word::operator==(Word other_word) {
    return compare_suitability(other_word) == 0;
}


/* -------------------- private functions -------------------- */


uint8_t Word::get_element_ed(char c, size_t i) {
	// deletion and insertion refer to doing that operation to the compare word (ie. not word_)

	// get the substitution cost
    uint8_t substitution_cost = 0;
    if (c != word_[i]) {
        substitution_cost = EditCostArray::subCost(c, word_[i]);
    }

	// get the insert/delete cost
	uint8_t ins_del_cost = EditCostArray::insDelCost(previous_char_, c, word_[i]);

    // if we are adding the first char of a new compare word
    if (j_ == 0) {
        // if element is the first row of the matrix: return the substitution cost * penalty
        if (i == 0) {
            return substitution_cost * FIRST_LETTER_PENALTY;
        }

        // if not first row: return the value of the element above + deletion cost * penalty
        return ed_matrix_[i - 1][0] + (ins_del_cost * FIRST_LETTER_PENALTY);
    }


    // if element is the first row and not the first column, return left number + insertion cost
    if (i == 0) {
        return ed_matrix_[i][!current_column_] + ins_del_cost;
    }

    // we aren't in the first row or column: return min of insertion, deletion, or substitution
    return min_of_three(ed_matrix_[i][!current_column_] + ins_del_cost,		// insertion
			ed_matrix_[i - 1][current_column_] + ins_del_cost,				// deletion
            ed_matrix_[i - 1][!current_column_] + substitution_cost);		// substitution
}

uint8_t Word::min_of_three(uint8_t a, uint8_t b, uint8_t c) {
    return MIN(MIN(a, b), c);
}

/* Compares suitability as a suggestion with a different word
 *  if this word more suitable than the other word return a positive integer
 *  if suitability is equal return 0
 */
int Word::compare_suitability(Word other_word) {
    auto other_word_ed = other_word.getMinEditDistance();

    if (min_ed_ < other_word_ed) {
        return 1;
    }

    if (min_ed_ > other_word_ed) {
        return -1;
    }

    auto other_word_count = other_word.getCount();

    if (count_ > other_word_count) {
        return 1;
    }

    if (count_ < other_word_count) {
        return -1;
    }

    return 0;
}
