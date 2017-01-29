#include "ssaalgo.h"
#include <iostream>
#include <vector>
#include <ftca.h>
#include <stdio.h>
#include <fstream>
#include <math.h>

using namespace std;
using namespace Eigen;

SsaAlgo::SsaAlgo(TimeSeries &data, unsigned int L)
{
    final = data;
    this->L = L;
    N = data.size;
    d = 0;
    assert(L<N);
}

SsaAlgo::SsaAlgo(TimeSeries &data)
{
    final = data;
    N = data.size;
    d = 0;
    cout << "data size = " << N << " , processing.." << endl;
    setOptimalL();
    cout << "L = " << L << endl;
}

void SsaAlgo::attachments()
{
    S.resize(L,L);
    unsigned int K = N-L+1;
    for (unsigned int i=0; i<L; i++)
        for (unsigned int j=0; j<L; j++)
        {
            S(i,j) = 0;
            for (unsigned int k=0; k<K; k++)
                S(i,j) += final.values(i+k).TROTOT * final.values(j+k).TROTOT;
        }
        cout << "Attachments done.." << endl;
}

void SsaAlgo::rank()
{
    evSolver = new SelfAdjointEigenSolver<MatrixXd>();
    evSolver->compute(S);
    eval = evSolver->eigenvalues();
    evec = evSolver->eigenvectors();
    delete evSolver;

    d = L;
    for (unsigned int i=0; i<L; i++)
        if (eval(i) < 0) d--;
    eigenvaluesSortDecrease();
    cout << "Eigen calculated.." << endl;
}

void SsaAlgo::eigenvaluesSortDecrease()
{
    for (unsigned int i=0; i<L; i++)
        for (unsigned int j=0; j<i; j++)
            if (eval(i) > eval(j))
            {
                std::swap(eval(i), eval(j));
                VectorXd t = evec.col(i);
                evec.col(i) = evec.col(j);
                evec.col(j) = t;
            }
}

void SsaAlgo::decomposition()
{
    unsigned int K = N-L+1;
    unsigned int max = (L>K)?L:K;
    unsigned int min = (L>K)?K:L;

    seriesComponents.resize(d,N);

    for (unsigned int i=0; i<d; i++)
    {

        for (unsigned int step=0; step < min ; step++)
        {
            seriesComponents(i,step) = 0;

            for (unsigned int a=0; a<=step; a++)
                for (unsigned int k=0; k<L; k++)
                    seriesComponents(i,step) += evec(a,i)
                                              * evec(k,i)
                                              * final.values(step-a+k).TROTOT;
            seriesComponents(i,step) /= (step+1);
        }

        for (unsigned int step = min ; step < max ; step++)
        {
            seriesComponents(i,step) = 0;

            if (L<K)
            {
                for (unsigned int a=0; a<L; a++)
                    for (unsigned int k=0; k<L; k++)
                        seriesComponents(i,step) += evec(a,i)
                                                  * evec(k,i)
                                                  * final.values(step-a+k).TROTOT;
                seriesComponents(i,step) /= L;
            }
            else
            {
                for (unsigned int b=0; b<K; b++)
                    for (unsigned int k=0; k<K; k++)
                        seriesComponents(i,step) += evec(step-b,i)
                                                  * evec(k,i)
                                                  * final.values(b+k).TROTOT;
                seriesComponents(i,step) /= K;
            }
        }

        for (unsigned int step = max ; step < N; step++)
        {
            seriesComponents(i,step) = 0;

            for (unsigned int a = L-(N-step) ; a<L; a++)
                for (unsigned int k=0; k<L; k++)
                    seriesComponents(i,step) += evec(a,i)
                                              * evec(k,i)
                                              * final.values(step-a+k).TROTOT;
            seriesComponents(i,step) /= (N-step);
        }
            if (i!=0) printf("%c[2K", 27);
            cout << "Decomposition:   " << i+1 << "/" << d << " complete.." << endl;
    }
}

void SsaAlgo::writeCorrMatrix()
{
    corrMatr.resize(d,d);
    for (unsigned int i=0; i<d; i++)
        for (unsigned int j=0; j<d; j++)
            corrMatr(i,j) = correlation(seriesComponents.row(i), seriesComponents.row(j));
}

