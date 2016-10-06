#include "edit_cost_array.h"

vector<COST> EditCostArray::substitution_costs_ = vector<COST>(676, NP);

EditCostArray::SubstitutionCostInitialiser::SubstitutionCostInitialiser() {
	// initialise vector with all positions storing the non-proximal cost
	//EditCostArray::substitution_costs_ = vector<COST>(676, NP);

	// replace cost at all letter self-matches with 0
	for (size_t i = 0; i < 26; i++) {
		EditCostArray::substitution_costs_[i * 26 + i] = 0;
	}

	// change costs for all keys proximal to each letter 
	// 		ie. a is proximal to the letters qwsxz on a keyboard
	vector<string> proximal_keys = {"qwsxz", 	// a
									"vghn",
									"xdfv",		// c
									"serfcx",
									"wrfds",	// e
									"drtgvc",
									"ftyhbv",	// g
									"gyujnb",
									"uolkj",	// i
									"huikmn",	
									"jiolm",	// k
									"kop",
									"njk", 		// m
									"bhjm",
									"iplk",		// o
									"ol",
									"wsa",		// q
									"etgfd",
									"awedxz",	// s
									"ryhgf",
									"yikjh",	// u
									"cfgb",
									"qedsa",	// w
									"zsdc",
									"tujhg",	// y
									"asx"};

	for (size_t i = 0; i < 26; i++) {
		for (char c: proximal_keys[i]) {
			EditCostArray::substitution_costs_[i * 26 + (c - 'a')] = KP;
		}
	}
}



COST EditCostArray::insDelCost(char previous_char, char new_char, char word_char) {
	COST cost = 0;

	if (new_char == '\'' || word_char == '\'' || new_char == '-' || word_char == '-') {
		// inserting or deleting a non-letter char
		cost = IDN;
	} else if (previous_char == '*' || new_char == previous_char) {
		// inserting or deleting a char that is the same as the previous char
		cost = IDS;
	} else {
		// default return value for insert/delete
		cost = IDD;
	}

	return cost;
}

COST EditCostArray::subCost(char new_char, char word_char) {
	if (new_char == '\'' || word_char == '\'' || new_char == '-' || word_char == '-') {
		return NC;
	}

	return substitution_costs_[(new_char - 'a') * 26 + (word_char - 'a')];
}

/* -------------------- private functions -------------------- */
