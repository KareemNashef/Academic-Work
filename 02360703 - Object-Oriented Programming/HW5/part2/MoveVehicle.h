#ifndef MOVEVEHICLE_H
#define MOVEVEHICLE_H

#include "BoardCell.h"
#include "GameBoard.h"
#include "Utilities.h"

#define GetCell(row, column) GetAtIndex<column, typename GetAtIndex<row, typename InBoard::board>::value>::value
#define GetCellMove(row, column) GetAtIndex<column, typename GetAtIndex<row, InBoard>::value>::value

#define GetMin(a, b) ((a) < (b) ? (a) : (b))
#define GetMax(a, b) ((a) > (b) ? (a) : (b))

template <CellType InType, Direction InDirection, int InAmount>
struct Move {

	// Check if the cell is empty
	static_assert(InType != EMPTY, "fery sad");

	constexpr static CellType type = InType;
	constexpr static Direction direction = InDirection;
	constexpr static int amount = InAmount;

};

// ========================================================================================
// Vehicle coordinates helpers

// Go over the entire column and find the first and last cell of the vehicle
template <typename InBoard, int InCol, CellType CarType, int CurrentRow = InBoard::height - 1>
struct GetRows {

	static constexpr bool IsMatch = GetCell(CurrentRow, InCol)::type == CarType;
	using NextRow = GetRows<InBoard, InCol, CarType, CurrentRow - 1>;

	static constexpr int first = IsMatch ? CurrentRow : NextRow::first;
	static constexpr int last = IsMatch ? (NextRow::last == -1 ? CurrentRow : NextRow::last) : NextRow::last;

};

// Base case for GetRows
template <typename InBoard, int InCol, CellType CarType>
struct GetRows<InBoard, InCol, CarType, -1> {
	static constexpr int first = -1;
	static constexpr int last = -1;
};

// Go over the entire row and find the first and last cell of the vehicle
template <typename InBoard, int InRow, CellType CarType, int CurrentCol = InBoard::width - 1>
struct GetColumns {

	static constexpr bool IsMatch = GetCell(InRow, CurrentCol)::type == CarType;
	using NextCol = GetColumns<InBoard, InRow, CarType, CurrentCol - 1>;

	static constexpr int first = IsMatch ? CurrentCol : NextCol::first;
	static constexpr int last = IsMatch ? (NextCol::last == -1 ? CurrentCol : NextCol::last) : NextCol::last;

};

// Base case for GetColumns
template <typename InBoard, int InRow, CellType CarType>
struct GetColumns<InBoard, InRow, CarType, -1> {
	static constexpr int first = -1;
	static constexpr int last = -1;
};

template <typename InBoard, int InRow, int InCol, Direction InDirection>
struct GetCoordinates {

	// Get the car's orientation
	static constexpr bool orientation = IsHorizontal<InDirection>::value;		// True: horizontal, False: vertical

	// Getting the coordinates of the edges
	static constexpr int firstEdgeRow = orientation ? InRow : GetRows<InBoard, InCol, GetCell(InRow, InCol)::type>::first;
	static constexpr int secondEdgeRow = orientation ? InRow : GetRows<InBoard, InCol, GetCell(InRow, InCol)::type>::last;
	static constexpr int firstEdgeCol = orientation ? GetColumns<InBoard, InRow, GetCell(InRow, InCol)::type>::first : InCol;
	static constexpr int secondEdgeCol = orientation ? GetColumns<InBoard, InRow, GetCell(InRow, InCol)::type>::last : InCol;

	// Determine front and rear based on direction
	static constexpr int rearRow =
		(InDirection == Direction::UP) ? GetMax(firstEdgeRow, secondEdgeRow) :
		(InDirection == Direction::DOWN) ? GetMin(firstEdgeRow, secondEdgeRow) :
		firstEdgeRow;  // For LEFT and RIGHT, rear row is the same as first edge row

	static constexpr int rearCol =
		(InDirection == Direction::LEFT) ? GetMax(firstEdgeCol, secondEdgeCol) :
		(InDirection == Direction::RIGHT) ? GetMin(firstEdgeCol, secondEdgeCol) :
		firstEdgeCol;  // For UP and DOWN, rear col is the same as first edge col

	static constexpr int frontRow =
		(InDirection == Direction::UP) ? GetMin(firstEdgeRow, secondEdgeRow) :
		(InDirection == Direction::DOWN) ? GetMax(firstEdgeRow, secondEdgeRow) :
		firstEdgeRow;  // For LEFT and RIGHT, front row is the same as first edge row

