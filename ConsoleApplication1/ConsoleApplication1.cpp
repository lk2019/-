// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include"pch.h"
#include<iostream>
#include<cstring>
#include<string>
#include<sstream>
#include<stdlib.h>
#include<stack>
#include<vector>
#include<set>
#include<queue>

#define MAX 128

using namespace std;

typedef set<int> IntSet;
typedef set<char> CharSet;


struct NfaState				
{

	int key;				
	char data;				
	int chTrans;		//转移到的状态号
	IntSet epTrans;			//空所到的状态集合
};

struct NFA
{

	NfaState *head;			
	NfaState *last;			
};

NfaState NfaStates[MAX];	
int nfaStateNum = 0;	

//--------------------------
struct Dfas
{
	char key;
	int data;
};
struct Dfa
{
	Dfas dfas[10];
	int data;
	int dfasnum;
	IntSet bibao;
	bool end;
};
Dfa Dfastates[128];
int dfastatesnum = 0;

struct dfanode
{
	int start;
	IntSet end;
	CharSet ends;
	int trans[128][128];
};

void add(NfaState *n1, NfaState *n2, char ch)
{

	n1->data = ch;
	n1->chTrans = n2->key;
}


void add(NfaState *n1, NfaState *n2)
{

	n1->epTrans.insert(n2->key);
}


NFA creatNFA(int sum)
{

	NFA n;

	n.head = &NfaStates[sum];
	n.last = &NfaStates[sum + 1];

	return n;
}

//在字符后面增加符号
void insert(string &s, int n, char ch)
{

	s += '#';

	for (int i = s.size() - 1; i > n; i--)
	{
		s[i] = s[i - 1];
	}

	s[n] = ch;
}

//增加连接符.
void preprocess(string &s)
{

	int i = 0, length = s.size();

	while (i < length)
	{
		if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] == '*') || (s[i] == ')'))
		{
			if ((s[i + 1] >= 'a' && s[i + 1] <= 'z') || s[i + 1] == '(')
			{

				insert(s, i + 1, '.');
				length++;
			}
		}

		i++;
	}
}

//定义优先级
int priority(char ch)
{

	if (ch == '*')
	{
		return 3;
	}

	if (ch == '.')
	{
		return 2;
	}

	if (ch == '|')
	{
		return 1;
	}

	if (ch == '(')
	{
		return 0;
	}
}

/*中缀表达式转后缀表达式*/
string infixToSuffix(string s)
{

	preprocess(s);			

	string str;			
	stack<char> oper;		

	for (int i = 0; i < s.size(); i++)
	{

		if (s[i] >= 'a' && s[i] <= 'z')
		{
			str += s[i];
		}
		else							
		{

			if (s[i] == '(')			
			{
				oper.push(s[i]);
			}

			else if (s[i] == ')')	/*遇到右括号时*/
			{

				char ch = oper.top();
				while (ch != '(')		/*将栈中元素出栈，直到栈顶为左括号*/
				{

					str += ch;

					oper.pop();
					ch = oper.top();
				}

				oper.pop();				/*最后将左括号出栈*/
			}
			else					/*遇到其他操作符时*/
			{

				if (!oper.empty())			/*如果栈不为空*/
				{

					char ch = oper.top();
					while (priority(ch) >= priority(s[i]))	/*弹出栈中优先级大于等于当前运算符的运算符*/
					{

						str += ch;
						oper.pop();

						if (oper.empty())	/*如果栈为空则结束循环*/
						{
							break;
						}
						else ch = oper.top();
					}

					oper.push(s[i]);		/*再将当前运算符入栈*/
				}

				else				/*如果栈为空，直接将运算符入栈*/
				{
					oper.push(s[i]);
				}
			}
		}
	}

	/*最后如果栈不为空，则出栈并输出到字符串*/
	while (!oper.empty())
	{

		char ch = oper.top();
		oper.pop();

		str += ch;
	}

	cout << "中缀表达式为：" << s << endl ;
	cout << "后缀表达式为：" << str << endl ;

	return str;
}

