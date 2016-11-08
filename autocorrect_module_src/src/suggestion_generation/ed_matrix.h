//
// Created by Malcolm Joseland on 08/11/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_EDMATRIX_H
#define GLOBAL_AUTOCOMPLETE_EDMATRIX_H

#include <iostream>

#ifndef COST
#define COST uint8_t
#endif

using namespace std;

class EdMatrix {
public:
	EdMatrix(uint8_t size);
	~EdMatrix();

	const uint8_t size() const;

	COST &get(const uint8_t i, const bool current_column);
	

private:
	const uint8_t size_;

	COST *matrix_;
};

#endif //GLOBAL_AUTOCOMPLETE_EDMATRIX_H

