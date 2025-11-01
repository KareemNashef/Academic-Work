#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "List.h"

template <typename InBoard>
struct GameBoard {

	typedef InBoard board;
	constexpr static int width = board::head::size;		// Row size
	constexpr static int height = board::size;			// Column size

};


#endif