	static constexpr int frontCol =
		(InDirection == Direction::LEFT) ? GetMin(firstEdgeCol, secondEdgeCol) :
		(InDirection == Direction::RIGHT) ? GetMax(firstEdgeCol, secondEdgeCol) :
		firstEdgeCol;  // For UP and DOWN, front col is the same as first edge col

};

// ========================================================================================
// Move making helpers

// SetCell - Sets the given cell in the board
template <typename InBoard, int InRow, int InCol, CellType InType, Direction InDirection, int InLength>
struct SetCell {


	//typedef typename SetAtIndex<InRow, typename SetAtIndex<InCol, BoardCell<InType, InDirection, InLength>, typename GetAtIndex<InRow, typename InBoard::board>::value>::list, typename InBoard::board>::list NewBoard;

	// Get The row of the needed cell
	//using Row = typename GetAtIndex<InRow, typename InBoard::board>::value;
	typedef typename GetAtIndex<InRow, typename InBoard>::value Row;

	// Update the cell at the given column and row
	typedef typename SetAtIndex<InCol, BoardCell<InType, InDirection, InLength>, Row>::list NewRow;

	// Update the board with the new row
	typedef typename SetAtIndex<InRow, NewRow, typename InBoard>::list NewBoard;

};

// MakeMove - Moves a given vehicle in a given direction
template <typename InBoard, int RearRow, int RearCol, int FrontRow, int FrontCol, Direction InDirection, int InAmount>
struct MakeMove {
	
	// Calculate the next rear/front in the path
	static constexpr int nextRearRow = InDirection == UP ? RearRow - 1 : InDirection == DOWN ? RearRow + 1 : RearRow;
	static constexpr int nextRearCol = InDirection == LEFT ? RearCol - 1 : InDirection == RIGHT ? RearCol + 1 : RearCol;

	static constexpr int nextFrontRow = InDirection == UP ? FrontRow - 1 : InDirection == DOWN ? FrontRow + 1 : FrontRow;
	static constexpr int nextFrontCol = InDirection == LEFT ? FrontCol - 1 : InDirection == RIGHT ? FrontCol + 1 : FrontCol;
	
	// Check if the step is valid
	static_assert(nextFrontRow >= 0 && nextFrontRow < InBoard::size && nextFrontCol >= 0 && nextFrontCol < InBoard::head::size, "the fery and the sad");
	static_assert(GetCellMove(nextFrontRow, nextFrontCol)::type == EMPTY, "fery five");
	

	// Set the original rear cell to EMPTY
	using EmptyRear = typename SetCell<InBoard, RearRow, RearCol, EMPTY, UP, 0>::NewBoard;

	// Set the next front to the car's data
	using FilledFront = typename SetCell<EmptyRear, nextFrontRow, nextFrontCol, GetCellMove(FrontRow, FrontCol)::type, GetCellMove(FrontRow, FrontCol)::direction, GetCellMove(FrontRow, FrontCol)::length>::NewBoard;

	// Set the moved board as the result
	typedef typename MakeMove<FilledFront, nextRearRow, nextRearCol, nextFrontRow, nextFrontCol, InDirection, InAmount - 1>::result result;
};

// Specilization for InAmount = 0 to end the recursion
template <typename InBoard, int RearRow, int RearCol, int FrontRow, int FrontCol, Direction InDirection>
struct MakeMove <InBoard, RearRow, RearCol, FrontRow, FrontCol, InDirection, 0> {

	typedef InBoard result;

};

// ========================================================================================

template <typename InBoard, int InRow, int InCol, Direction InDirection, int InAmount>
struct MoveVehicle {

	// Check the coordinates
	static_assert(InRow >= 0 && InCol >= 0 && InRow < InBoard::height && InCol < InBoard::width, "sad7");

	// Check for valid cell content
	static_assert(GetCell(InRow, InCol)::type != EMPTY, "2 fery 2 sad");

	// Check for valid move direction
	static_assert(DirectionMismatch<GetCell(InRow, InCol)::direction, InDirection>::value, "fery sad: tokyo drift");

	// Find the coordinates of the vehicle
	static constexpr int rearRow = GetCoordinates<InBoard, InRow, InCol, InDirection>::rearRow;
	static constexpr int rearCol = GetCoordinates<InBoard, InRow, InCol, InDirection>::rearCol;
	static constexpr int frontRow = GetCoordinates<InBoard, InRow, InCol, InDirection>::frontRow;
	static constexpr int frontCol = GetCoordinates<InBoard, InRow, InCol, InDirection>::frontCol;

	// Move the vehicle
	typedef GameBoard<typename MakeMove<typename InBoard::board, rearRow, rearCol, frontRow, frontCol, InDirection, InAmount>::result> board;

};

#endif