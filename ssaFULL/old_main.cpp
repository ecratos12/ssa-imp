#include <iostream>
#include <eigen3/Eigen/Core>
#include <ssaalgo.h>
#include <fstream>
#include <string>
#include <QString>
#include <iomanip>

using namespace Eigen;
using namespace std;

const int L = 240;
const double THRESHOLD = .001;

void carveData(SsaAlgo &doo, string fileName);

void signal_noise_determining(SsaAlgo &DO, string fname);

int main()
{
    ifstream filesList("/home/ecratos/ssa_project/ListOfSeriesNames.txt");
    string temp;
    do
    {
        std::getline(filesList, temp);
        TimeSeries data(temp.c_str(), true);
        SsaAlgo DO(data,L);
        string fname = QString::fromStdString(temp).section('/',5,5).toStdString().c_str();
        carveData(DO,fname);
    } while(temp != "");
    return 0;
}

void carveData(SsaAlgo &doo, string fileName)
{
    int N = doo.getN(),
        currentGapLength(0),
        x(0);
    TimeSeries dat(doo.getFinalSeries());
    cout << "----" << fileName << endl;
    for (int i=0; i<N; i++)
    {
        if (dat.getValues()(i).TROTOT == 0) {currentGapLength++;}
        if ((currentGapLength > L && dat.getValues()(i).TROTOT != 0) || i==N-1)
        {
            string fname = fileName +
                                     QString::number(x).toStdString() + "__" +
                                     QString::number(i-currentGapLength).toStdString();
            TimeSeries temp(dat,x,i-currentGapLength);
            cout << "   Doing decomposition from " << x << " to " << i-currentGapLength << endl;
            SsaAlgo DO(temp,L);
            DO.ssa();
            signal_noise_determining(DO, fname);
            x = i;
            currentGapLength = 0;
        }
    }
}

void signal_noise_determining(SsaAlgo &DO, string fname)
{
    cout << "d : ";
    while (DO.getd()>2)
    {
        int d = DO.getd();
        DO.reduction(THRESHOLD);
        cout << d << ", ";
        if (d==DO.getd()) break;
    } cout << endl;
    if (DO.getd()!=2)
    {
        cout << "Cannot reduce! d = " << DO.getd() << endl;
        DO.resetD();
        x = i;
        currentGapLength = 0;
        continue;
    }
    DO.resetD();

    ofstream result(("res" + fname).c_str()),
            signal(("sig" + fname).c_str()),
            noise(("nos" + fname).c_str());
    double signalVar=0,noiseVar=0;
    double mS = DO.reducedData.row(0).mean(),
           mN = DO.reducedData.row(1).mean();
    for (int i=0; i<DO.getN(); i++)
    {
        signalVar += (mS - DO.reducedData(0,i))*(mS - DO.reducedData(0,i));
        noiseVar += (mN - DO.reducedData(1,i))*(mN - DO.reducedData(1,i));
    }
    result << setw(20) << setprecision(4) << signalVar
        << setw(20) << setprecision(4) << noiseVar << endl;
    for (int i=0; i<DO.getN(); i++)
    {
        signal << setw(8) << i << setw(10) << setprecision(2) << DO.reducedData(0,i) << endl;
        noise  << setw(8) << i << setw(10) << setprecision(2) << DO.reducedData(1,i) << endl;
    }
}
