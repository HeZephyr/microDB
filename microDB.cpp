#include "microDB.h"

std::string title = "微型数据存储和查询系统";
std::string menu[] = {
    "1 创建文件",
    "2 修改/更新文件",
    "3 管理文件",
    "4 查询处理（有无索引）",
    "5 事务功能",
    "0 退出"
};
int width = title.length() + 6; // 菜单宽度

bool flag = false; // 是否进行了数据补全

void initMenu() {

    // 输出顶部和底部边框
    std::cout << std::string(width, '*') << std::endl;

    // 输出带有居中标题的行
    std::cout << "*" << std::string(8, ' ') << title << std::string(7, ' ') << "*" << std::endl;

    std::cout << std::string(width, '*') << std::endl;

    // 输出菜单项
    for (const auto& item : menu) {
        std::cout << "*" << std::string(2, ' ');
        std::cout << item << std::endl;
    }

    // 输出底部边框
    std::cout << std::string(width, '*') << std::endl;
}

void fillData() {
    // 使用每列的平均值填充缺失的数据
    vector<string> filenames;
    string suffix = "csv";
    string path = "./";
    getDataFiles(path, filenames, suffix);
    for (auto filename : filenames) {
        ifstream fp("./" + filename); // 定义声明一个ifstream对象，指定文件路径
        string line;
        getline(fp, line); // 跳过列名，第一行不做处理
        double x_avg = 0, y_avg = 0, z_avg = 0, w_avg = 0;  // 每列的平均值
        int x_count = 0, y_count = 0, z_count = 0, w_count = 0;
        int x_miss = 0, y_miss = 0, z_miss = 0, w_miss = 0;
        double _x, _y, _z, _w;
        while (getline(fp, line)) { // 循环读取每行数据
            string number;
            istringstream readstr(line); // string数据流化
            // 将一行数据按'，'分割
            // 可根据数据的实际情况取循环获取
            getline(readstr, number, ','); // 读取X数据
            if (number != "") {
                _x = stod(number);
                x_avg += _x;
                x_count++;
            } else {
                x_miss++;
            }
            getline(readstr, number, ','); // 读取Y数据
            if (number != "") {
                _y = stod(number);
                y_avg += _y;
                y_count++;
            } else {
                y_miss++;
            }
            getline(readstr, number, ','); // 读取Z数据
            if (number != "") {
                _z = stod(number);
                z_avg += _z;
                z_count++;
            } else {
                z_miss++;
            }
            getline(readstr, number, ','); // 读取W数据
            // 去除最后的换行符
            if (number[number.size() - 1] == '\r' || number[number.size() - 1] == '\n') {
                number = number.substr(0, number.size() - 1);
            }
            if (number != "") {
                _w = stod(number);
                w_avg += _w;
                w_count++;
            } else {
                w_miss++;
            }
        }
        fp.close();

        // 计算平均值
        x_avg /= x_count;
        y_avg /= y_count;
        z_avg /= z_count;
        w_avg /= w_count;

        // 重新读取文件，填充缺失的数据
        ifstream fp1(path + filename);
        ofstream outfp(path + "filled_" + filename); // 定义声明一个ofstream对象，写入新的文件
        getline(fp1, line); // 跳过列名，第一行不做处理
        outfp << line << endl; // 将列名写入新文件

        while (getline(fp1, line)) {
            istringstream readstr(line); // string数据流化
            string number;
            Data data;
            getline(readstr, number, ','); // 读取X数据
            _x = (!number.empty()) ? stod(number) : x_avg;
            getline(readstr, number, ','); // 读取Y数据
            _y = (!number.empty()) ? stod(number) : y_avg;
            getline(readstr, number, ','); // 读取Z数据
            _z = (!number.empty()) ? stod(number) : z_avg;
            getline(readstr, number, ','); // 读取W数据
            if (number[number.size() - 1] == '\r' || number[number.size() - 1] == '\n') {
                number = number.substr(0, number.size() - 1);
            }
            _w = (!number.empty()) ? stod(number) : w_avg;

            // 写入新的文件，设置精度为16
            outfp << fixed << setprecision(16) << _x << "," << _y << "," << _z << "," << _w << endl;
        }
        outfp.close();
        fp1.close();
        // 备份原文件
        rename(("./" + filename).c_str(), ("./" + filename + ".bak").c_str());
        // 命名新文件
        rename(("./filled_" + filename).c_str(), ("./" + filename).c_str());

        // 输出缺失数据统计
        cout << filename << " 缺失数据统计：" << endl;
        cout << "X缺失：" << x_miss << "个，Y缺失：" << y_miss << "个，Z缺失：" << z_miss << "个，W缺失：" << w_miss << "个" << endl;
    }
}

