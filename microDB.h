#ifndef DB_MAIN_H
#define DB_MAIN_H

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stack>
#include <ctime>
#include <time.h>

using namespace std;

class Data {
public:
    double getX() const;

    void setX(double x);

    double getY() const;

    void setY(double y);

    double getZ() const;

    void setZ(double z);

    double getW() const;

    void setW(double w);

    double getIndex(Data d) {
        return d.X + d.Y + d.W + d.Z;
    }

    Data(double x, double y, double z, double w) {
        this->X = x;
        this->Y = y;
        this->Z = z;
        this->W = w;
    };
    Data(){};

public:
    double X;
    double Y;
    double Z;
    double W;
};

class RecordData {
public:
    int id;
    string time;
    string filename;
    string OP;
    string content;

    RecordData();

    RecordData(int id, const string &time, const string &filename, const string &op, const string &content);
};

bool createFile();

bool openFile();

bool modifyAndUpdate();

unordered_map<double, vector<string> > DataMap;

stack<RecordData> recordStack;

bool selectByIndex(Data d);

bool selectAndUpdateByIndex(Data d1, Data d2);

void listAllFile();

void getDataFiles(string path, vector<string> &filenames, string suffix);

void searchByWindow();

void compareIndexWithNoIndex();

int searchWithoutIndex(double _x1, double _y1, double _z1, double _w1);

int searchWithIndex(double _x1, double _y1, double _z1, double _w1);

void createIndex();

void writeBackToCSV(vector<Data> d, string filename);

void listAllOperations();

void createLogItem(RecordData r);

void rollback();

vector<string> split(const string &str, const string &delim);

void writeBackToLog();

string getTime();

void initMenu();

void fillData();

void loadInStack();

void selectAndUpdateByIndexWithoutCreateLog(Data d1, Data d2);

#endif // DB_MAIN_H
