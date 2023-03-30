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

//求编号n1的结点的变异出现时间，实际是求从根结点到n1结点路径长,time表示原始根结点到root的变异出现时间
int SumVariationTime(Node* root, int time, int n1) {
	if (root) {
		if (root->FirstChild) {
			//遍历root的孩子结点，并且对遍历的每一个孩子递归查找n1
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
	return 0;//没有找到n1
}

//各个变异体的疫苗是否能在该变异出现前研制成功？
//按编号从小到大的顺序依次输出，若能，输出yes, 若不能，输出no并输出会落后多长时间
//利用队列的层次序遍历
void IsTimely(Node* root) {
	queue<Node*> q;
	q.push(root);
	while (!q.empty()) {
		Node* p = q.front(); q.pop();//出队的每个病毒都已经计算过了
		if (p->FirstChild) {
			p = p->FirstChild;
			if (p->VaccineTime < SumVariationTime(root, 0, p->Number))cout << "yes" << endl;
			else cout << "no " << p->VaccineTime - SumVariationTime(root, 0, p->Number) << endl;
			q.push(p);
			while (p->NextSibling) {
				p = p->NextSibling;
				if (p->VaccineTime < SumVariationTime(root, 0, p->Number))cout << "yes" << endl;
				else cout << "no " << p->VaccineTime - SumVariationTime(root, 0, p->Number) << endl;
				q.push(p); //root的所有孩子依次入队
			}
		}
	}
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
	cout << "每种变异体的疫苗是否能在该变异出现前研制成功？" << endl;
	cout << "若能，输出yes, 若不能，输出no并输出会落后多长时间(按变异体病毒编号依次输出）：" << endl;
	IsTimely(root);
	system("pause");
	return 0;
}