int main() {
    int choice;
    createIndex();
    loadInStack();
    while (1) {
        initMenu();
        if (!flag) {
            fillData();
            cout << "数据补全完成！" << endl;
            flag = true;
        }
        cout << "请输入所选功能：";
        cin >> choice;
        switch (choice) {
        case 1:
            if (createFile()) {
                cout << "成功创建文件！" << endl;
            }
            else {
                cout << "文件创建失败！" << endl;
            }
            break;
        case 2:
            if (modifyAndUpdate()) {
                cout << "已更改数据" << endl;
            }
            else {
                cout << "修改失败" << endl;
            }
            break;
        case 3:
            listAllFile();
            break;
        case 4:
            searchByWindow();
            compareIndexWithNoIndex();
            break;
        case 5:
            listAllOperations();
            rollback();
            break;
        default:
            exit(0);
            break;
        }
    }
    return 0;
}

void rollback() {
    int idx;
    cout << "请选择要回退的id号：" << endl;
    cin >> idx;
    auto r = recordStack.top();
    while (idx < r.id) {
        string operate = r.content;
        vector<string> res = split(operate, ",");
        double _x1 = stof(res[0]);
        double _y1 = stof(res[1]);
        double _z1 = stof(res[2]);
        double _w1 = stof(res[3]);
        // 新值
        double _x2 = stod(res[4]);
        double _y2 = stod(res[5]);
        double _z2 = stod(res[6]);
        double _w2 = stod(res[7]);
        selectAndUpdateByIndexWithoutCreateLog(Data(_x2, _y2, _z2, _w2), Data(_x1, _y1, _z1, _w1));
        recordStack.pop();
        r = recordStack.top();
    }
    writeBackToLog();
}

void selectAndUpdateByIndexWithoutCreateLog(Data d1, Data d2) {
    double index = d1.getX() + d1.getY() + d1.getZ() + d1.getW();
    vector<string> filenames;
    auto iter = DataMap.find(index);
    if (iter != DataMap.end()) {
        filenames = iter->second;
    }
    else {
        string suffix = "csv";
        string path = "./";
        getDataFiles(path, filenames, suffix);
    }
    vector<Data> incsv;

    for (auto filename : filenames) {
        ifstream fp("./" + filename);
        string line;
        getline(fp, line);
        while (getline(fp, line)) {
            double x1, y1, z1, w1;
            string number;
            istringstream readstr(line); // string数据流化
            getline(readstr, number, ',');
            x1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            y1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            z1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            w1 = ((number != "") ? stod(number) : 0);
            if (x1 == d1.X && y1 == d1.Y && z1 == d1.Z && w1 == d1.W) {
                x1 = d2.X;
                y1 = d2.Y;
                z1 = d2.Z;
                w1 = d2.W;
            }
            incsv.push_back(Data(x1, y1, z1, w1));
        }
        fp.close();
        writeBackToCSV(incsv, filename);
    }
}

void loadInStack() {
    // 将文件中数据读出到栈中
    ifstream fp("./record.log");
    string line;
    while (getline(fp, line)) {
        vector<string> res = split(line, ",");
        RecordData rd;
        rd.id = stoi(res[0]);
        rd.time = res[1];
        rd.filename = res[2];
        rd.OP = res[3];
        rd.content = res[4] + "," + res[5] + "," + res[6] + "," + res[7] + "," + res[8] + "," + res[9] + "," + res[10] + "," + res[11];
        recordStack.push(rd);
    }
}

