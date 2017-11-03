#include"dfa.h"
int State::count = 0;
DFA::DFA(string regex)
{
    Regex = regex;
    AddedDotRegex = "";
    CharSet = "";
    Postfix_Expression = "";

    getCharSet();

    getAddedRegex();
    getPostfix_Expression();
    /*cout << "原始的正则表达式：" << Regex << endl;
    cout << "添加Dot之后的正则表达式：" << AddedDotRegex << endl;*/

    getStatset();
    /*cout << "后缀表达式：" << Postfix_Expression << endl;*/
    getDFA_State();
    getFlag();
    getScript();
    cout << script1 << endl;
    OutPutDotFile();
    //show();
    getPng();
}

void DFA::getCharSet()//得到字母表
{
    int len = Regex.size();
    for (int i = 0; i < len; ++i)
    {
        if (SpecialCharSet.find(Regex[i])==string::npos&&CharSet.find(Regex[i])==string::npos)//不是特殊字符就是字母表中的一个
        {
          CharSet+= Regex[i];
        }
    }

}

void DFA::getAddedRegex()//得到添加了与运算符的正则表达式
{
    AddedDotRegex = Regex;
    auto iter = AddedDotRegex.begin();
    iter++;
    while (iter != AddedDotRegex.end())
    {
        if((CharSet.find(*(iter - 1)) != string::npos&&CharSet.find(*iter) != string::npos)||(*(iter - 1)=='*'&&CharSet.find(*iter) != string::npos)||(*(iter - 1) == ')'&&CharSet.find(*iter) != string::npos)||(*(iter-1)=='*'&&*iter=='(')||(*(iter-1)==')'&&*iter=='(')||(CharSet.find(*(iter-1))!=string::npos&&*iter=='('))//两个字母表中的字母中间是与运算符,*与字母之间，*与（，）和字母之间也是与运算符；
        {
            iter=AddedDotRegex.insert(iter, Dot);
        }
        iter++;
    }
}

bool DFA::Compare(char a, char b)//验证字符a的优先级是不是高于b的
{
    if ((a == '*')||(a==Dot&&b=='|'))return true;
    else return false;
}

void DFA::getPostfix_Expression()//得到后缀表达式
{
    int len = AddedDotRegex.size();
    while (!CharStack.empty())//清空操作
    {
        CharStack.pop();
    }
    for (int i = 0; i < len; ++i)
    {
        if (CharSet.find(AddedDotRegex[i]) != string::npos)//是一个字母
        {
            Postfix_Expression += AddedDotRegex[i];
        }
        else
        {
            if (CharStack.empty())//栈空直接入栈
            {
                CharStack.push(AddedDotRegex[i]);
            }
            else
            {
                if (AddedDotRegex[i] == '(')//左括号直接入栈
                {
                    CharStack.push(AddedDotRegex[i]);
                }
                else if (AddedDotRegex[i] == ')')//右括号的情况
                {
                    while (CharStack.top()!= '(')
                    {
                        Postfix_Expression += CharStack.top();
                        CharStack.pop();
                    }
                    CharStack.pop();
                }
                else//其他的特殊字符的情况；
                {
                    if (CharStack.top() == '(')CharStack.push(AddedDotRegex[i]);
                    //比较优先级放入这个Add【i】
                    else if (Compare(AddedDotRegex[i], CharStack.top()))//新字符的优先级高
                    {
                        CharStack.push(AddedDotRegex[i]);
                    }
                    else if (!Compare(AddedDotRegex[i], CharStack.top()))//栈内的优先级高
                    {
                        while (!Compare(AddedDotRegex[i], CharStack.top()))
                        {
                            Postfix_Expression += CharStack.top();
                            CharStack.pop();
                            if (CharStack.empty())break;
                            else if (CharStack.top() == '(')break;
                        }
                        CharStack.push(AddedDotRegex[i]);
                    }
                }
            }
        }
    }
    while (!CharStack.empty())
    {
        Postfix_Expression += CharStack.top();
        CharStack.pop();
    }
}

