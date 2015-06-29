#pragma once

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
        unsigned int* confusionMatrix();


    protected:
        int& cell(int pred, int actual);

        unsigned int* _confusionMatrix;
        std::string _name;
        int _nr = 0;
        int _correct = 0;
        int _nrClasses = 0;
    };

    Evaluation::Evaluation(std::string name, int nrClasses) : _nrClasses(nrClasses), _name(name) {
        _confusionMatrix = new unsigned int[nrClasses * nrClasses];
        for (auto i = 0u; i < _nrClasses * _nrClasses; i++)
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
//        _confusionMatrix[pred * nrClasses() + actual] += 1;
//        _confusionMatrix[pred * nrClasses() + actual] += 1;
        if (pred == actual)
            _correct += 1;
        _nr += 1;
    }

    std::string Evaluation::summaryString() {
        std::ostringstream stream;
        stream << this->name() << ": " << this->correct() << "/" << this->nr() << " = " << this->accuracy() << std::endl;
        stream << "Confusion Matrix:";
        for (auto i = 0u; i < _nrClasses * _nrClasses; i++) {
            if (i % _nrClasses == 0)
                stream << std::endl;

            stream << std::setw(7) << _confusionMatrix[i] << " ";
        }
        return stream.str();
    }

    std::string Evaluation::name() {
        return _name;
    }

    float Evaluation::accuracy() {
        return static_cast<float>(_correct) / _nr;
    }

    int Evaluation::nr() {
        return _nr;
    }

    int Evaluation::correct() {
        return _correct;
    }

    unsigned int* Evaluation::confusionMatrix() {
        return _confusionMatrix;
    }

    int &Evaluation::cell(int pred, int actual) {
        return (int &) _confusionMatrix[actual * nrClasses() + pred];
    }

}
