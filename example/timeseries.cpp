#ifndef TIMESERIES_CPP
#define TIMESERIES_CPP

#include "timeseries.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <QString>
#include <string.h>

using namespace std;

TimeSeries::TimeSeries()
{
    mean = 0;
    size = 0;
}

TimeSeries::TimeSeries(unsigned int size)
{
    values.resize(size);
    this->size = size;
    mean = 0;
}

TimeSeries::TimeSeries(TimeSeries &a)
{
    values = a.values;
    mean = a.mean;
    size = a.size;
}

TimeSeries::TimeSeries(TimeSeries &a, unsigned int fromElement, unsigned int toElement)
{
    assert(fromElement != toElement);
    size = abs(toElement - fromElement) + 1;
    values.resize(size);
    if (fromElement < toElement)
    {
        for (unsigned int i=0; i<size; i++)
            values(i) = a.values(fromElement+i);
    }
    else
    {
        for (unsigned int i=0; i<size; i++)
            values(i) = a.values(fromElement-i);\
    }
    setZeroMean();
}

TimeSeries::TimeSeries(const char *fileName, bool _setZeroMean)
{
    ifstream in(fileName);
    assert(in.is_open());

    string temp;
    QString qtemp;
    QString timeFormat("yyyy-MM-dd/HH:mm:ss");

    in >> size;
    values.resize(size);
    getline(in, temp);

    for (unsigned int i=0; i<size; i++)
    {
        getline(in, temp);
        qtemp = qtemp.fromStdString(temp);

        values(i).time = QDateTime::fromString(qtemp.section(' ',0,0, QString::SectionSkipEmpty), timeFormat); //Epoch
        values(i).TROTOT = qtemp.section(' ',1,1, QString::SectionSkipEmpty).toDouble(); //TROTOT
    }
    if (_setZeroMean) setZeroMean();

}

void TimeSeries::setValue(unsigned int index, double _TROTOT, QDateTime _time)
{
    values(index).time = _time;
    values(index).TROTOT = _TROTOT;
}

void TimeSeries::setMean()
{
    mean = 0;
    int count = 0;
    for (unsigned int i=0; i<size; i++)
        if (values(i).TROTOT != 0)
        {
            mean += values(i).TROTOT;
            count++;
        }
    if (count != 0) mean /= count;
}

void TimeSeries::setZeroMean()
{
    setMean();
    for (unsigned int i=0; i<size; i++)
        if (values(i).TROTOT != 0) values(i).TROTOT -= mean;
    mean = 0;
}

void TimeSeries::sortChronologically()
{
    std::sort(values.data(), values.data() + size - 1);
}

TimeSeries& TimeSeries::operator= (const TimeSeries &a)
{
    size = a.size;
    mean = a.mean;
    values.resize(size);

    for (unsigned int i=0; i<size; i++)
        values(i) = a.values(i);
    return *this;
}

#endif // TIMESERIES_CPP