//前半部分主要是得到了后缀表达式,后半部分得到DFA
DFA::DFA(char a,string Charset,string Post)
{
    this->CharSet = Charset;
    this->Postfix_Expression = Post;
    Start = new State(State::count);
    State * End1 = new State(State::count);
    string tmp = "";
    tmp += char(Start->Statenum + '0');
    tmp += a;
    tmp += char(End1->Statenum + '0');
    Edge.push_back(tmp);
    StateSet.push_back(*Start);
    StateSet.push_back(*End1);
    EndSet.push_back(*End1);
}

DFA::DFA()
{

}

DFA::DFA(const DFA& Right)
{
    //*this = Right;
    this->Start=Right.Start;//开始状态
    EndSet=Right.EndSet;//接受状态的集合
    Edge=Right.Edge;//边
    StateSet=Right.StateSet;//状态集合
    CharSet = Right.CharSet;
    Postfix_Expression = Right.Postfix_Expression;
}

DFA& DFA::OpOr(DFA& Right)//对应于NFA对象之间的|运算
{
    DFA *tmp = new DFA();
    tmp->Start = new State(State::count);
    State* End1 = new State(State::count);
    tmp->StateSet.push_back(*(tmp->Start));
    tmp->StateSet.push_back(*End1);
    tmp->EndSet.push_back(*End1);
    //得到所有的操作数的点
    while (!this->StateSet.empty())
    {
        tmp->StateSet.push_back(this->StateSet.back());
        this->StateSet.pop_back();
    }
    while (!Right.StateSet.empty())
    {
        tmp->StateSet.push_back(Right.StateSet.back());
        Right.StateSet.pop_back();
    }
    //获得所有操作数的边
    while (!this->Edge.empty())
    {
        tmp->Edge.push_back(this->Edge.back());
        this->Edge.pop_back();
    }
    while (!Right.Edge.empty())
    {
        tmp->Edge.push_back(Right.Edge.back());
        Right.Edge.pop_back();
    }
    //新添加到tmp中的边
    {
        string strtmp = "";
        strtmp += char(tmp->Start->Statenum + '0');
        strtmp += "_";
        strtmp += char(this->Start->Statenum + '0');
        tmp->Edge.push_back(strtmp);

        strtmp = "";
        strtmp += char(tmp->Start->Statenum + '0');
        strtmp += "_";
        strtmp += char(Right.Start->Statenum + '0');
        tmp->Edge.push_back(strtmp);
        //完成了头部的交接

        for (const State& a : this->EndSet)
        {
            for (const State& b : tmp->EndSet)
            {
                strtmp = "";
                strtmp += char(a.Statenum + '0');
                strtmp += "_";
                strtmp += char(b.Statenum + '0');
                tmp->Edge.push_back(strtmp);
            }
        }

        for (const State& a : Right.EndSet)
        {
            for (const State& b : tmp->EndSet)
            {
                strtmp = "";
                strtmp += char(a.Statenum + '0');
                strtmp += "_";
                strtmp += char(b.Statenum + '0');
                tmp->Edge.push_back(strtmp);
            }
        }
    }
    tmp->CharSet = this->CharSet;
    return *tmp;
}

DFA& DFA::OpJi(DFA& Right)
{

    DFA *tmp = new DFA();
    tmp->Start = this->Start;
    tmp->CharSet = this->CharSet;
    tmp->EndSet = Right.EndSet;
    tmp->Postfix_Expression = this->Postfix_Expression;
    for (string& a : this->Edge)
    {
        if (a[2] == char(this->EndSet[0].Statenum + '0'))
        {
            a[2] = char(Right.Start->Statenum + '0');
        }
    }
    //得到所有的操作数的点
    while (!this->StateSet.empty())
    {
        if (this->StateSet.back().Statenum != this->EndSet[0].Statenum)
            tmp->StateSet.push_back(this->StateSet.back());
        this->StateSet.pop_back();
    }
    while (!Right.StateSet.empty())
    {
        tmp->StateSet.push_back(Right.StateSet.back());
        Right.StateSet.pop_back();
    }
    //获得所有操作数的边
    while (!this->Edge.empty())
    {
        tmp->Edge.push_back(this->Edge.back());
        this->Edge.pop_back();
    }
    while (!Right.Edge.empty())
    {
        tmp->Edge.push_back(Right.Edge.back());
        Right.Edge.pop_back();
    }
    return *tmp;
}

