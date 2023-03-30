#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#define MAXVirus 10//最多10种病毒
using namespace std;
int sum = 100000;//蓝星上最多10万人

//病毒树结点，依次存储编号,到达该结点的路径上的变异时间,研制疫苗时间,疫苗研制成功后变异时间,孩子数量,潜伏期，
//（潜伏期）传染能力，致死率（康复、持续、死亡的概率）
struct Node {
	int Number, VariationTime, VaccineTime, AfterVariationTime, ChildNumber, LurkTime;
	float Infect, Recover, Remain, Death;
	Node* FirstChild, * NextSibling;
	Node(int num = 0, int variationT = 0, int vaccineT = 0, int afterVariationT = 0,
		int cn = 0, int lt = 0, float inf = 0, float rec = 0, float rem = 0, float death = 0,
		Node* fc = NULL, Node* ns = NULL) :
		Number(num), VariationTime(variationT), VaccineTime(vaccineT), AfterVariationTime(afterVariationT),
		ChildNumber(cn), LurkTime(lt), Infect(inf), Recover(rec), Remain(rem), Death(death),
		FirstChild(fc), NextSibling(ns) {}
};

//人所感染的病毒状态信息
struct Virus {
	int Exist;//0表不存在，1表存在
	int InfectTime;//感染时间
	int State;//1康复，2持续，3死亡
	int Isolation;//1隔离，0未隔离
	Virus(int e = 0, int i = 0, int s = 0, int iso = 0) :Exist(e), InfectTime(i), State(s), Isolation(0) {}
};

//人群中每个个体的类定义,包含目前感染病毒信息和历史感染病毒编号
struct Person {
	vector<Virus> NowVirus;//下标表示病毒编号-1,如下标为0，病毒编号为1
	vector<int> HistoryVirus;//下标表示病毒编号-1,为0没感染过，为1感染过,初始全部置为0
	Person(int N = MAXVirus) {
		for (int i = 0; i < N; i++) {
			HistoryVirus.push_back(0);
			Virus V;
			NowVirus.push_back(V);
		}
	}
};

