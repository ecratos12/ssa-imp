#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <eigen3/Eigen/Core>
#include <QDateTime>
#include <measurement.h>

using namespace Eigen;

typedef Matrix<Measurement, 1, Dynamic> VectorValues;

class TimeSeries
{
    friend class SsaAlgo;
public:
    TimeSeries();
    TimeSeries(unsigned int size);
    TimeSeries(const char* fileName, bool _setZeroMean = true);
    TimeSeries(TimeSeries &a);
    TimeSeries(TimeSeries &a, unsigned int fromElement, unsigned int toElement);

    inline int gSize() {return size;}
    inline const VectorValues getValues() const {return values;}

    void setValue(unsigned int index, double _TROTOT, QDateTime _time = QDateTime::currentDateTime());
    void setMean();
    void setZeroMean();

    void sortChronologically();

    TimeSeries& operator= (const TimeSeries &a)      ;
    TimeSeries  operator+=(const TimeSeries &a) const;

protected:
    VectorValues values;
    double mean;
    unsigned int size;
};

#endif // TIMESERIES_H