void DFA::OpXi()
{
    State* NStart = new State(State::count);
    State* NEnd = new State(State::count);
    string strtmp = "";
    strtmp += char(NStart->Statenum + '0');
    strtmp += '_';
    strtmp += char(this->Start->Statenum + '0');
    this->Edge.push_back(strtmp);

    strtmp = "";
    strtmp += char(NStart->Statenum + '0');
    strtmp += '_';
    strtmp += char(NEnd->Statenum + '0');
    this->Edge.push_back(strtmp);

    for (const State& a : this->EndSet)
    {
        string strtmp = "";
        strtmp += char(a.Statenum + '0');
        strtmp += '_';
        strtmp += char(this->Start->Statenum + '0');
        this->Edge.push_back(strtmp);
    }

    for (const State& a : this->EndSet)
    {
        string strtmp = "";
        strtmp += char(a.Statenum + '0');
        strtmp += '_';
        strtmp += char(NEnd->Statenum + '0');
        this->Edge.push_back(strtmp);
    }

    while (!this->EndSet.empty())
    {
        EndSet.pop_back();
    }
    this->Start = NStart;
    this->EndSet.push_back(*NEnd);
    this->StateSet.push_back(*NEnd);
    this->StateSet.push_back(*NStart);
}

void DFA::getStatset()//得到NFA的状态，根据的是后缀表达式
{
    stack<DFA> Stack;
    int len = Postfix_Expression.size();
    for (int i = 0; i < len; ++i)
    {
        if (CharSet.find(Postfix_Expression[i])!=string::npos)//是字母表中的一个字母
        {
            DFA r(Postfix_Expression[i],this->CharSet,this->Postfix_Expression);
            Stack.push(r);
        }
        else if (Postfix_Expression[i] == '|')
        {
            DFA *tmp1 = new DFA();
            *tmp1= Stack.top();
            Stack.pop();
            DFA *tmp2 = new DFA();
            *tmp2=Stack.top();
            Stack.pop();
            Stack.push(tmp2->OpOr(*tmp1));
        }
        else if (Postfix_Expression[i] == '.')
        {
            DFA *tmp1 = new DFA();
            *tmp1 = Stack.top();
            Stack.pop();
            DFA *tmp2 = new DFA();
            *tmp2 = Stack.top();
            Stack.pop();
            Stack.push(tmp2->OpJi(*tmp1));
        }
        else if (Postfix_Expression[i] == '*')
        {
            DFA *tmp = new DFA();
            *tmp=Stack.top();
            Stack.pop();
            tmp->OpXi();
            Stack.push(*tmp);
        }
    }

    *this = Stack.top();
}

void DFA::show()
{
    /*cout << "StateSet:" << endl;
    for (const State& a : StateSet)
    {
        cout << "S" << a.Statenum << " ";
    }
    cout << endl;
    cout << "Edge:" << endl;
    for (const string& str : Edge)
    {
        cout << str[0] - '0' << str[1] << str[2] - '0' << " ";
    }
    cout << endl;*/
    /*for (const Node& a : Point)
    {
        cout << a.i<< " "<<a.j<<" "<<a.s << endl;;
    }
    cout << endl;*/
    cout << script1 << endl;
}

void DFA::operator =(const DFA& Right)
{
    this->Start = Right.Start;//开始状态
    EndSet = Right.EndSet;//接受状态的集合
    Edge = Right.Edge;//边
    StateSet = Right.StateSet;//状态集合
    CharSet = Right.CharSet;
    Postfix_Expression = Right.Postfix_Expression;
}

string DFA::Closure(State& a)//得到闭包
{
    //dfa状态是NFA的状态的集合，NFA状态用的是一位char表示，于是可以使用stribng来表示dfa的状态
    string StateTmp = "";
    queue<State> Q;
    Q.push(a);
    while (!Q.empty())
    {
        StateTmp += char(Q.front().Statenum + '0');
        for (auto iter = Edge.begin(); iter != Edge.end(); ++iter)
        {
            if ((*iter).at(0) == char(Q.front().Statenum + '0') && (*iter).at(1) == '_')
            {
                Q.push(State(int((*iter).at(2) - '0')));
            }
        }
        Q.pop();
    }
    return StateTmp;
}