//每种病毒的当前状态
struct VirusState {
	int InfectNumber, LurkNumber, OutBreakNumber, IsolateNumber, RecoverNumber, DeathNumber;
	VirusState(int ifn = 0, int ln = 0, int on = 0, int isn = 0, int rn = 0, int dn = 0) :InfectNumber(ifn),
		LurkNumber(ln), OutBreakNumber(on), IsolateNumber(isn), RecoverNumber(rn), DeathNumber(dn) {}
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

//人群感染的病毒随机变异,输入病毒编号，孩子数，感染天数,存储感染人群中变异情况到引用variation中，
//variation下标为感染人群中的序号-1,值表示变异成的病毒序号，若已研制出疫苗，变异时间改变
void Variation(Node* root, int Number, int ChildNumber, int InfectTime, vector<VirusState> result,
	vector<int>& variation, vector<int> VaccineVirus) {
	Node* p;//找到编号为Number的指针p
	if (Number == 1)p = root;
	else {
		p = FindFather(root, 0, Number);
		p = p->FirstChild;
		while (p->Number != Number)p = p->NextSibling;
	}
	//感染时间超过孩子的变异时间的孩子数量，是要随机的次数
	int RandTime = 0;
	if (p->FirstChild) {//至少有一个孩子，才会变异
		p = p->FirstChild;
		vector<int> Child;//保存可能被随机到的变异体编号
		for (int j = ChildNumber; j > 0; j--) {
			//若该病毒已研制出疫苗
			if (VaccineVirus.end() != find(VaccineVirus.begin(), VaccineVirus.end(), p->Number)) {
				if (p->AfterVariationTime <= InfectTime) {
					RandTime++; Child.push_back(p->Number);
				}
			}
			else {
				if (p->VariationTime <= InfectTime) {
					RandTime++; Child.push_back(p->Number);
				}
			}
			p = p->NextSibling;
		}
		for (int i = 0; i < RandTime; i++) {//进行RandTime次随机
			//每次随机将选出感染总人数的1/ChildNumber的人数
			for (int j = 0; j < result[Number - 1].InfectNumber / ChildNumber; j++) {
				int num = rand() % result[Number - 1].InfectNumber + 1;//生成1到感染总人数的随机数
				if (variation[num - 1] == 0)variation[num - 1] = Child[i];//感染人群中第num 个将变异为可能被随机到的第i+1个变异体
				else {
					int n = rand() % 2 + 1;//生成1或2的随机数
					if (n == 1)variation[num - 1] = Child[i];//感染人群中第num个将变异为可能被随机到的第i+1个变异体，否则
					//感染人群中第num个将变异为原来variation[num-1]存储的变异体
				}
			}
		}
	}
	
}

//求最大公约数
int gcd(int a, int b) {
	int r;
	while (b > 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}

//求最小公倍数
int lcm(int a, int b) {
	return a / gcd(a, b) * b;
}

//人感染相应病毒的状态变化
int StateTransfer(Node* root, int VirusNumber) {
	Node* p;
	if (VirusNumber == root->Number)p = root;
	else {
		Node* father = FindFather(root, NULL, VirusNumber);
		p = father->FirstChild;
		while (p->Number != VirusNumber)p = p->NextSibling;//找到VirusNumber的指针p
	}
	float Recover = p->Recover; int RecoverN1 = 1 / Recover;
	float Remain = p->Remain; int RemainN1 = 1 / Remain;
	float Death = p->Death; int DeathN1 = 1 / Death;
	int LCM = lcm(RecoverN1, RemainN1);
	LCM = lcm(LCM, DeathN1);
	int num = rand() % LCM + 1;//生成1到LCM的随机数
	if (num > 0 && num <= LCM * Recover)return 1;//下一状态为康复
	else if (num > LCM * Recover && num <= LCM * (Recover + Remain))return 2;//下一状态为持续
	else  return 3;//下一状态为死亡
}

//传染其他人,即在crowd中再找出Infect个没感染过编号为VirusNumber的病毒的人，使其感染,已研制出疫苗的病毒不再感染
void InfectOthers(Node* root, vector<Person>& crowd, vector<VirusState>& result, int VirusNumber,
	float Infect, int N, vector<int> VaccineVirus) {//Infect为感染能力
	//若该病毒已研制出疫苗
	if (VaccineVirus.end() != find(VaccineVirus.begin(), VaccineVirus.end(), VirusNumber)) return;
	//需要新感染的人数
	int NewInfect = int(Infect * (result[VirusNumber - 1].InfectNumber - result[VirusNumber - 1].IsolateNumber)
		+ 10 * Infect * result[VirusNumber - 1].OutBreakNumber);
	int count = 0;//记录新感染的人数
	Node* father;
	vector<Person>::iterator iter = crowd.begin();
	for (; iter != crowd.end();) {
		if (count < NewInfect && iter->HistoryVirus[VirusNumber - 1] == 0) {//未感染过该病毒及其祖先
			count++;
			iter->HistoryVirus[VirusNumber - 1] = 1;
			iter->NowVirus[VirusNumber - 1].Exist = 1;
			iter->NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
			if (iter->NowVirus[VirusNumber - 1].State == 1) {//新状态为康复
				result[VirusNumber - 1].RecoverNumber++;
				iter->NowVirus[VirusNumber - 1].Exist = 0;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 2) {//新状态为持续
				result[VirusNumber - 1].InfectNumber++;
				//感染时间为0，该人应在潜伏期
				result[VirusNumber - 1].LurkNumber++;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 3) {//新状态为死亡
				result[VirusNumber - 1].DeathNumber++;
				iter=crowd.erase(iter); sum--;
				continue;
			}
			int tempNumber = VirusNumber;
			while (father = FindFather(root, NULL, tempNumber)) {//标记该病毒及其祖先，使其不会再次感染
				iter->HistoryVirus[father->Number - 1] = 1;
				tempNumber = father->Number;
			}
		}
		else if (count == Infect)break;
		iter++;
	}
	while (count < NewInfect&&crowd.size()<sum) {//原有人群中的未感染人数不足
		count++;
		Person P3(N);
		P3.HistoryVirus[VirusNumber - 1] = 1;
		P3.NowVirus[VirusNumber - 1].Exist = 1;
		P3.NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
		if (P3.NowVirus[VirusNumber - 1].State == 1) {//新状态为康复
			result[VirusNumber - 1].RecoverNumber++;
			P3.NowVirus[VirusNumber - 1].Exist = 0;
		}
		else if (P3.NowVirus[VirusNumber - 1].State == 2) {//新状态为持续
			result[VirusNumber - 1].InfectNumber++;
			//感染时间为0，该人应在潜伏期
			result[VirusNumber - 1].LurkNumber++;
		}
		else if (P3.NowVirus[VirusNumber - 1].State == 3) {//新状态为死亡
			result[VirusNumber - 1].DeathNumber++; sum--;
			continue;
		}
		int tempNumber = VirusNumber;
		while (father = FindFather(root, NULL, tempNumber)) {//标记该病毒及其祖先，使其不会再次感染
			P3.HistoryVirus[father->Number - 1] = 1;
			tempNumber = father->Number;
		}
		crowd.push_back(P3);
	}
}

//病毒变异，人的感染状态改变，result相应改变,函数返回1表示该人死亡，需要删除，否则返回0
int VariationChange(Node* root,Person& P, vector<VirusState>& result, int VirusNumber, int NewVirusNumber,int t) {
	P.NowVirus[VirusNumber - 1].Exist = 0;
	//原来状态一定为持续
	int time = P.NowVirus[VirusNumber - 1].InfectTime;
	result[VirusNumber - 1].InfectNumber--;
	Node* p;//找到编号为VirusNumber的指针p
	if (VirusNumber == 1)p = root;
	else {
		p = FindFather(root, 0, VirusNumber);
		p = p->FirstChild;
		while (p->Number != VirusNumber)p = p->NextSibling;
	}
	if (time <= (p->LurkTime)*24) //该人原在潜伏期
		result[VirusNumber - 1].LurkNumber--;
	else if (time > p->LurkTime*24 && time <= (p->LurkTime + t) * 24)  //该人原来在爆发期
		result[VirusNumber - 1].OutBreakNumber--;
	else {//该人原来在隔离期
		result[VirusNumber - 1].IsolateNumber--;
	}
	//新病毒感染状态
	P.NowVirus[NewVirusNumber - 1].Exist = 1;
	P.NowVirus[NewVirusNumber - 1].InfectTime = 0;
	P.NowVirus[NewVirusNumber - 1].State = StateTransfer(root, VirusNumber);
	if (P.NowVirus[NewVirusNumber - 1].State == 1) {//新状态为康复
		result[NewVirusNumber - 1].RecoverNumber++;
		P.NowVirus[NewVirusNumber - 1].Exist = 0;
	}
	else if (P.NowVirus[NewVirusNumber - 1].State == 2) {//新状态为持续
		result[NewVirusNumber - 1].InfectNumber++;
		 //该人应在潜伏期
			result[NewVirusNumber - 1].LurkNumber++;
	}
	else {//新状态为死亡
		result[NewVirusNumber - 1].DeathNumber++;
		return 1;
	}
	P.NowVirus[NewVirusNumber - 1].Isolation = 0;//未隔离
	return 0;
}

//遍历人群中感染该病毒的人,相应改变每种病毒的各项人数状态，和人群的该病毒感染状态
void TravelCrowd(Node* root, vector<Person>& crowd, vector<VirusState>& result, Node* p,
	int t, int N, vector<int> VaccineVirus) {
	if (crowd.size() == 0)return;
	int VirusNumber = p->Number;//病毒编号
	int VariationTime = p->VariationTime;
	int VaccineTime = p->VaccineTime;
	int AfterVariationTime = p->AfterVariationTime;
	int ChildNumber = p->ChildNumber;
	int LurkTime = p->LurkTime;
	float Infect = p->Infect;
	//遍历人群中感染该病毒的人,遍历完后再统一感染新的人
	vector<Person>::iterator iter = crowd.begin();
	for (; iter != crowd.end();) {
		if (iter->NowVirus[VirusNumber - 1].Exist) {
			//原来状态一定为持续
			int time = iter->NowVirus[VirusNumber - 1].InfectTime;
			result[VirusNumber - 1].InfectNumber--;
			if (time <= LurkTime*24) //该人原在潜伏期
				result[VirusNumber - 1].LurkNumber--;
			else if (time > LurkTime*24 && time <= (LurkTime + t) * 24)  //该人原来在爆发期
				result[VirusNumber - 1].OutBreakNumber--;
			else //该人原来在隔离期
				result[VirusNumber - 1].IsolateNumber--;
			//人的状态变化
			iter->NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
			time = ++iter->NowVirus[VirusNumber - 1].InfectTime;
			if (iter->NowVirus[VirusNumber - 1].State == 1) {//新状态为康复
				result[VirusNumber - 1].RecoverNumber++;
				iter->NowVirus[VirusNumber - 1].Exist = 0;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 2) {//新状态为持续
				result[VirusNumber - 1].InfectNumber++;
				if (time <= LurkTime * 24) //该人应在潜伏期
					result[VirusNumber - 1].LurkNumber++;
				else if (time > LurkTime * 24 && time <= (LurkTime+t) * 24) //该人应在爆发期
					result[VirusNumber - 1].OutBreakNumber++;
				else {//该人应在隔离期
					result[VirusNumber - 1].IsolateNumber++;
					iter->NowVirus[VirusNumber - 1].Isolation = 1;
				}
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 3) {//新状态为死亡
				result[VirusNumber - 1].DeathNumber++;
				iter=crowd.erase(iter); sum--;
				continue;
			}
			if (time % 24 == 0&& iter->NowVirus[VirusNumber - 1].Exist ) {//感染时间到达整天数，才考虑变异
				vector<int> variation;//用于保存感染人群中变异情况
				for (int k = 0; k < result[VirusNumber - 1].InfectNumber; k++) variation.push_back(0);
				Variation(root, VirusNumber, ChildNumber, time / 24, result, variation, VaccineVirus);//随机变异
				int t = 0, count = 0;//找i是第几个感染VirusNumber病毒的人
				for (; t < result[VirusNumber - 1].InfectNumber; t++) {
					if (crowd[t].NowVirus[VirusNumber - 1].Exist)count++;
					if (t == iter-crowd.begin())break;
				}
				int NewVirusNumber = 0;
				if (count != 0)NewVirusNumber = variation[count - 1];//感染人群中第一个人下标为0
				if (NewVirusNumber == 0)continue;//感染时间未达到最短变异时间的孩子的要求，故不会变异
				if (VariationChange(root, *iter, result, VirusNumber, NewVirusNumber, t)) {
					iter=crowd.erase(iter); sum--;
					continue;
				}
			}
		}
		iter++;
	}
	InfectOthers(root, crowd, result, VirusNumber, Infect, N, VaccineVirus);
}


//遍历病毒树，同时对每种病毒遍历一次人群
void TravelVirus(Node* WholeRoot, Node* PartialRoot, vector<Person>& crowd, vector<VirusState>& result,
	int t, int N, vector<int> VaccineVirus) {
	if (PartialRoot) {
		TravelCrowd(WholeRoot, crowd, result, PartialRoot, t, N, VaccineVirus);
		if (PartialRoot->FirstChild)
			TravelVirus(WholeRoot, PartialRoot->FirstChild, crowd, result, t, N, VaccineVirus);
		if (PartialRoot->NextSibling)
			TravelVirus(WholeRoot, PartialRoot->NextSibling, crowd, result, t, N, VaccineVirus);
	}
}

//层次序遍历病毒树，vector存储已研制出疫苗的病毒编号，1号病毒没有路径，没有疫苗研制
void TravelVaccine(Node* root, int days, vector<int>& VaccineVirus) {
	Node* p = root;
	queue<Node*> q;
	q.push(p);
	while (!q.empty()) {
		p = q.front(); q.pop();
		if (days >= p->VaccineTime && VaccineVirus.end() == find(VaccineVirus.begin(), VaccineVirus.end(), p->Number))
			VaccineVirus.push_back(p->Number);
		if (p->FirstChild)q.push(p->FirstChild);
		if (p->NextSibling)q.push(p->NextSibling);
	}
}

//沙箱模拟,t是爆发到隔离的时间
void Simulation(Node* root, int N, int t, int days) {
	vector<Person> crowd;
	vector<VirusState> result;
	Person P1(N);
	VirusState VS1(0, 0, 0, 0, 0, 0);
	P1.HistoryVirus[0] = 1;
	P1.NowVirus[0].Exist = 1;
	P1.NowVirus[0].State = StateTransfer(root, 1);
	if (P1.NowVirus[0].State == 1) {//新状态为康复
		VS1.RecoverNumber=1;
		P1.NowVirus[0].Exist = 0;
	}
	else if (P1.NowVirus[0].State == 2) {//新状态为持续
		VS1.InfectNumber = 1;
		 //该人应在潜伏期
		VS1.LurkNumber=1;
	}
	else if (P1.NowVirus[0].State == 3) {//新状态为死亡
		VS1.DeathNumber=1;
		P1.NowVirus[0].Exist = 0;//该人死亡后病毒也不存在了
	}
	crowd.push_back(P1);
	result.push_back(VS1);
	for (int j = 1; j < N; j++) {
		VirusState VS2;
		result.push_back(VS2);
	}
	Node* p = root;
	int hours = 1;
	vector<int> VaccineVirus;//用于存储已研制出疫苗的病毒编号
	cout << "天数  病毒编号  感染人数  潜伏期人数  爆发期人数  隔离救治期人数  康复人数  死亡人数" << endl;
	for (; hours <= days * 24; hours++) {//为便于分析，规定模拟最多7天的感染情况
		TravelVirus(root, p, crowd, result, t, N, VaccineVirus);//每小时遍历一次病毒树
		if (hours % 24 == 0) {
			TravelVaccine(root, hours / 24, VaccineVirus);
			for (int i = 0; i < N; i++) {
				cout << hours / 24 << "  " << i + 1 << "  " << result[i].InfectNumber << "  "
					<< result[i].LurkNumber << "  " << result[i].OutBreakNumber << "  " << result[i].IsolateNumber
					<< "  " << result[i].RecoverNumber << "  " << result[i].DeathNumber << endl;
			}
		}
	}

}

int main() {
	srand(time(0));
	cout << "请输入病毒变异树：" << endl;
	int N;//病毒变异树的结点个数
	cin >> N;
	int Parent, VariationTime, VaccineTime, AfterVariationTime, ChildNumber, LurkTime;
	float Infect, Recover, Remain, Death;
	cin >> ChildNumber >> LurkTime >> Infect >> Recover >> Remain >> Death;
	Node* root = new Node(1, 100000, 1000000, 100000, ChildNumber, LurkTime, Infect, Recover, Remain, Death);
	for (int i = 2; i <= N; i++) {
		cin >> Parent >> VariationTime >> VaccineTime >> AfterVariationTime
			>> ChildNumber >> LurkTime >> Infect >> Recover >> Remain >> Death;
		Node* p = new Node(i, VariationTime, VaccineTime, AfterVariationTime, ChildNumber,
			LurkTime, Infect, Recover, Remain, Death);
		Insert(root, p, Parent);
	}
	cout << "请输入爆发到隔离的时间：" << endl;
	int t, days;
	cin >> t;
	cout << "请输入要观察的天数：" << endl;
	cin >> days;
	Simulation(root, N, t, days);
	system("pause");
	return 0;
}