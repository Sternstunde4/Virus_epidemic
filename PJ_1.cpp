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

//��rootΪ�����������ҵ�n1�ĸ���㲢����ָ�򸸽���ָ��,��û�ҵ�������NULL��
//father��root�ĸ���㣨�������ֵܵĴ洢������ǰ�������
Node* FindFather(Node* root, Node* father, int n1) {
	Node* result = NULL;
	if (root) {
		if (root->FirstChild && root->FirstChild->Number == n1)return root;
		if (root->NextSibling && root->NextSibling->Number == n1)return father;
		if (root->FirstChild) {
			result = FindFather(root->FirstChild, root, n1);
			if (result)return result;
		}
		if (root->NextSibling) {
			result = FindFather(root->NextSibling, father, n1);
			if (result)return result;
		}
	}
	return result;
}

//��rootΪ�����������ҵ����n1�ĺ��ӽ�㷵��true���������ֵܵĴ洢������ǰ�������
bool FindChild(Node* root, int n1) {
	if (root) {
		if (root->Number == n1)return false;
		if (root->FirstChild) {
			//����root�ĺ��ӽ�㣬���ҶԱ�����ÿһ�����ӵݹ����n1
			Node* p = root->FirstChild;
			while (p->Number != n1 && p->NextSibling) {
				bool temp = FindChild(p, n1);
				if (temp)return temp;
				p = p->NextSibling;
			}
			if (p->Number == n1)return true;
			else {//p��root�����һ�����ӣ��ٶ�p�ݹ����n1
				bool temp = FindChild(p, n1);
				if (temp)return temp;
			}
		}
	}
	return false;
}

//��rootΪ�����������ҵ�n1,n2������������ȣ�������
int LCA(Node* root, int n1, int n2) {
	if (root->Number == n1)return n1;
	if (root->Number == n2)return n2;
	//��n2��n1������
	Node* father = FindFather(root, NULL, n2);
	Node* p = father->FirstChild;
	while (p->Number != n2)p = p->NextSibling;//�ҵ�n2��ָ��p
	if (FindChild(p, n1))return n2;
	//��n1��n2������
	father = FindFather(root, NULL, n1);
	p = father->FirstChild;
	while (p->Number != n1)p = p->NextSibling;//�ҵ�n1��ָ��p
	if (FindChild(p, n2))return n1;
	//ѭ����n1���ϲ����ȣ�ֱ��Ҳ��n2������Ϊֹ
	while (father && !FindChild(father, n2)) {
		father = FindFather(root, NULL, father->Number);
	}
	return father->Number;
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
	cout << "������������������������ȵĲ�����ţ�" << endl;
	int n1, n2;
	cin >> n1 >> n2;
	cout << LCA(root, n1, n2) << endl;
	system("pause");
	return 0;
}