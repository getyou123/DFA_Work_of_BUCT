#include"DFA.h"
int State::count = 0;
DFA::DFA(string regex)
{
	Regex = regex;
	AddedDotRegex = "";
	CharSet = "";
	Postfix_Expression = "";

	getCharSet();
	
	getAddedRegex();
	try {
		getPostfix_Expression();
	}
	catch (char)
	{
		throw 'E';
	}
	for (int i = 0; i < Postfix_Expression.size(); ++i)
	{
		if (Postfix_Expression[i] == '(' || Postfix_Expression[i] == ')')throw 'E';
	}
	/*cout << "原始的正则表达式：" << Regex << endl;
	cout << "添加Dot之后的正则表达式：" << AddedDotRegex << endl;*/
	
	getStatset();
	getDFA_State();

	getFlag();

	//最小化dfa
	Min();
	cout << endl << section_all << endl;
	for (const Node& n : point)
	{
		cout << n.i << " " << n.j << " "<<n.s << endl;
	}
	for (int i = 0; i < section_all; i++)
	{
		cout << flag_1[i] << " ";
	}
	cout << endl;
	getScript();
	cout << script1 << endl;
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
				if(AddedDotRegex[i]==')')throw 'E';
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
					if (CharStack.empty())throw 'E';//上来就是一个空
					while (!CharStack.empty()&&CharStack.top()!= '(')
					{
						Postfix_Expression += CharStack.top();
						CharStack.pop();
					}
					if (CharStack.empty())throw 'E';//没有与‘）“对应的（
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
		if(StateTmp.find(char(Q.front().Statenum + '0'))==string::npos)StateTmp += char(Q.front().Statenum + '0');
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
	string emp = "";
	for (int j = 0; j < res.size(); ++j) { if (j == 0 || res[j] != res[j - 1])emp += res[j]; }
	return emp;
}

void DFA::getDFA_State()//得到的是DFA中的状态DFA_State;
{
	queue<string> Q;
	string Sorttmp = Closure(*Start);
	sort(Sorttmp.begin(), Sorttmp.end());
	Q.push(Sorttmp);
	DFA_State.push_back(Sorttmp);
	count = 0;
	while (!Q.empty())
	{
		/*Sorttmp = Q.front();
		sort(Sorttmp.begin(), Sorttmp.end());
		int k = 0;
		for (k = 0; k < DFA_State.size(); k++) { 
			if (DFA_State[k] == Sorttmp)break;
		}
		if (k == DFA_State.size())
		{
			DFA_State.push_back(Sorttmp);
		}*/
		/*for (int j = 0; j < DFA_State.back().size(); ++j)
		{
			cout << int(DFA_State.back()[j] - '0') << " ";
		}
		cout << DFA_State.back().size() << endl;*/
		int len = CharSet.size();
		for (int i = 0; i < len; ++i)
		{
			string NState = Move(Q.front(), CharSet[i]);
			int kstart = 0;
			for (kstart=0; kstart < DFA_State.size(); kstart++)
			{
				if (Q.front() == DFA_State[kstart])break;
			}
			int flag = -1;
			for (int i= 0; i < DFA_State.size(); ++i)
			{
				if (DFA_State[i] == NState)flag = i;//旧的状态
			}
			if (NState != ""&&flag == -1)
			{
				Q.push(NState);
				count++;
				Point.push_back(Node(kstart, count, CharSet[i]));
				DFA_State.push_back(NState);		
			}
			else
			{
				if(flag!=-1)
				Point.push_back(Node(kstart,flag, CharSet[i]));
			}
		}
		Q.pop();
	}
	for (const Node& a : Point)
	{
		cout << a.i << " " << a.j << " " << a.s << endl;
	}
}