double SsaAlgo::correlation(VectorXd a, VectorXd b)
{
    double aver_a(0),
           aver_b(0),
           numerator(0),
           denominator_a(0),
           denominator_b(0);
    unsigned int n = std::max(a.cols(), a.rows());

    for (unsigned int i=0; i<n; i++)
    {
        aver_a += a(i);
        aver_b += b(i);
    }
    aver_a /= n;
    aver_b /= n;

    for (unsigned int i=0; i<n; i++)
    {
        numerator +=     (aver_a - a(i))*(aver_b - b(i));
        denominator_a += (aver_a - a(i))*(aver_a - a(i));
        denominator_b += (aver_b - b(i))*(aver_b - b(i));
    }
    return numerator/sqrt(denominator_a * denominator_b);
}

int SsaAlgo::ssa(double criterion)
{
    if (L==0)
    {
        d=1;
        seriesComponents.resize(d,N);
        for (unsigned int i=0; i<N; i++)
            seriesComponents(0,i) = final.values(i).TROTOT;
        return -1;
    }
    attachments();
    rank();
    int r = sequentialAnalysis(criterion);
    decomposition();
    writeCorrMatrix();
    return r;
}

void SsaAlgo::setOptimalL()
{
    int lengthMaxGap(0), currentGapLength(0);
    for (unsigned int i=0; i<N; i++)
        if (final.values(i).TROTOT == 0)
        {
            currentGapLength++;
            if (currentGapLength > lengthMaxGap) lengthMaxGap = currentGapLength;
        }
            else {currentGapLength = 0;}

    L = 2*lengthMaxGap;
    // no gaps
    if (L==0 && N>48) L=48; // 24hours - minimum period
    if (L==0 && N<=48) L=0; // senceless to do ssa

    if (L%24 != 0) L += 24 - L%24;
}

void SsaAlgo::writeFinalSeries(string fname)
{
    fname += "__FINAL";
    ofstream out(fname.c_str());
    for (unsigned int i=0; i<N; i++)
        out << i << "       " << final.values(i).TROTOT << endl;
}

int SsaAlgo::sequentialAnalysis(double criterion)
{
    VectorXd reccurentCoef;
    reccurentCoef.resize(L-1);
    for (unsigned int a=0; a<(L-1); a++)
    {
        reccurentCoef(a) = 0;
        for (unsigned int i=0; i<(L-1); i++)
            reccurentCoef(a) += evec(L-1,i)*evec(a,i);
    }
    int counter=0;
    bool previousWasNull = false;
    for (unsigned int next=23; next+1<N; next++)
    {
                                                                            double mean=0,sdv=0;
                                                                            for (unsigned int a=1; a<L; a++)
                                                                                mean += final.values(next-a).TROTOT;
                                                                            mean/=L-1;
                                                                            for (unsigned int a=1; a<L; a++)
                                                                                sdv += (mean-final.values(next-a).TROTOT)
                                                                                        *(mean-final.values(next-a).TROTOT);
                                                                            sdv/=L-1;
                                                                            sdv = sqrt(sdv);
            if (final.values(next).TROTOT==0)
            {
                for (unsigned int a=0; a<(L-1); a++)
                    final.values(next).TROTOT += reccurentCoef(a) * final.values(next-L+1+a).TROTOT;
                final.values(next).TROTOT /= evec(L-1,L-1)*evec(L-1,L-1);
                previousWasNull = true;
            }
            else
            {
                if (abs(final.values(next).TROTOT-mean) > criterion*sdv && (final.values(next+1).TROTOT==0 || previousWasNull))
                {
                    final.values(next).TROTOT = 0;
                    for (unsigned int a=0; a<(L-1); a++)
                        final.values(next).TROTOT += reccurentCoef(a) * final.values(next-L+1+a).TROTOT;
                    final.values(next).TROTOT /= evec(L-1,L-1)*evec(L-1,L-1);
                    counter++;
                }
                previousWasNull = false;
            }
    }
    return counter;
}
