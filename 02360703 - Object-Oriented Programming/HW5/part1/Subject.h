#ifndef SUBJECT_H
#define SUBJECT_H

#include <list>
#include <algorithm>
#include "OOP5EventException.h"
#include "Observer.h"

template <typename T>
class Subject {

    // Private variables
    std::list<Observer<T>*> observers;

public:

    // Constructor
    Subject() = default;

    // Destructor
    ~Subject() = default;

    // notify:
    // Input: argument of type T
    // Sends the argument to all known observers
    void notify(const T& arg) {

        // Iterate over the observers and call their handleEvent function
        for (Observer<T>* current : observers) {
            current->handleEvent(arg);
        }
    }

    // addObserver:
    // Input: observer of type Observer<T>
    // Adds the observer to the list of observers
    // Throws: ObserverAlreadyKnownToSubject if the observer is already known to the subject
    void addObserver(Observer<T>& observer) {

        // Check if the observer is already known to the subject
        if (std::find(observers.begin(), observers.end(), &observer) == observers.end())
            // If not, add it
            observers.push_back(&observer);
        else
            // If it is, throw an exception
            throw ObserverAlreadyKnownToSubject();
    }

    // removeObserver:
    // Input: observer of type Observer<T>
    // Removes the observer from the list of observers
    // Throws: ObserverUnknownToSubject if the observer is not known to the subject
    void removeObserver(Observer<T>& observer) {

        // Check if the observer is known to the subject
        if (std::find(observers.begin(), observers.end(), &observer) != observers.end())
            // If so, remove it
            observers.remove(&observer);
        else
            // If not, throw an exception
            throw ObserverUnknownToSubject();
    }

    // operator+=
    // Input: observer of type Observer<T>
    // Uses the addObserver function
    // Returns self
    Subject<T>& operator+=(Observer<T>& observer) {

        // Add the observer
        addObserver(observer);

        // Return self
        return *this;
    }

    // operator-=
    // Input: observer of type Observer<T>
    // Uses the removeObserver function
    // Returns self
    Subject<T>& operator-=(Observer<T>& observer) {

        // Remove the observer
        removeObserver(observer);

        // Return self
        return *this;
    }

    // operator()
    // Input: argument of type T
    // Uses the notify function
    // Returns self
    Subject<T>& operator()(const T& arg) {

        // Notify all observers
        notify(arg);

        // Return self
        return *this;
    }

};



#endif //SUBJECT_H
