#ifndef OBSERVER_H
#define OBSERVER_H


// Generic class Observer

template <typename T>
class Observer {

public:

    // Constructor
    Observer() = default;

    // Destructor
    virtual ~Observer() = default;

    // handleEvent:
    // Input: argument of type T
    // Pure virtual function, Implement in derived classes
    virtual void handleEvent(const T&) = 0;

};



#endif //OBSERVER_H
