#ifndef DFA_H
#define DFA_H
#include<string>
#include<stack>
#include<iostream>
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<queue>
#include<stack>
#include<Windows.h>
//#include"mainwindow.h"
#include<QPixmap>
#include<QImage>
#include <QMainWindow>
#include<QString>
#include<QMessageBox>
#include"dfa.h"
#include<QProcess>
#include<QDebug>
#include<QFile>
using std::stack;
using std::queue;
//dot -Tpng test.dot -o tset.png
using std::string;
using std::cout;
using std::endl;
using std::vector;
using namespace std;
class State
{
public:
    State(int a) :Statenum(a) { count++; }
    static int count;
    int Statenum;
    bool operator<(const State& right) const
    {
        return this->Statenum < right.Statenum;
    }
};
class Node
{
public:
    int i;
    int j;
    char s;
    Node(int a, int b, char c)
    {
        i = a;
        j = b;
        s = c;
    }
};

class DFA
{
public:
    DFA(string regex);//根据正则表达式(字符串)做一个DFA
    void getCharSet();//得到字母表
    void getAddedRegex();//得到添加了与运算符的正则表达式
    void getPostfix_Expression();//得到后缀表达式
    bool Compare(char a, char b);//判断优先级，如果a>b返回true;否则会返回false；
private:
    string Regex;//原始的正则表达式
    string AddedDotRegex;//添加了与运算符的正则表达式
    string CharSet;//字母表
    string Postfix_Expression;//后缀表达式
    const string SpecialCharSet = "|*()";//特殊字符集
    stack<char> CharStack;//一个字符的栈
    const char Dot = '.';//与运算符
    //前半部分主要是得到了后缀表达式,后半部分得到DFA
public:
    DFA(char a,string Charset,string Post);
    DFA();
    DFA(const DFA& Right);
    void operator =(const DFA& Right);
    State * Start;//开始状态
    vector<State> EndSet;//接受状态的集合
    vector<string> Edge;//边
    vector<State> StateSet;//NFA状态集合
    void getStatset();//得到NFA的状态，根据的是后缀表达式
    DFA& OpOr(DFA& Right);//对应于NFA对象之间的|运算
    DFA& OpJi(DFA& Right);//链接运算
    void OpXi();//星运算
    void show();
    //得到了所有的状态和边的存储形式是：状态存在StateSet中，所有的边存在Edge中
    //下面从NFA得到DFA
public:
    vector<string> DFATmpState;//每个string一个DFA状态
    vector<string> DFA_State;//NFA状态是一个字符，那么DFA状态就是string
    string Closure(State& a);//得到一个状态a的闭包
    string Move(string str,char a); //得到状态str经过边a之后的状态
    void getDFA_State();//得到的是DFA中的状态DFA_State;
    int count;//在getDFA_State()随时记录数组(状态)的大小
    vector<Node> Point;//记录起点和转化
    string script1;//绘制图形的脚本
    void getScript();//得到脚本
    string toString(int);//将整数转化成string
    void OutPutDotFile();//写入一个dot文件
    void getPng();//得到图像

    //最后的部分是对输入的字符串的判断
    bool Judge(string);//输入字符串进行比较
    bool *Flag;//标记是不是终结状态
    void getFlag();//获得所有的标记

};
#endif // DFA_H
