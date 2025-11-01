#ifndef LIST_H
#define LIST_H

// Forward declaration for List
template <typename... Types>
struct List;

// General case - Linked list with 2+ elements
template<typename HeadType, typename... RestType>
struct List<HeadType, RestType...> {
    typedef HeadType head;
    typedef List<RestType...> next;
    static constexpr int size = sizeof...(RestType) + 1;
};

// Specialization for a list with 1 element
template<typename HeadType>
struct List<HeadType> {
    typedef HeadType head;
    typedef List<> next;
    static constexpr int size = 1;
};

// Specialization for a list with 0 elements
template<>
struct List<> {
    typedef List<> head;
    typedef List<> next;
    static constexpr int size = 0;
};

// ========================================================================================

// Forward declaration for PrependList
template<typename NewHead, typename ExistingList>
struct PrependList;

// Specialization for unpacking ExistingList
template<typename NewHead, typename... Elements>
struct PrependList<NewHead, List<Elements...>> {
    typedef List<NewHead, Elements...> list;
};

// ========================================================================================

// Forward declaration for GetAtIndex
template<int Index, typename InList>
struct GetAtIndex {};

// Base case: Index is 0
template<typename InHead, typename ...InRest>
struct GetAtIndex<0, List<InHead, InRest...>> {
    typedef InHead value;
};

// Recursive case: Decrease Index and move to the next element in the list
template<int Index, typename InHead, typename... InRest>
struct GetAtIndex<Index, List<InHead, InRest... >> {
    typedef typename GetAtIndex<Index - 1, List<InRest...>>::value value;
};

// ========================================================================================

// Forward declarations to avoid partial specialization issues
template<int Index, typename NewType, typename List>
struct SetAtIndex;

// Base case: When Index is 0, create a new list with NewType as head
template<typename NewType, typename Head, typename... Rest>
struct SetAtIndex<0, NewType, List<Head, Rest...>> {
    typedef List<NewType, Rest...> list;
};

// Recursive case: Decrease Index and move to the next element in the list
template<int Index, typename NewType, typename Head, typename... Rest>
struct SetAtIndex<Index, NewType, List<Head, Rest...>> {
    typedef typename PrependList<Head, typename SetAtIndex<Index - 1, NewType, List<Rest...>>::list>::list list;
};

#endif