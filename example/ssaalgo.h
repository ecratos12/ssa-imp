#ifndef SSAALGO_H
#define SSAALGO_H

#include <timeseries.h>
#include <eigen3/Eigen/Eigenvalues>
#include <eigen3/Eigen/Dense>

class SsaAlgo
{
public: // core
        SsaAlgo() {}
        SsaAlgo(TimeSeries &data);  // runs with min possible L
        SsaAlgo(TimeSeries &data, unsigned int L); // runs with manual L

        void attachments();
        void rank();
        void forecasting(); //using linear reccurent formula
        void decomposition();
        void writeCorrMatrix();
        void ssa(); // full implemetation


public: // acsess
        inline void setL(unsigned int l) {assert(l<N); L=l;}
        inline void resetD() {d = seriesComponents.col(0).size(); writeCorrMatrix();}

        inline int getd() {return d;}
        inline int getL() {return L;}
        inline int getN() {return N;}
        inline int clustersCount() {return _clustersCount;}
        inline TimeSeries& getFinalSeries() {return final;}
        inline const VectorXd getEval() {return eval;}
        inline MatrixXd getCorrelationMatrix() {return corrMatr;}

        void writeFinalSeries(std::string fname);

public: // service
        static double correlation(VectorXd a, VectorXd b);

public: // result
        MatrixXd seriesComponents;

protected:
//        result
    TimeSeries final;
    SelfAdjointEigenSolver<MatrixXd>* evSolver;
    /**
     * Because of SelfAdjointEigenSolver's results are const
     * we cannot sort eigenvalues
     * So we going to write eigens to the common vector and matrix
    */
    VectorXd eval;
    MatrixXd evec;
    MatrixXd corrMatr;
//        parameters
    unsigned int d;
    unsigned int L;
    int _clustersCount;
    unsigned int N; // equals data.size
//        service
    MatrixXd S;
    void eigenvaluesSortDecrease();
    void setOptimalL();
};

#endif // SSAALGO_H
