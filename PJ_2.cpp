#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

//��������㣬���δ洢���,����ý���·���ϵı���ʱ��,��������ʱ��,�������Ƴɹ������ʱ��
struct Node {
	int Number, VariationTime, VaccineTime, AfterVariationTime;
	Node* FirstChild, * NextSibling;
	Node(int num = 0, int variationT = 0, int vaccineT = 0, int afterVariationT = 0,
		Node* fc = NULL, Node* ns = NULL) :
		Number(num), VariationTime(variationT), VaccineTime(vaccineT), AfterVariationTime(afterVariationT),
		FirstChild(fc), NextSibling(ns) {}
};

//��p��ָ�Ľ����뵽rootΪ���������еĶ�Ӧλ�ã�����Ϊid�ĺ���,����ɹ�����true
bool Insert(Node* root, Node* p, int parent) {
	Node* q = root; bool result = false;
	//���root��parent����p�����뵽���һ�����ӵ�NextSibling
	if (q->Number == parent && q->FirstChild) {
		q = q->FirstChild;
		while (q->NextSibling) q = q->NextSibling;
		q->NextSibling = p;
		return true;
	}
	//���root��parent��rootû�к��ӣ�p��Ϊroot��FirstChild
	else if (q->Number == parent && !q->FirstChild) {
		q->FirstChild = p; return true;
	}
	//���root����parent���ݹ��p������root->FirstChildΪ����������
	else if (root->FirstChild) {
		result = Insert(root->FirstChild, p, parent);
	}
	//���root����parent���ҽ��û�гɹ�����root->FirstChildΪ���������У�
	//�ݹ��p������root->NextSiblingΪ����������
	if (!result && root->NextSibling)return Insert(root->NextSibling, p, parent);
	return result;
}

//����n1�Ľ��ı������ʱ�䣬ʵ������Ӹ���㵽n1���·����,time��ʾԭʼ����㵽root�ı������ʱ��
int SumVariationTime(Node* root, int time, int n1) {
	if (root) {
		if (root->FirstChild) {
			//����root�ĺ��ӽ�㣬���ҶԱ�����ÿһ�����ӵݹ����n1
			Node* p = root->FirstChild;
			int SumTime = time;
			while (p && p->Number != n1) {
				int t = SumVariationTime(p, time + p->VariationTime, n1);
				if (t)return t;
				p = p->NextSibling;
			}
			if (p && p->Number == n1)return time + p->VariationTime;
		}
	}
	return 0;//û���ҵ�n1
}

//����������������Ƿ����ڸñ������ǰ���Ƴɹ���
//����Ŵ�С�����˳��������������ܣ����yes, �����ܣ����no����������೤ʱ��
//���ö��еĲ�������
void IsTimely(Node* root) {
	queue<Node*> q;
	q.push(root);
	while (!q.empty()) {
		Node* p = q.front(); q.pop();//���ӵ�ÿ���������Ѿ��������
		if (p->FirstChild) {
			p = p->FirstChild;
			if (p->VaccineTime < SumVariationTime(root, 0, p->Number))cout << "yes" << endl;
			else cout << "no " << p->VaccineTime - SumVariationTime(root, 0, p->Number) << endl;
			q.push(p);
			while (p->NextSibling) {
				p = p->NextSibling;
				if (p->VaccineTime < SumVariationTime(root, 0, p->Number))cout << "yes" << endl;
				else cout << "no " << p->VaccineTime - SumVariationTime(root, 0, p->Number) << endl;
				q.push(p); //root�����к����������
			}
		}
	}
}

int main() {
	cout << "�����벡����������" << endl;
	int N;//�����������Ľ�����
	cin >> N;
	int Parent, VariationTime, VaccineTime, AfterVariationTime;
	Node* root = new Node(1, 0, 0, 0);
	for (int i = 2; i <= N; i++) {
		cin >> Parent >> VariationTime >> VaccineTime >> AfterVariationTime;
		Node* p = new Node(i, VariationTime, VaccineTime, AfterVariationTime);
		Insert(root, p, Parent);
	}
	cout << "ÿ�ֱ�����������Ƿ����ڸñ������ǰ���Ƴɹ���" << endl;
	cout << "���ܣ����yes, �����ܣ����no����������೤ʱ��(�������岡����������������" << endl;
	IsTimely(root);
	system("pause");
	return 0;
}