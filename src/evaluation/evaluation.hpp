#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

template <class T>
class Evaluation {
    public:
        Evaluation(std::string name, int nrClasses);

        void prediction(T& pred, T& actual);
        std::string summaryString();
        std::string name();
        int correct();
        int nr();
        float accuracy();
        int nrClasses();

    protected:
        std::string _name;
        int _nr = 0;
        int _correct = 0;
        int _nrClasses = 0;
    };

    template <class T>
    Evaluation<T>::Evaluation(std::string name, int nrClasses) : _nrClasses(nrClasses), _name(name) { }

    template <class T>
    int Evaluation<T>::nrClasses() {
        return _nrClasses;
    }

    template <class T>
    void Evaluation<T>::prediction(T& pred, T& actual) {
        if (pred == actual)
            _correct += 1;
        _nr += 1;
    }

    template <class T>
    std::string Evaluation<T>::summaryString() {
        std::ostringstream stream;
        stream << this->name() << ": " << this->correct() << "/" << this->nr() << " = " << this->accuracy();
        return stream.str();
    }

    template <class T>
    std::string Evaluation<T>::name() {
        return _name;
    }

    template <class T>
    float Evaluation<T>::accuracy() {
        return static_cast<float>(_correct) / _nr;
    }

    template <class T>
    int Evaluation<T>::nr() {
        return _nr;
    }

    template <class T>
    int Evaluation<T>::correct() {
        return _correct;
    }
}