/*后缀表达式转nfa*/
NFA strToNfa(string s)
{

	stack<NFA> NfaStack;		

	for (int i = 0; i < s.size(); i++)		
	{

		if (s[i] >= 'a' && s[i] <= 'z')	
		{

			NFA n = creatNFA(nfaStateNum);		
			nfaStateNum += 2;					

			add(n.head, n.last, s[i]);			

			NfaStack.push(n);					
		}

		else if (s[i] == '*')	
		{

			NFA n1 = creatNFA(nfaStateNum);		
			nfaStateNum += 2;					

			NFA n2 = NfaStack.top();			
			NfaStack.pop();

			add(n2.last, n1.head);				
			add(n2.last, n1.last);				
			add(n1.head, n2.head);				
			add(n1.head, n1.last);				

			NfaStack.push(n1);					
		}

		else if (s[i] == '|')	
		{

			NFA n1, n2;							
			n2 = NfaStack.top();
			NfaStack.pop();

			n1 = NfaStack.top();
			NfaStack.pop();

			NFA n = creatNFA(nfaStateNum);		
			nfaStateNum += 2;					

			add(n.head, n1.head);				
			add(n.head, n2.head);				
			add(n1.last, n.last);				
			add(n2.last, n.last);				

			NfaStack.push(n);					
		}

		else if (s[i] == '.')	
		{

			NFA n1, n2, n;				

			n2 = NfaStack.top();				
			NfaStack.pop();

			n1 = NfaStack.top();
			NfaStack.pop();

			add(n1.last, n2.head);				

			n.head = n1.head;					
			n.last = n2.last;					

			NfaStack.push(n);					
		}
	}

	return NfaStack.top();		
}

CharSet NFA_alphabet;
void printNFA(NFA nfa)
{

	cout << "NFA总共有" << nfaStateNum << "个状态，" << endl;
	cout << "初态为" << nfa.head->key << "号，终态为" << nfa.last->key << "号。" << endl << endl << "NFA路径为：" << endl;

	for (int i = 0; i < nfaStateNum; i++)		
	{
	
		if (NfaStates[i].data != '#')			
		{
			NFA_alphabet.insert(NfaStates[i].data);
			cout << NfaStates[i].key << "-->'" << NfaStates[i].data << "'-->" << NfaStates[i].chTrans;
		}

		IntSet::iterator it;					
		for (it = NfaStates[i].epTrans.begin(); it != NfaStates[i].epTrans.end(); it++)
		{
			cout << NfaStates[i].key << "-->" << "~" << "-->" << *it<<"  " ;
		}
		cout<<endl;
	}

}
bool yfend(NFA n, IntSet s)
{
	IntSet::iterator it;
	for (it = s.begin(); it != s.end(); it++)	
	{
		if (*it == n.head->key)				
		{
			return true;
		}
	}

	return false;
}
IntSet samestate(IntSet s)
{
	stack<int> temp;
	IntSet::iterator it;
	for (it = s.begin(); it != s.end(); it++)
	{
		temp.push(*it);
	}
	while (!temp.empty())
	{
		int i = temp.top();
		temp.pop();
		IntSet::iterator iter;
		for (iter = NfaStates[i].epTrans.begin(); iter != NfaStates[i].epTrans.end(); iter++)
		{

			if (!s.count(*iter))
			{
				s.insert(*iter);
				temp.push(*iter);
			}
		}
	}
	return s;
}

IntSet samelaterstate(char ch,IntSet s)
{
	IntSet temp;
	IntSet::iterator it;
	for (it = s.begin(); it != s.end(); it++)		
	{
		if (NfaStates[*it].data == ch)				
		{
			temp.insert(NfaStates[*it].chTrans);		
		}
	}

	temp = samestate(temp);
	return temp;
}

bool isend(NFA n,IntSet s)
{
	IntSet::iterator it;
	for (it = s.begin(); it != s.end(); it++)
	{
		if (*it == n.last->key)				
		{
			return true;
		}
	}

	return false;
}

dfanode NFAtoDFA(NFA nfa,string houzhui)

