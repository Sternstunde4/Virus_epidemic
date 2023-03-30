#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

//病毒树结点，依次存储编号,到达该结点的路径上的变异时间,研制疫苗时间,疫苗研制成功后变异时间
struct Node {
	int Number, VariationTime, VaccineTime, AfterVariationTime;
	Node* FirstChild, * NextSibling;
	Node(int num = 0, int variationT = 0, int vaccineT = 0, int afterVariationT = 0,
		Node* fc = NULL, Node* ns = NULL) :
		Number(num), VariationTime(variationT), VaccineTime(vaccineT), AfterVariationTime(afterVariationT),
		FirstChild(fc), NextSibling(ns) {}
};

//将p所指的结点插入到root为根结点的树中的对应位置，即作为id的孩子,插入成功返回true
bool Insert(Node* root, Node* p, int parent) {
	Node* q = root; bool result = false;
	//如果root是parent，将p结点插入到最后一个孩子的NextSibling
	if (q->Number == parent && q->FirstChild) {
		q = q->FirstChild;
		while (q->NextSibling) q = q->NextSibling;
		q->NextSibling = p;
		return true;
	}
	//如果root是parent且root没有孩子，p作为root的FirstChild
	else if (q->Number == parent && !q->FirstChild) {
		q->FirstChild = p; return true;
	}
	//如果root不是parent，递归把p结点插入root->FirstChild为根结点的树中
	else if (root->FirstChild) {
		result = Insert(root->FirstChild, p, parent);
	}
	//如果root不是parent，且结点没有成功插入root->FirstChild为根结点的树中，
	//递归把p结点插入root->NextSibling为根结点的树中
	if (!result && root->NextSibling)return Insert(root->NextSibling, p, parent);
	return result;
}

//在root为根结点的树中找到n1的父结点并返回指向父结点的指针,若没找到，返回NULL，
//father是root的父结点（左孩子右兄弟的存储下利用前序遍历）
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

//在root为根结点的树中找到编号n1的孩子结点返回true（左孩子右兄弟的存储下利用前序遍历）
bool FindChild(Node* root, int n1) {
	if (root) {
		if (root->Number == n1)return false;
		if (root->FirstChild) {
			//遍历root的孩子结点，并且对遍历的每一个孩子递归查找n1
			Node* p = root->FirstChild;
			while (p->Number != n1 && p->NextSibling) {
				bool temp = FindChild(p, n1);
				if (temp)return temp;
				p = p->NextSibling;
			}
			if (p->Number == n1)return true;
			else {//p是root的最后一个孩子，再对p递归查找n1
				bool temp = FindChild(p, n1);
				if (temp)return temp;
			}
		}
	}
	return false;
}

//在root为根结点的树中找到n1,n2的最近公共祖先，输出编号
int LCA(Node* root, int n1, int n2) {
	if (root->Number == n1)return n1;
	if (root->Number == n2)return n2;
	//若n2是n1的祖先
	Node* father = FindFather(root, NULL, n2);
	Node* p = father->FirstChild;
	while (p->Number != n2)p = p->NextSibling;//找到n2的指针p
	if (FindChild(p, n1))return n2;
	//若n1是n2的祖先
	father = FindFather(root, NULL, n1);
	p = father->FirstChild;
	while (p->Number != n1)p = p->NextSibling;//找到n1的指针p
	if (FindChild(p, n2))return n1;
	//循环找n1的上层祖先，直到也是n2的祖先为止
	while (father && !FindChild(father, n2)) {
		father = FindFather(root, NULL, father->Number);
	}
	return father->Number;
}

int main() {
	cout << "请输入病毒变异树：" << endl;
	int N;//病毒变异树的结点个数
	cin >> N;
	int Parent, VariationTime, VaccineTime, AfterVariationTime;
	Node* root = new Node(1, 0, 0, 0);
	for (int i = 2; i <= N; i++) {
		cin >> Parent >> VariationTime >> VaccineTime >> AfterVariationTime;
		Node* p = new Node(i, VariationTime, VaccineTime, AfterVariationTime);
		Insert(root, p, Parent);
	}
	cout << "请输入两个需求最近公共祖先的病毒编号：" << endl;
	int n1, n2;
	cin >> n1 >> n2;
	cout << LCA(root, n1, n2) << endl;
	system("pause");
	return 0;
}