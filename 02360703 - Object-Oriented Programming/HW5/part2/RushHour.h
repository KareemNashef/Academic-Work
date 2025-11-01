#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include "MoveVehicle.h"

// ========================================================================================
// FindCarLocation - Find the coordinates of the car of the given type in a given board

// Forward declaration
template <typename InBoard, CellType InCar, int CurrentRow = 0, int CurrentCol = 0, bool EndOfBoard = false>
struct FindCarLocation;

// Main implementation
template <typename InBoard, CellType InCar, int CurrentRow, int CurrentCol, bool EndOfBoard>
struct FindCarLocation {

    // Get the current row
    using CurrentRowType = typename GetAtIndex<CurrentRow, typename InBoard::board>::value;

    // Get the current cell
    using CurrentCell = typename GetAtIndex<CurrentCol, CurrentRowType>::value;

    // Check if we've found the car
    static constexpr bool FoundCar = CurrentCell::type == InCar;

    // Check if we're at the end of the row
    static constexpr bool EndOfRow = CurrentCol == CurrentRowType::size - 1;

    // Check if we're at the end of the board
    static constexpr bool ReachedEndOfBoard = EndOfRow && CurrentRow == InBoard::height - 1;

    // Define the next position to search
    using NextPosition = FindCarLocation<InBoard, InCar, EndOfRow ? CurrentRow + 1 : CurrentRow, EndOfRow ? 0 : CurrentCol + 1, ReachedEndOfBoard>;
               
    // Define the result
    static constexpr int row = FoundCar ? CurrentRow : NextPosition::row;
    static constexpr int col = FoundCar ? CurrentCol : NextPosition::col;
};

// Specialization for when we've reached the end of the board
template <typename InBoard, CellType InCar, int CurrentRow, int CurrentCol>
struct FindCarLocation<InBoard, InCar, CurrentRow, CurrentCol, true> {
    static constexpr int row = -1;
    static constexpr int col = -1;
};

// ========================================================================================
// CheckRightPath - Checks the path to the right of the red car

template <typename InBoard, int InRow, int InCol>
struct CheckRightPath {
    static constexpr bool pathClear = ConditionalInteger<GetCell(InRow, InCol)::type == X, true, ConditionalInteger<GetCell(InRow, InCol)::type == EMPTY, CheckRightPath<InBoard, InRow, InCol - 1>::pathClear, false>::value>::value;
};

template <typename InBoard, int InRow>
struct CheckRightPath<InBoard, InRow, 0> {
    static constexpr bool pathClear = GetCell(InRow, 0)::type == X;
};

template <typename InBoard>
struct CheckWin {

    // Get the red car's row
	constexpr static int redRow = FindCarLocation<InBoard, X, 0, 0>::row;
    
	// Check if all cells to the right of the red car are empty
    static constexpr bool result = CheckRightPath<InBoard, redRow, InBoard::width - 1>::pathClear;
};

template <typename InBoard, typename MovesList>
struct CheckSolution {

    // Get the car's coordinates
    static constexpr int carRow = FindCarLocation<InBoard, MovesList::head::type, 0, 0>::row;
    static constexpr int carCol = FindCarLocation<InBoard, MovesList::head::type, 0, 0>::col;

    // Apply move and move on to the next one
    static constexpr bool result = CheckSolution<MoveVehicle<InBoard, carRow, carCol, MovesList::head::direction, MovesList::head::amount>::board, typename MovesList::next>::result;

};

// Base case: Moves list is empty
template <typename InBoard>
struct CheckSolution<InBoard, List<> > {
    static constexpr bool result = CheckWin<InBoard>::result;
};

#endif