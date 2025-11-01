#ifndef DIRECTION_H
#define DIRECTION_H

enum Direction
{
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};


// Helpful structs
template <Direction InDirection>
struct IsVertical {
	static const bool value = (InDirection == UP || InDirection == DOWN);
};

template <Direction InDirection>
struct IsHorizontal {
	static const bool value = (InDirection == LEFT || InDirection == RIGHT);
};

// Check if two directions are compatible
template <Direction InDirection1, Direction InDirection2>
struct DirectionMismatch {
	static const bool value = (IsVertical<InDirection1>::value && IsVertical<InDirection2>::value) ||
							  (IsHorizontal<InDirection2>::value && IsHorizontal<InDirection1>::value);
};

#endif
