//
// Created by Malcolm Joseland on 08/11/2016.
//

#include "ed_matrix.h"


EdMatrix::EdMatrix(uint8_t size) : size_(size) {
	matrix_ = new COST[size_ * 2];

	if (matrix_ == nullptr) {
		cerr << "EdMatrix::EdMatrix(): matrix_ is nullptr, allocation failed" << endl;
	}
}


EdMatrix::~EdMatrix() { }

const uint8_t EdMatrix::size() const {
	return size_;
}

COST &EdMatrix::get(const uint8_t i, const bool current_column) {
	return matrix_[i + current_column * size_];
}