void writeBackToLog() {
    stack<RecordData> temp, reverse;
    temp = recordStack;
    ofstream ofp("./record.log");

    while (!recordStack.empty()) {
        reverse.push(recordStack.top());
        recordStack.pop();
    }
    while (!reverse.empty()) {
        auto rd = reverse.top();
        string line = to_string(rd.id) + "," + rd.time + "," + rd.filename + "," + rd.OP + "," +
                      rd.content;
        ofp << line << endl;
        reverse.pop();
    }
    recordStack = temp;
    ofp.close();
}

void listAllOperations() {
    string line;
    ifstream fp("./record.log");
    while (getline(fp, line)) {
        cout << line << endl;
    }
    fp.close();
}

void createLogItem(RecordData r) {
    string line;
    fstream fp("./record.log", ios::app);
    int idx = 0;
    if (!recordStack.empty()) {
        idx = recordStack.top().id + 1;
    }
    fp << idx << "," << r.time << "," << r.filename << "," << r.OP << "," << r.content << endl;
    r.id = idx;
    recordStack.push(r);
    fp.close();
}

bool createFile() {
    string fileName;
    cout << "请输入创建的文件名:" << endl;
    cin >> fileName;
    ofstream file;
    file.open("./" + fileName + ".csv", ios::app);
    if (!file) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool modifyAndUpdate() {
    double _x, _y, _z, _w, _x1, _y1, _z1, _w1;
    cout << "请输入查找的值:" << endl;
    scanf("%lf,%lf,%lf,%lf", &_x, &_y, &_z, &_w);
    Data d1 = Data(_x, _y, _z, _w);
    cout << "请输入要修改的值:" << endl;
    scanf("%lf,%lf,%lf,%lf", &_x1, &_y1, &_z1, &_w1);
    Data d2 = Data(_x1, _y1, _z1, _w1);
    return selectAndUpdateByIndex(d1, d2);
}

void listAllFile() {
    vector<string> filenames;
    string suffix = "csv";
    string path = "./";
    getDataFiles(path, filenames, suffix);
    for (int i = 0; i < filenames.size(); ++i) {
        cout << filenames[i] << endl;
    }
    for (auto it = DataMap.begin(); it != DataMap.end(); it++) {
        cout << "索引哈希key:" << it->first << "\t";
        cout << "对应的文件为";
        for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
            /* code */
            cout << "\t" << *iter;
        }
        cout << endl;
    }
}

void getDataFiles(string path, vector<string> &filenames, string suffix)
{
    DIR *pDir;
    struct dirent *ptr;
    if (!(pDir = opendir(path.c_str())))
        return;
    while ((ptr = readdir(pDir)) != 0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            string fileName = ptr->d_name;
            if (fileName.substr(fileName.find_last_of(".") + 1) == suffix) {
                filenames.emplace_back(fileName);
            }
        }
    }
    closedir(pDir);
}

void searchByWindow()
{
    double s_x, e_x, s_y, e_y, s_z, e_z, s_w, e_w;
    cout << "请输入X、Y、Z、W的范围：" << endl;
    scanf("%lf,%lf", &s_x, &e_x);
    scanf("%lf,%lf", &s_y, &e_y);
    scanf("%lf,%lf", &s_z, &e_z);
    scanf("%lf,%lf", &s_w, &e_w);
    vector<string> filenames;
    string suffix = "csv";
    string path = "./";
    getDataFiles(path, filenames, suffix);
    for (auto filename : filenames) {
        cout << filename + ":" << endl;
        cout << "X" << setw(8) << "Y" << setw(8) << "Z" << setw(8) << "W" << setw(8) << endl;
        ifstream fp("./" + filename); // 定义声明一个ifstream对象，指定文件路径
        string line;
        getline(fp, line); // 跳过列名，第一行不做处理
        while (getline(fp, line)) { // 循环读取每行数据
            string number;
            istringstream readstr(line); // string数据流化
            // 将一行数据按'，'分割
            // 可根据数据的实际情况取循环获取
            getline(readstr, number, ','); // 读取X数据
            if (number == "") {
                continue;
            }
            double _x = stod(number); // 字符串传double
            if (_x < s_x || _x > e_x) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            if (number == "") {
                continue;
            }
            double _y = stod(number); // 字符串传double
            if (_y < s_y || _y > e_y) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            if (number == "") {
                continue;
            }
            double _z = stod(number); // 字符串传double
            if (_z < s_z || _z > e_z) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            if (number == "") {
                continue;
            }
            double _w = stod(number); // 字符串传double
            if (_w < s_w || _w > e_w) {
                continue;
            }
            cout << _x << setw(8) << _y << setw(8) << _z << setw(8) << _w << setw(8) << endl;
            break;
        }
    }
}

