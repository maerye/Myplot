#pragma once
#include<queue>
#include<stack>
#include<math.h>
#include<iostream>
//#include<windows.h>
#include "stdafx.h"
using namespace std;
class calculator
{
public:
	calculator() {}
	void setQueue(queue<TCHAR>_q) { q = _q; }
	bool getX(double x, double & y) {
		TCHAR ch;

		if (q.empty())
		{
			//cout << "please set the expression" << endl;
			return false;
		}
		while (!q.empty())
		{
			ch = q.front();
			q.pop();
			if (isdigit(ch) != 0)
			{
				addOperand(ch - 48);
			}
			else if (ch == 'x' || ch == 'X')
			{
				addOperand(x);
			}
			else
			{
				if (!doOperator(ch))
					return false;
			}
		}
		y = s.top();
		return true;
	}

	void clear() {
		while (!s.empty())
			s.pop();
		while (!q.empty())
			q.pop();
	}
	~calculator() {}
private:
	stack<double> s;
	queue<TCHAR>q;

	void addOperand(double value) { s.push(value); }
	bool get2Operands(double& left, double& right) {
		if (s.empty())
		{
			//cout << "missing the right operand" << endl;
			return false;
		}
		right = s.top();
		s.pop();
		if (s.empty())
		{
			//cout << "missing the left operand" << endl;
			return false;
		}
		left = s.top();
		s.pop();
		return true;
	}
	bool doOperator(TCHAR op) {
		double left, right, value;
		if (get2Operands(left, right))
		{
			switch (op)
			{
			case '+':
				value = left + right;
				s.push(value);
				break;
			case '-':
				value = left - right;
				s.push(value);
				break;
			case '*':
				value = left*right;
				s.push(value);
				break;
			case '/':
				if (right == 0)
				{
					//cout << "Divided by 0!" << endl;
					clear();
					return false;
				}
				else
				{
					value = left / right;
					s.push(value);
				}
				break;
			case '^':
				value = pow(left, right);
				s.push(value);
				break;
			case '%':
				value = (int)left % (int)right;
				s.push(value);
				break;
			default:
				//cout << "can't identify the symbol" << endl;
				clear();
				break;
			}
			return true;
		}
		else
		{
			clear();
			return false;
		}
	}
};