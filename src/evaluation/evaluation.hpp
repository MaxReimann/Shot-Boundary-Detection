#pragma once

#include <iomanip>
#include "../forwarddeclarations.hpp"
#include <iomanip>

namespace sbd {

class Evaluation {
    public:
        Evaluation(std::string name, int nrClasses);
        virtual ~Evaluation();

        void prediction(int pred, int actual);
        std::string summaryString();
        std::string name();
        int correct();
        int nr();
        float accuracy();
        int nrClasses();
        float precision(int);
        float recall(int);
        unsigned int* confusionMatrix();


    protected:
        int& cell(int pred, int actual);

        unsigned int* _confusionMatrix;
        std::string _name;
        int _nrClasses = 0;
        int _matrixSize = 0;
    };

    Evaluation::Evaluation(std::string name, int nrClasses) : _nrClasses(nrClasses), _name(name) {
        _matrixSize = _nrClasses * _nrClasses;
        _confusionMatrix = new unsigned int[_matrixSize];
        for (auto i = 0u; i < _matrixSize; i++)
            _confusionMatrix[i] = 0;
    }

    Evaluation::~Evaluation() {
        delete[] _confusionMatrix;
    }

    int Evaluation::nrClasses() {
        return _nrClasses;
    }

    void Evaluation::prediction(int pred, int actual) {
        assert(pred >= 0 && pred < nrClasses());
        assert(actual >= 0 && actual < nrClasses());
        cell(pred, actual) += 1;
    }

    std::string Evaluation::summaryString() {
        std::ostringstream stream;
        stream << this->name() << ": " << this->correct() << "/" << this->nr() << " = " << this->accuracy() << std::endl;
        stream << "Confusion Matrix:";
        for (auto i = 0u; i < _matrixSize; i++) {
            if (i % _nrClasses == 0)
                stream << std::endl;

            stream << std::setw(7) << _confusionMatrix[i] << " ";
        }
        for (auto i = 0u; i < _nrClasses; i++) {
            stream << std::endl << i << ": Precision = " << precision(i) << ", Recall = " << recall(i);
        }
        return stream.str();
    }

    std::string Evaluation::name() {
        return _name;
    }

    float Evaluation::accuracy() {
        return static_cast<float>(correct()) / nr();
    }

    int Evaluation::nr() {
        int sum = 0;
        for (auto i = 0u; i < _matrixSize; i++) {
            sum += _confusionMatrix[i];
        }
        return sum;
    }

    int Evaluation::correct() {
        int sum = 0;
        for (auto i = 0u; i < _nrClasses; i++) {
            sum += cell(i, i);
        }
        return sum;
    }

    float Evaluation::precision(int clazz) {
        int count = 0;
        int correct = 0;
        for (auto i = 0u; i < _nrClasses; i++) {
            count += cell(i, clazz);
            if (i == clazz)
                correct = cell(clazz, clazz);
        }
        return static_cast<float>(correct) / count;
    }

    float Evaluation::recall(int clazz) {
        int count = 0;
        int correct = 0;
        for (auto i = 0u; i < _nrClasses; i++) {
            count += cell(clazz, i);
            if (i == clazz)
                correct = cell(clazz, clazz);
        }
        return static_cast<float>(correct) / count;
    }

    unsigned int* Evaluation::confusionMatrix() {
        return _confusionMatrix;
    }

    int &Evaluation::cell(int pred, int actual) {
        return (int &) _confusionMatrix[actual * nrClasses() + pred];
    }

}
