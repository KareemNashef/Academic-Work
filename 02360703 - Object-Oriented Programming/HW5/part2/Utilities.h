#ifndef UTILITIES_H
#define UTILITIES_H

// Conditional template for type selection based on a boolean condition
template<bool Condition, typename TrueType, typename FalseType>
struct Conditional { typedef TrueType value; };

// Specialization for when Condition is false
template<typename TrueType, typename FalseType>
struct Conditional<false, TrueType, FalseType> { typedef FalseType value; };

// ConditionalInteger template for integer selection based on a boolean condition
template <bool Condition, int TrueValue, int FalseValue>
struct ConditionalInteger { constexpr static int value = TrueValue; };

// Specialization for when Condition is false
template <int TrueValue, int FalseValue>
struct ConditionalInteger<false, TrueValue, FalseValue> { constexpr static int value = FalseValue; };


#endif