void createIndex() {
    vector<string> filenames;
    string suffix = "csv";
    string path = "./";
    getDataFiles(path, filenames, suffix);
    for (auto filename : filenames) {
        ifstream fp("./" + filename); // 定义声明一个ifstream对象，指定文件路径
        string line;
        getline(fp, line); // 跳过列名，第一行不做处理
        while (getline(fp, line)) { // 循环读取每行数据
            string number;
            istringstream readstr(line); // string数据流化
            // 将一行数据按'，'分割
            // 可根据数据的实际情况取循环获取
            getline(readstr, number, ','); // 读取X数据
            double _x = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ','); // 读取Y数据
            double _y = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ','); // 读取Z数据
            double _z = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ','); // 读取W数据
            // 去除最后的换行符
            if (number[number.size() - 1] == '\r' || number[number.size() - 1] == '\n') {
                number = number.substr(0, number.size() - 1);
            }
            double _w = ((number != "") ? stod(number) : 0);
            double sum = _x + _y + _z + _w;
            // 如果查找到了
            auto iter = DataMap.find(sum);
            if (iter != DataMap.end()) {
                iter->second.emplace_back(filename);
            }
            // 没有找到
            else {
                vector<string> tmp;
                tmp.emplace_back(filename);
                DataMap[sum] = tmp;
            }
        }
    }
}

void compareIndexWithNoIndex() {
    double _x1, _y1, _z1, _w1;
    cout << "请输入查找的值:" << endl;
    cin >> _x1;
    cin >> _y1;
    cin >> _z1;
    cin >> _w1;
    int countWithIndex = searchWithIndex(_x1, _y1, _z1, _w1);
    // 如果count为-1，则代表没有找到
    if (countWithIndex == -1) {
        cout << "没有找到，输入数据错误！";
    }
    int countWithoutIndex = searchWithoutIndex(_x1, _y1, _z1, _w1);
    cout << "通过索引的次数为:" << countWithIndex << endl;
    cout << "不通过索引的次数为:" << countWithoutIndex << endl;
    cout << "次数之差为:" << countWithoutIndex - countWithIndex << endl;
}