string DFA::Move(string str, char a)//得到状态str经过边a之后的状态
{
    string res = "";
    for (int i = 0; i < str.size(); ++i)
    {
        for (const string E : Edge)
        {
            if (E.at(0) == str[i] && E.at(1) == a)
            {
                State tmp(int(E.at(2) - '0'));
                res += Closure(tmp);
            }
        }
    }
    sort(res.begin(), res.end());
    return res;
}

void DFA::getDFA_State()//得到的是DFA中的状态DFA_State;
{
    queue<string> Q;
    Q.push(Closure(*Start));
    count = 0;
    while (!Q.empty())
    {
        int len = CharSet.size();
        DFA_State.push_back(Q.front());
        for (int i = 0; i < len; ++i)
        {
            string NState = Move(Q.front(), CharSet[i]);
            int flag = -1;
            for (int i= 0; i < DFA_State.size(); ++i)
            {
                if (DFA_State[i] == NState)flag = i;
            }
            if (NState != ""&&flag == -1)
            {
                Q.push(NState);
                count++;
                Point.push_back(Node(DFA_State.size()-1, count, CharSet[i]));
            }
            else
            {
                if(flag!=-1)
                Point.push_back(Node(DFA_State.size()- 1,flag, CharSet[i]));
            }
        }
        Q.pop();
    }
    //for (const Node& a : Point)
    //{
    //	cout << a.i << " " << a.j << " " << a.s << endl;
    //}
}

void DFA::getScript()//得到绘制的脚本
{
    script1 = "";
    script1 += "digraph abc{\n";
    for (int i = 0; i < DFA_State.size(); ++i)
    {
        script1 += toString(i);
        script1 += ";\n";
    }
    script1 += "\n0 [style = filled, color = lightgrey];\n";
    for (int i = 1; i <DFA_State.size(); ++i)
        {
            if (Flag[i] == false)
            {
                script1 += toString(i);
                script1 += " [shape = doublecircle];\n";
            }
        }
        script1 += "\n";
    for (int i = 0; i < Point.size(); ++i)
    {
        script1 += toString(Point[i].i);
        script1 += " -> ";
        script1 += toString(Point[i].j );
        script1 += " [label = \"";
        script1 += Point[i].s;
        script1 += "\" ];\n";
        //script1 += "\n";
    }
    script1 = script1 + "\n}";
    /*script1 = script1 + Regex + ";\n}";*/
}

string DFA::toString(int a)
{
    if (a == 0)return "0";
    string tmp = "";
    while (a)
    {
        tmp += char(a % 10 + '0');
        a = a / 10;
    }
    reverse(tmp.begin(), tmp.end());
    return tmp;
}

//void DFA::OutPutDotFile()//写入一个dot文件
//{
//	fout.open("E:\QtProject\NFAProject\DFA.dot");
//	if (!fout.is_open())
//	{
//		cout << "打开文件失败" << endl;
//	}
//	fout << script1;
//}
    void DFA::OutPutDotFile()//д��һ��dot�ļ�
    {
        QString lines = QString::fromStdString(script1);
        QFile Qf("DFA.dot");
        if (Qf.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QTextStream stream(&Qf);
            stream << lines;
        }
        Qf.close();
    }


void DFA::getPng()
{
    WinExec("cmd.exe /c dot -Tpng \"E:\\QtProject\\NFAProject\\DFA.dot\" -o \"E:\\QtProject\\NFAProject\\DFA.png\"", SW_HIDE);
}
bool DFA::Judge(string str)
{
    int start = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (CharSet.find(str[i]) == string::npos)return false;
        for (const Node& a : Point)
        {
            if (str[i] == a.s&&start== a.i)
            {
                start= a.j;
                break;
             }
        }
    }
    if (Flag[start] == false)
    {
        return true;
    }
    else return false;
}
void DFA::getFlag()//获得所有的标记
{
    int len = DFA_State.size();
    Flag = new bool(len);
    for (int i = 0; i < len; ++i)Flag[i] = false;
    for (const Node& a : Point)
        {
        Flag[a.i] = true;
        }
}
