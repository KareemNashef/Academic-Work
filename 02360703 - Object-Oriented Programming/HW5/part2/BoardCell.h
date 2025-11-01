#ifndef BOARDCELL_H
#define BOARDCELL_H

#include "CellType.h"
#include "Direction.h"

template <CellType InType, Direction InDirection, int InLength>
struct BoardCell {

	constexpr static CellType type = InType;
	constexpr static Direction direction = InDirection;
	constexpr static int length = InLength;

};


#endif
