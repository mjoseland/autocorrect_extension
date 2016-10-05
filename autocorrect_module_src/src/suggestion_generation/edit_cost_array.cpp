#include "edit_cost_array.h"

static const COST substitution_costs[] = 
			// a   b   c   d   e   f   g   h   i   j   k   l   m   n   o   p   q   r   s   t   u   v   w   x   y   z
			  {0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, KP, KP, KP, NP, NP, NP, KP, NP, NP, // a
			   NP, 0,  NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // b
			   NP, NP, 0,  NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // c
			   NP, NP, NP, 0,  NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // d
			   NP, NP, NP, NP, 0,  NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // e
			   NP, NP, NP, NP, NP, 0,  NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // f
			   NP, NP, NP, NP, NP, NP, 0,  KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // g
			   NP, NP, NP, NP, NP, NP, NP, 0,  KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // h
			   NP, NP, NP, NP, NP, NP, NP, KP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // i
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // j
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // k
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // l
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // m
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // n
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // o
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, // p
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, NP, // q
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, NP, // r
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, NP, // s
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, NP, // t
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, NP, // u
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, NP, // v
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, NP, // w
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, NP, // x
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0,  NP, // y
			   NP, NP, NP, NP, NP, NP, NP, KP, KP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, NP, 0}; // z

const COST *EditCostArray::substitution_costs_ = substitution_costs;



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