int searchWithIndex(double _x1, double _y1, double _z1, double _w1) {
    int count = 0;
    cout << "请输入所要查找的Index值:" << endl;
    double index;
    cin >> index;
    vector<string> filenames = DataMap[index];
    for (auto filename : filenames) {
        ifstream fp("./" + filename); // 定义声明一个ifstream对象，指定文件路径
        string line;
        getline(fp, line); // 跳过列名，第一行不做处理
        while (getline(fp, line)) { // 循环读取每行数据
            count++;
            string number;
            istringstream readstr(line); // string数据流化
            // 将一行数据按'，'分割
            // 可根据数据的实际情况取循环获取
            getline(readstr, number, ','); // 读取X数据
            double _x = ((number != "") ? stod(number) : 0);
            if (_x != _x1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _y = ((number != "") ? stod(number) : 0);
            if (_y != _y1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _z = ((number != "") ? stod(number) : 0);
            if (_z != _z1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _w = ((number != "") ? stod(number) : 0);
            if (_w != _w1) {
                continue;
            }
            return count;
        }
    }
    return -1;
}

int searchWithoutIndex(double _x1, double _y1, double _z1, double _w1) {
    int count = 0;
    vector<string> filenames;
    string suffix = "csv";
    string path = "./";
    getDataFiles(path, filenames, suffix);
    for (auto filename : filenames) {
        ifstream fp("./" + filename); // 定义声明一个ifstream对象，指定文件路径
        string line;
        getline(fp, line); // 跳过列名，第一行不做处理
        while (getline(fp, line)) { // 循环读取每行数据
            count++;
            string number;
            istringstream readstr(line); // string数据流化
            // 将一行数据按'，'分割
            // 可根据数据的实际情况取循环获取
            getline(readstr, number, ','); // 读取X数据
            double _x = ((number != "") ? stod(number) : 0);
            if (_x != _x1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _y = ((number != "") ? stod(number) : 0);
            if (_y != _y1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _z = ((number != "") ? stod(number) : 0);
            if (_z != _z1) {
                continue;
            }
            getline(readstr, number, ','); // 读取X数据
            double _w = ((number != "") ? stod(number) : 0);
            if (_w != _w1) {
                continue;
            }
            return count;
        }
    }
    return -1;
}

/// 通过index更新数值
bool selectAndUpdateByIndex(Data d1, Data d2) {
    double index = d1.getX() + d1.getY() + d1.getZ() + d1.getW();
    vector<string> filenames;
    auto iter = DataMap.find(index);
    if (iter != DataMap.end()) {
        filenames = iter->second;
    }
    else {
        string suffix = "csv";
        string path = "./";
        getDataFiles(path, filenames, suffix);
    }
    vector<Data> incsv;

    for (auto filename : filenames) {
        ifstream fp("./" + filename);
        string line;
        getline(fp, line);
        while (getline(fp, line)) {
            double x1, y1, z1, w1;
            string number;
            istringstream readstr(line); // string数据流化
            getline(readstr, number, ',');
            x1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            y1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            z1 = ((number != "") ? stod(number) : 0);
            getline(readstr, number, ',');
            w1 = ((number != "") ? stod(number) : 0);
            if (x1 == d1.X && y1 == d1.Y && z1 == d1.Z && w1 == d1.W) {
                x1 = d2.X;
                y1 = d2.Y;
                z1 = d2.Z;
                w1 = d2.W;
            }
            incsv.push_back(Data(x1, y1, z1, w1));
        }
        fp.close();
        string time = getTime();
        std::stringstream ss;
        ss << std::fixed << std::setprecision(16);
        ss << d1.X << "," << d1.Y << "," << d1.Z << "," << d1.W;
        ss << "," << d2.X << "," << d2.Y << "," << d2.Z << "," << d2.W;
        string content = ss.str();
        RecordData rd = RecordData(0, time, filename, "w", content);
        createLogItem(rd);
        writeBackToCSV(incsv, filename);
    }
    return true;
}

void writeBackToCSV(vector<Data> d, string filename) {
    ofstream fp("./" + filename);
    fp << "X,Y,Z,W" << endl;
    for (auto d1 : d) {
        fp << fixed << setprecision(16) << d1.X << "," << d1.Y << "," << d1.Z << "," << d1.W << endl;
    }
    fp.close();
}

vector<string> split(const string &str, const string &delim) { // 将分割后的子字符串存储在vector中
    vector<string> res;
    if ("" == str)
        return res;

    string strs = str + delim; //*****扩展字符串以方便检索最后一个分隔出的字符串
    size_t pos;
    size_t size = strs.size();

    for (int i = 0; i < size; ++i) {
        pos = strs.find(delim, i); // pos为分隔符第一次出现的位置，从i到pos之前的字符串是分隔出来的字符串
        if (pos < size) {                                       // 如果查找到，如果没有查找到分隔符，pos为string::npos
            string s = strs.substr(i, pos - i); //*****从i开始长度为pos-i的子字符串
            res.push_back(s);                   // 两个连续空格之间切割出的字符串为空字符串，这里没有判断s是否为空，所以最后的结果中有空字符的输出，
            i = pos + delim.size() - 1;
        }
    }
    return res;
}

double Data::getX() const {
    return X;
}

void Data::setX(double x) {
    X = x;
}

double Data::getY() const {
    return Y;
}

void Data::setY(double y) {
    Y = y;
}

double Data::getZ() const {
    return Z;
}

void Data::setZ(double z) {
    Z = z;
}

double Data::getW() const {
    return W;
}

void Data::setW(double w) {
    W = w;
}

RecordData::RecordData(int id, const string &time, const string &filename, const string &op, const string &content)
    : id(id), time(time), filename(filename), OP(op), content(content) {}

RecordData::RecordData() {}

string getTime() {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return tmp;
}
