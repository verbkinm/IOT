#ifndef SUBJECT_H
#define SUBJECT_H

#include <list>
#include "observer.h"

class Subject
{
public:
    Subject(){_observers.clear();};
    virtual ~Subject(){};

    void attach(Observer* observer)
    {
        _observers.push_back(observer);
    }

    void detach(Observer* observer)
    {
        _observers.remove(observer);
    }

    void destroyObservers()
    {
        while (_observers.size())
            (*_observers.begin())->~Observer();
    }

    void notify()
    {
        for (Observer* observer : _observers)
            observer->update();
    }

    std::list<Observer*> _observers;
};

#endif // SUBJECT_H
