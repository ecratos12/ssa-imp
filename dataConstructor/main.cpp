#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <QString>
#include <QDateTime>

using namespace std;

int filesList(const char* fname, const char *dirName);
void sortStringsFileAlphabet(const char* fname, int linesNumber);
void quickSort(string *, int);
void createFinalDataFile(const char*, const char*);

int main()
{
    int numberOfNames = filesList("ListOfSeriesNames.txt", "/home/ecratos/ssa_project/TROPO/");
    sortStringsFileAlphabet("ListOfSeriesNames.txt", numberOfNames);

    ifstream in("ListOfSeriesNames.txt");
    string temp_dat, temp_res;
    do
    {
        std::getline(in,temp_dat);
        std::getline(in,temp_res);
        createFinalDataFile(temp_dat.c_str(), temp_res.c_str());
    } while (temp_dat != "" && temp_res != "");

    numberOfNames = filesList("ListOfSeriesNames.txt", "/home/ecratos/ssa_project/FINAL/");
    sortStringsFileAlphabet("ListOfSeriesNames.txt", numberOfNames);

    return 0;
}
/**
 * @brief filesList
 * @param fname
 * @return File contains names of all tropo data files
 */
int filesList(const char* fname, const char* dirName)
{
    ofstream out(fname);
    DIR *pDIR;
    struct dirent *entry;
    int numberOfFiles = 0;
    if (pDIR = opendir(dirName))
    {
        while (entry = readdir(pDIR))
        {
            if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name, "..") != 0) {
                numberOfFiles++;
                out << dirName << entry->d_name << endl;
            }
        }
    }
    return numberOfFiles;
}
/**
 * @brief sortStringsFileAlphabet
 * @param fname
 * @param linesNumber
 * Sorting files lines by alphabet
 */
void sortStringsFileAlphabet(const char* fname, int linesNumber)
{
    ifstream File(fname);
    string* tempArray = new string[linesNumber];
    for (int i=0; i<linesNumber; i++) std::getline(File,tempArray[i]);
    quickSort(tempArray, linesNumber);
    File.close();

    ofstream FileWrite(fname);
    for (int i=0; i<linesNumber; i++) FileWrite << tempArray[i] << endl;
}

void quickSort(string* array, int size)
{
    for (int i=0; i<size; i++)
        for (int j=i; j<size; j++)
            if (strcmp(array[i].c_str(), array[j].c_str())>0) std::swap(array[i], array[j]);
}

void createFinalDataFile(const char *datFname, const char *resFname)
{
    ifstream dat(datFname), res(resFname);

    string dirFinal = "/home/ecratos/ssa_project/FINAL/";
    dirFinal += QString::fromUtf8(datFname).section('/',5,5).toStdString();
    ofstream final(dirFinal.c_str());

    string temp, temp_dat; // temp var's

    // needful characteristic var's
    int size;
    const QString resTimeFormat("yyyy MM dd HH mm ss"),
                  datTimeFormat("yyyy-MM-dd/HH:mm:ss");

    QDateTime dateStart, dateFinish;

    std::getline(res,temp);

    std::getline(res,temp);
    dateStart = QDateTime::fromString(QString::fromStdString(temp), resTimeFormat);
    std::getline(res,temp);
    dateFinish = QDateTime::fromString(QString::fromStdString(temp), resTimeFormat);
    std::getline(res,temp);
    size = QString::fromStdString(temp).section(' ',2,2,QString::SectionSkipEmpty).toInt();

    std::getline(dat,temp_dat); // first line from .dat has no sence

    int step = 0;
    final << size << endl;
    do
    {
        std::getline(res,temp);
        int gapStart  = QString::fromStdString(temp).section(' ',1,1,QString::SectionSkipEmpty).toInt();
        int gapFinish = QString::fromStdString(temp).section(' ',2,2,QString::SectionSkipEmpty).toInt();

        for (int i=step; i<gapStart; i++)
        {
            std::getline(dat,temp_dat);
            final << QString::fromStdString(temp_dat).section(' ',0,1,QString::SectionSkipEmpty).toStdString() << endl;
        }
        QDateTime currentDate (QDateTime::fromString(QString::fromStdString(temp_dat).section(' ',0,0,QString::SectionSkipEmpty), datTimeFormat));
        for (int i=gapStart; i<=gapFinish; i++)
        {
            currentDate = currentDate.addSecs(3600);
            final << currentDate.toString(datTimeFormat).toStdString().c_str() << "      " << "0" << endl; // '0' means empty
        }
        step = gapFinish+1;
    } while(temp[0] == 'G');
    for (int i=step; i<size; i++)
    {
        std::getline(dat,temp_dat);
        final << QString::fromStdString(temp_dat).section(' ',0,1,QString::SectionSkipEmpty).toStdString() << endl;
    }
}