{
	set<IntSet> DFAnum;
	dfanode d;
	for (int i = 0; i < houzhui.size(); i++)			/*遍历后缀表达式*/
	{
		if (houzhui[i] >= 'a' && houzhui[i] <= 'z')		/*如果遇到操作数，则把它加入到dfa的终结符集中*/
		{
			d.ends.insert(houzhui[i]);
		}
	}
	IntSet temp;
	d.start = 0;
	temp.insert(nfa.head->key);
	Dfastates[0].bibao = samestate(temp);
	Dfastates[0].end = isend(nfa,Dfastates[0].bibao);
	dfastatesnum++;

	queue<int> q;
	q.push(d.start);
	while (!q.empty())
	{
		int num = q.front();
		q.pop();
		CharSet::iterator ir;
		for (ir = d.ends.begin(); ir != d.ends.end(); ir++)
		{
			IntSet temp = samelaterstate(*ir, Dfastates[num].bibao);
			IntSet::iterator t;
			for(t = temp.begin(); t != temp.end(); t++) 
			{
				cout<<*t<<' ';
			}
			cout<<endl;
			if (!DFAnum.count(temp) && !temp.empty())
			{
				DFAnum.insert(temp);
				Dfastates[dfastatesnum].bibao = temp;

				Dfastates[num].dfas[Dfastates[num].dfasnum].key = *ir;
				Dfastates[num].dfas[Dfastates[num].dfasnum].data = dfastatesnum;
				Dfastates[num].dfasnum++;
				d.trans[num][*ir - 97] = dfastatesnum;
				Dfastates[dfastatesnum].end= isend(nfa,Dfastates[dfastatesnum].bibao);
				q.push(dfastatesnum);
				dfastatesnum++;
			}
			else
			{
				for (int i = 0; i < dfastatesnum; i++)
				{
					if (temp == Dfastates[i].bibao)
					{
						Dfastates[num].dfas[Dfastates[num].dfasnum].key = *ir;
						Dfastates[num].dfas[Dfastates[num].dfasnum].data = i;
						Dfastates[num].dfasnum++;
						d.trans[num][*ir - 97] = i;
						break;
					}
				}
			}
		}
	}
	for (int i = 0; i < dfastatesnum; i++)	
	{
		if (Dfastates[i].end == true)		
		{
			d.end.insert(i);		
		}
	}
	cout << "NFA转DFA为" << endl;
	cout << "     ";
	CharSet::iterator t;
	for (t = d.ends.begin(); t != d.ends.end(); t++)
	{
		cout << *t << "   ";
	}
	cout << endl;

	for (int i = 0; i < dfastatesnum; i++)
	{

		if (d.end.count(i))
		{
			cout << '<' << i << ">  ";
		}
		else
		{
			cout << ' ' << i << "   ";
		}

		for (int j = 0; j < 26; j++)
		{
			if (d.ends.count(j + 97))
			{
				if (d.trans[i][j] != -858993460)
				{
					cout << d.trans[i][j] << "   ";
				}
				else
				{
					cout << "    ";
				}
			}
		}

		cout << endl;
	}
	return d;
}
int minDfaStateNum = 0;
struct minstate			
{
	int data;			
	IntSet s;			
};
IntSet s[MAX];
int findSetNum(int count, int n)
{

	for (int i = 0; i < count; i++)
	{
		if (s[i].count(n))
		{
			return i;
		}
	}
}
					
