#include <iostream>
#include <eigen3/Eigen/Core>
#include <ssaalgo.h>
#include <fstream>
#include <string>
#include <QString>
#include <iomanip>
#include <math.h>
using namespace Eigen;
using namespace std;

const int L = 24;

int main()
{    
    ifstream seriesNames("ListOfSeriesNames.txt");
    ofstream out1("newTable24", std::ios::app);
    out1 << std::fixed;
    out1.precision(3);
    out1 << "name   N   outliners   criterion   var(3comps)   var(4comps)   varAll   without3toAll   without4toAll" << endl;
    do
    {
        string name; double var3,var4,varAll;
        std::getline(seriesNames, name);
        TimeSeries series(name.c_str());
        name = QString::fromStdString(name).section('/',5,5).toStdString();
        for (double crit=2; crit<3.1; crit+=0.5)
        {
            SsaAlgo DO(series,L);
            int outliners = DO.ssa(crit);
//            DO.writeFinalSeries(name);
            for (int a=0; a<DO.getd(); a++)
            {
                double m = DO.seriesComponents.row(0).mean(), res = 0;
                for (int i=0; i<DO.seriesComponents.row(0).size(); i++)
                    res += (m-DO.seriesComponents(0,i))*(m-DO.seriesComponents(0,i));
                if (a == 2) var3 = res/DO.getN();
                if (a == 3) var4 = res/DO.getN();
                if (a == DO.getd()-1) {varAll = res/DO.getN(); break;}
                DO.seriesComponents.row(0) += DO.seriesComponents.row(a+1);
            }
            out1 << name << " & "
                 << DO.getN() << " & "
                 << outliners << " & "
                 << crit << " & "
                 << std::scientific << var3 << " & "
                 << std::scientific << var4 << " & "
                 << std::scientific << varAll;
            out1.unsetf(std::ios_base::scientific);
            out1 << " & " << 1 - var3/varAll << " & " << 1 - var4/varAll << endl;
        }
        out1 << endl;
    } while (seriesNames.is_open());
}