void DFA::getScript()//得到绘制的脚本
{
	script1 = "";
	script1 += "digraph abc{\n";
	for (int i = 0; i <section_all; ++i)
	{
		script1 += toString(i);
		script1 += ";\n";
	}
	script1 += "\n0 [style = filled, color = lightgrey];\n";
	for (int i = 1; i <section_all; ++i)
	{
		if (flag_1[i] == true)
		{
			script1 += toString(i);
			script1 += " [shape = doublecircle];\n";
		}
	}
	script1 += "\n";
	
	for (int i = 0; i < point.size(); ++i)
	{
		script1 += toString(point[i].i);
		script1 += " -> ";
		script1 += toString(point[i].j );
		script1 += " [label = \"";
		script1 += point[i].s;
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
	//void DFA::OutPutDotFile()//д��һ��dot�ļ�
	//{
	//	QString lines = QString::fromStdString(script1);
	//	QFile Qf("DFA.dot");
	//	if (Qf.open(QIODevice::ReadWrite | QIODevice::Text))
	//	{
	//		QTextStream stream(&Qf);
	//		stream << lines;
	//	}
	//	Qf.close();
	//}


void DFA::getPng()
{
	WinExec("cmd.exe /c dot -Tpng \"E:\\QtProject\\NFAProject\\DFA.dot\" -o \"E:\\QtProject\\NFAProject\\DFA.png\"", SW_HIDE);
}
bool DFA::Judge(string str)//对输入的串进行判断
{
	int start = 0;
	int i = 0;
	for (i = 0; i < str.size(); i++)
	{
		if (CharSet.find(str[i]) == string::npos)return false;//如果不在字母表中直接返回false
		bool j = false;//标志是不是走了一步
		for (const Node& a : Point)
		{
			if (str[i] == a.s&&start== a.i)
			{
				start= a.j;
				j = true;
				break;
			 }
		}
		if (j == false)break;
	}
	if (Flag[start] == true&&i==str.size())
	{
		return true;
	}
	else return false;
}

void DFA::getFlag()//获得所有的标记
{
	//cout <<"Endwith:"<< EndSet[0].Statenum << endl;
	int len = DFA_State.size();
	string s1 = "";
	s1 = s1 + char(EndSet[0].Statenum+'0');
	Flag = new bool(len);
	for (int i = 0; i < len; ++i)Flag[i] = false;//默认是false
	for (int i = 0; i < len; ++i)
	{
		if (DFA_State[i].find(s1) != string::npos)Flag[i] = true;//包含中介的是true
	}
}

void DFA::Min()//实现最小化
{
	int DFA_num = DFA_State.size();//记录所有的dfa节点的总数
	section_num = new int[DFA_num];//
	for (int i = 0; i < DFA_num; i++)
	{
		if (Flag[i] == false)
		{
			section_num[i] = 0;
		}
		else
		{
			section_num[i] = 1;
		}
	}//完成对section_num的初始化
	section_all = 2;//目前来说的section总数
	vector<int> s;
	vector<int> t;
	for (int i = 0; i < DFA_num; i++)
	{
		if (Flag[i])s.push_back(i);
		else t.push_back(i);
	}
	cut(t);
	cut(s);
	for (int i = 0; i < DFA_num; i++)
	{
		cout << i << ":" << section_num[i] << "  ";
	}

	for (int i = 0; i < section_all; i++)
	{
		for (int j = 0; j < DFA_num; j++)
		{
			if (section_num[j] == i)//当前区号，j是区号i中的一个节点
			{
				for (const Node& n : Point)
				{
					if (n.i == j)
					{
						point.push_back(Node(i, section_num[n.j], n.s));
					}
				}
				break;
			}
		}
	}

	flag_1 = new bool[section_all];
	for (int i = 0; i < section_all; i++)
	{
		flag_1[i] = false;
	}
	for (int i = 0; i < DFA_State.size(); i++)
	{
		if (Flag[i]) { flag_1[section_num[i]] = true; }
	}
}

void DFA::cut(vector<int> V)//递归的进行划分
{
	if (V.size() <= 1)return;
	vector<vector<int> > Vall;
	vector<int> tmp;
	tmp.push_back(V[0]);
	Vall.push_back(tmp);
	for (int i = 1; i < V.size(); i++)
	{
		int j = 0;
		for (j = 0; j < Vall.size(); j++)
		{
			if (is_in_same_sec(V[i], Vall[j][0]))//在同一个区域
			{
				break;
			}
		}
		if (j < Vall.size()) // 旧的区域中的
		{
			Vall[j].push_back(V[i]);
			section_num[V[i]] = section_num[Vall[j][0]];
		}
		else//一个新的区域 
		{
			tmp.clear();
			tmp.push_back(V[i]);
			Vall.push_back(tmp);
			section_num[V[i]] = section_all++;
		}
	}
	if (Vall.size() == 1)return ;
	for (int i = 0; i < Vall.size(); i++)
	{
		cut(Vall[i]);
	}
}


bool DFA::is_in_same_sec(int a, int b)//判断a,b是否是在同一个section中
{
	string tmpa, tmpb;
	for (int k = 0; k < CharSet.size(); k++)
	{
		for (const Node& n: Point)
		{
			if (n.i == a&&n.s == CharSet[k])tmpa += char(n.j + '0');
			if (n.i == b&&n.s == CharSet[k])tmpb += char(n.j + '0');
		}
	}
	if (tmpa.size() == tmpb.size())
	{
		for (int i = 0; i < tmpa.size(); i++)
		{
			if (section_num[int(tmpa[i]-'0')] != section_num[int(tmpb[i]-'0')])return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}