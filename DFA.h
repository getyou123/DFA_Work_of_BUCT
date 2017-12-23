#pragma once
#include<string>
#include<stack>
#include<iostream>
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<queue>
#include<stack>
#include<fstream>
#include<Windows.h>
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
class Node//���Ƕ�һ���ߵ���������
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
	DFA(string regex);//����������ʽ(�ַ���)��һ��DFA
	void getCharSet();//�õ���ĸ��
	void getAddedRegex();//�õ���������������������ʽ
	void getPostfix_Expression();//�õ���׺���ʽ
	bool Compare(char a, char b);//�ж����ȼ������a>b����true;����᷵��false��
private:
	string Regex;//ԭʼ��������ʽ
	string AddedDotRegex;//��������������������ʽ
	string CharSet;//��ĸ��
	string Postfix_Expression;//��׺���ʽ
	const string SpecialCharSet = "|*()";//�����ַ���
	stack<char> CharStack;//һ���ַ���ջ
	const char Dot = '.';//�������
	//ǰ�벿����Ҫ�ǵõ��˺�׺���ʽ,��벿�ֵõ�DFA
public:
	DFA(char a,string Charset,string Post);
	DFA();
	DFA(const DFA& Right);
	void operator =(const DFA& Right);
	State * Start;//��ʼ״̬
	vector<State> EndSet;//����״̬�ļ���
	vector<string> Edge;//��
	vector<State> StateSet;//NFA״̬����
	void getStatset();//�õ�NFA��״̬�����ݵ��Ǻ�׺���ʽ
	DFA& OpOr(DFA& Right);//��Ӧ��NFA����֮���|����
	DFA& OpJi(DFA& Right);//��������
	void OpXi();//������
	void show();
	//�õ������е�״̬�ͱߵĴ洢��ʽ�ǣ�״̬����StateSet�У����еıߴ���Edge��
	//�����NFA�õ�DFA
public:
	vector<string> DFATmpState;//ÿ��stringһ��DFA״̬
	vector<string> DFA_State;//NFA״̬��һ���ַ�����ôDFA״̬����string
	string Closure(State& a);//�õ�һ��״̬a�ıհ�
	string Move(string str,char a); //�õ�״̬str������a֮���״̬
	void getDFA_State();//�õ�����DFA�е�״̬DFA_State;
	int count;//��getDFA_State()��ʱ��¼����(״̬)�Ĵ�С
	vector<Node> Point;//��¼����ת��
	string script1;//����ͼ�εĽű�
	void getScript();//�õ��ű�
	string toString(int);//������ת����string
	ofstream fout;//�ļ�����
	void OutPutDotFile();//д��һ��dot�ļ�
	void getPng();//�õ�ͼ��

	//���Ĳ����Ƕ�������ַ������ж�
	bool Judge(string);//�����ַ������бȽ�
	bool *Flag;//����ǲ����ս�״̬
	void getFlag();//������еı��

	int section_all;//���е�section�ļ���
	int* section_num;//��¼ÿ���ڵ������
	void Min();//ʵ����С��
	void cut(vector<int> V);//�ݹ�Ľ��л���
	bool is_in_same_sec(int a, int b);//�ж�a,b�Ƿ�����ͬһ��section��
	vector<Node> point;//�洢��С��֮��ıߵ�����
	bool *flag_1;//�����ŵı�־
}; 