#include <iostream>
#include <eigen3/Eigen/Core>
#include <ssaalgo.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <ftca.h>
#include <math.h>
#include <time.h>

using namespace Eigen;
using namespace std;

//void writeComp(SsaAlgo &doo,int L);
//void writeCorr(SsaAlgo &doo,int L);
//void writeEigenSpectrum(SsaAlgo &doo, int L);

int main()
{
    TimeSeries series("BOR1_TRO.dat");
    TimeSeries solid(series, 36048, 46447);
    SsaAlgo DO(solid,24);
    DO.ssa();
    for (int a=0; a<4; a++)
    {
        ofstream out((std::to_string(a+1)+"comp").c_str());
        for (int i=0; i<DO.getN(); i++)
            out << i << "       " << DO.seriesComponents(0,i) << endl;
        DO.seriesComponents.row(0) += DO.seriesComponents.row(a+1);
    }
    DO.writeFinalSeries("solid");
    return 0;
}