Dfa minDfaStates[MAX];
void DFAtomin(dfanode dfa)
{

	int i, j;

	dfanode minDfa;
	minDfa.ends = dfa.ends;

	bool endFlag = true;
	for (i = 0; i < dfastatesnum; i++)
	{
		Dfastates[i].data = i;
		if (Dfastates[i].end == false)
		{

			endFlag = false;
			minDfaStateNum = 2;

			s[1].insert(Dfastates[i].data);
		}
		else
		{
			s[0].insert(Dfastates[i].data);
		}
	}

	if (endFlag)
	{
		minDfaStateNum = 1;
	}

	bool cutFlag = true;
	while (cutFlag)
	{

		int cutCount = 0;
		for (i = 0; i < minDfaStateNum; i++)
		{

			CharSet::iterator it;
			for (it = dfa.ends.begin(); it != dfa.ends.end(); it++)
			{

				int setNum = 0;
				minstate temp[20];
				IntSet::iterator iter;
				for (iter = s[i].begin(); iter != s[i].end(); iter++)
				{

					bool epFlag = true;
					for (j = 0; j < Dfastates[*iter].dfasnum; j++)
					{

						if (Dfastates[*iter].dfas[j].key == *it)
						{

							epFlag = false;
							int transNum = findSetNum(minDfaStateNum, Dfastates[*iter].dfas[j].data);

							int curSetNum = 0;
							while ((temp[curSetNum].data != transNum) && (curSetNum < setNum))
							{
								curSetNum++;
							}

							if (curSetNum == setNum)
							{
								temp[setNum].data = transNum;
								temp[setNum].s.insert(*iter);

								setNum++;
							}
							else
							{
								temp[curSetNum].s.insert(*iter);
							}
						}
					}

					if (epFlag)
					{
						int curSetNum = 0;
						while ((temp[curSetNum].data != -858993460) && (curSetNum < setNum))
						{
							curSetNum++;
						}

						if (curSetNum == setNum)
						{
							temp[setNum].data = -1;
							temp[setNum].s.insert(*iter);

							setNum++;
						}
						else
						{
							temp[curSetNum].s.insert(*iter);
						}
					}
				}

				if (setNum > 1)
				{

					cutCount++;
					for (j = 1; j < setNum; j++)
					{

						IntSet::iterator t;
						for (t = temp[j].s.begin(); t != temp[j].s.end(); t++)
						{

							s[i].erase(*t);
							s[minDfaStateNum].insert(*t);
						}

						minDfaStateNum++;
					}
				}
			}
		}

		if (cutCount == 0)
		{
			cutFlag = false;
		}
	}
	for (i = 0; i < minDfaStateNum; i++)
	{

		IntSet::iterator y;
		for (y = s[i].begin(); y != s[i].end(); y++)
		{

			if (*y == dfa.start)
			{
				minDfa.start = i;
			}

			if (dfa.end.count(*y))
			{

				minDfaStates[i].end = true;
				minDfa.end.insert(i);
			}

			for (j = 0; j < Dfastates[*y].dfasnum; j++)
			{

				/*遍历划分好的状态集合，找出该弧转移到的状态现在属于哪个集合*/
				for (int t = 0; t < minDfaStateNum; t++)
				{
					if (s[t].count(Dfastates[*y].dfas[j].key))
					{

						bool haveEdge = false;
						for (int l = 0; l < minDfaStates[i].dfasnum; l++)
						{
							if ((minDfaStates[i].dfas[l].data == Dfastates[*y].dfas[j].data) && (minDfaStates[i].dfas[l].key == t))
							{
								haveEdge = true;
							}
						}

						if (!haveEdge)
						{

							minDfaStates[i].dfas[minDfaStates[i].dfasnum].data = Dfastates[*y].dfas[j].data;
							minDfaStates[i].dfas[minDfaStates[i].dfasnum].key = t;

							minDfa.trans[i][Dfastates[*y].dfas[j].data - 'a'] = t;

							minDfaStates[i].dfasnum++;
						}

						break;
					}
				}
			}
		}
	}
	cout << "最小化dfa为" << endl;
	cout << "     ";
	CharSet::iterator t;
	for (t = dfa.ends.begin(); t != dfa.ends.end(); t++)
	{
		cout << *t << "   ";
	}
	cout << endl;
	for (i = 0; i < minDfaStateNum; i++)
	{
		cout << ' ' << i << "   ";

		for (j = 0; j < 26; j++)
		{
				if (dfa.trans[i][j] != -858993460)
				{
					cout << dfa.trans[i][j] << "   ";
				}
				else
				{
					cout << "    ";
				}
		}

		cout << endl;
	}
}

int main()
{

	//测试样例1
	//string str = "(a|b)*abb";

	//测试样例2
	string str = "(a|b*)c*";
	str = infixToSuffix(str);		


	int i, j;
	for (i = 0; i < MAX; i++)
	{

		NfaStates[i].key = i;
		NfaStates[i].data = '#';
		NfaStates[i].chTrans = -1;
	}

	NFA n = strToNfa(str);
	dfanode d;
	printNFA(n);
	d=NFAtoDFA(n,str);
	DFAtomin(d);
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
