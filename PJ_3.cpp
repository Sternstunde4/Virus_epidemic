#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#define MAXVirus 10//���10�ֲ���
using namespace std;
int sum = 100000;//���������10����

//��������㣬���δ洢���,����ý���·���ϵı���ʱ��,��������ʱ��,�������Ƴɹ������ʱ��,��������,Ǳ���ڣ�
//��Ǳ���ڣ���Ⱦ�����������ʣ������������������ĸ��ʣ�
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

//������Ⱦ�Ĳ���״̬��Ϣ
struct Virus {
	int Exist;//0�����ڣ�1�����
	int InfectTime;//��Ⱦʱ��
	int State;//1������2������3����
	int Isolation;//1���룬0δ����
	Virus(int e = 0, int i = 0, int s = 0, int iso = 0) :Exist(e), InfectTime(i), State(s), Isolation(0) {}
};

//��Ⱥ��ÿ��������ඨ��,����Ŀǰ��Ⱦ������Ϣ����ʷ��Ⱦ�������
struct Person {
	vector<Virus> NowVirus;//�±��ʾ�������-1,���±�Ϊ0���������Ϊ1
	vector<int> HistoryVirus;//�±��ʾ�������-1,Ϊ0û��Ⱦ����Ϊ1��Ⱦ��,��ʼȫ����Ϊ0
	Person(int N = MAXVirus) {
		for (int i = 0; i < N; i++) {
			HistoryVirus.push_back(0);
			Virus V;
			NowVirus.push_back(V);
		}
	}
};

//ÿ�ֲ����ĵ�ǰ״̬
struct VirusState {
	int InfectNumber, LurkNumber, OutBreakNumber, IsolateNumber, RecoverNumber, DeathNumber;
	VirusState(int ifn = 0, int ln = 0, int on = 0, int isn = 0, int rn = 0, int dn = 0) :InfectNumber(ifn),
		LurkNumber(ln), OutBreakNumber(on), IsolateNumber(isn), RecoverNumber(rn), DeathNumber(dn) {}
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

//��Ⱥ��Ⱦ�Ĳ����������,���벡����ţ�����������Ⱦ����,�洢��Ⱦ��Ⱥ�б������������variation�У�
//variation�±�Ϊ��Ⱦ��Ⱥ�е����-1,ֵ��ʾ����ɵĲ�����ţ��������Ƴ����磬����ʱ��ı�
void Variation(Node* root, int Number, int ChildNumber, int InfectTime, vector<VirusState> result,
	vector<int>& variation, vector<int> VaccineVirus) {
	Node* p;//�ҵ����ΪNumber��ָ��p
	if (Number == 1)p = root;
	else {
		p = FindFather(root, 0, Number);
		p = p->FirstChild;
		while (p->Number != Number)p = p->NextSibling;
	}
	//��Ⱦʱ�䳬�����ӵı���ʱ��ĺ�����������Ҫ����Ĵ���
	int RandTime = 0;
	if (p->FirstChild) {//������һ�����ӣ��Ż����
		p = p->FirstChild;
		vector<int> Child;//������ܱ�������ı�������
		for (int j = ChildNumber; j > 0; j--) {
			//���ò��������Ƴ�����
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
		for (int i = 0; i < RandTime; i++) {//����RandTime�����
			//ÿ�������ѡ����Ⱦ��������1/ChildNumber������
			for (int j = 0; j < result[Number - 1].InfectNumber / ChildNumber; j++) {
				int num = rand() % result[Number - 1].InfectNumber + 1;//����1����Ⱦ�������������
				if (variation[num - 1] == 0)variation[num - 1] = Child[i];//��Ⱦ��Ⱥ�е�num ��������Ϊ���ܱ�������ĵ�i+1��������
				else {
					int n = rand() % 2 + 1;//����1��2�������
					if (n == 1)variation[num - 1] = Child[i];//��Ⱦ��Ⱥ�е�num��������Ϊ���ܱ�������ĵ�i+1�������壬����
					//��Ⱦ��Ⱥ�е�num��������Ϊԭ��variation[num-1]�洢�ı�����
				}
			}
		}
	}
	
}

//�����Լ��
int gcd(int a, int b) {
	int r;
	while (b > 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}

//����С������
int lcm(int a, int b) {
	return a / gcd(a, b) * b;
}

//�˸�Ⱦ��Ӧ������״̬�仯
int StateTransfer(Node* root, int VirusNumber) {
	Node* p;
	if (VirusNumber == root->Number)p = root;
	else {
		Node* father = FindFather(root, NULL, VirusNumber);
		p = father->FirstChild;
		while (p->Number != VirusNumber)p = p->NextSibling;//�ҵ�VirusNumber��ָ��p
	}
	float Recover = p->Recover; int RecoverN1 = 1 / Recover;
	float Remain = p->Remain; int RemainN1 = 1 / Remain;
	float Death = p->Death; int DeathN1 = 1 / Death;
	int LCM = lcm(RecoverN1, RemainN1);
	LCM = lcm(LCM, DeathN1);
	int num = rand() % LCM + 1;//����1��LCM�������
	if (num > 0 && num <= LCM * Recover)return 1;//��һ״̬Ϊ����
	else if (num > LCM * Recover && num <= LCM * (Recover + Remain))return 2;//��һ״̬Ϊ����
	else  return 3;//��һ״̬Ϊ����
}

//��Ⱦ������,����crowd�����ҳ�Infect��û��Ⱦ�����ΪVirusNumber�Ĳ������ˣ�ʹ���Ⱦ,�����Ƴ�����Ĳ������ٸ�Ⱦ
void InfectOthers(Node* root, vector<Person>& crowd, vector<VirusState>& result, int VirusNumber,
	float Infect, int N, vector<int> VaccineVirus) {//InfectΪ��Ⱦ����
	//���ò��������Ƴ�����
	if (VaccineVirus.end() != find(VaccineVirus.begin(), VaccineVirus.end(), VirusNumber)) return;
	//��Ҫ�¸�Ⱦ������
	int NewInfect = int(Infect * (result[VirusNumber - 1].InfectNumber - result[VirusNumber - 1].IsolateNumber)
		+ 10 * Infect * result[VirusNumber - 1].OutBreakNumber);
	int count = 0;//��¼�¸�Ⱦ������
	Node* father;
	vector<Person>::iterator iter = crowd.begin();
	for (; iter != crowd.end();) {
		if (count < NewInfect && iter->HistoryVirus[VirusNumber - 1] == 0) {//δ��Ⱦ���ò�����������
			count++;
			iter->HistoryVirus[VirusNumber - 1] = 1;
			iter->NowVirus[VirusNumber - 1].Exist = 1;
			iter->NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
			if (iter->NowVirus[VirusNumber - 1].State == 1) {//��״̬Ϊ����
				result[VirusNumber - 1].RecoverNumber++;
				iter->NowVirus[VirusNumber - 1].Exist = 0;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 2) {//��״̬Ϊ����
				result[VirusNumber - 1].InfectNumber++;
				//��Ⱦʱ��Ϊ0������Ӧ��Ǳ����
				result[VirusNumber - 1].LurkNumber++;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 3) {//��״̬Ϊ����
				result[VirusNumber - 1].DeathNumber++;
				iter=crowd.erase(iter); sum--;
				continue;
			}
			int tempNumber = VirusNumber;
			while (father = FindFather(root, NULL, tempNumber)) {//��Ǹò����������ȣ�ʹ�䲻���ٴθ�Ⱦ
				iter->HistoryVirus[father->Number - 1] = 1;
				tempNumber = father->Number;
			}
		}
		else if (count == Infect)break;
		iter++;
	}
	while (count < NewInfect&&crowd.size()<sum) {//ԭ����Ⱥ�е�δ��Ⱦ��������
		count++;
		Person P3(N);
		P3.HistoryVirus[VirusNumber - 1] = 1;
		P3.NowVirus[VirusNumber - 1].Exist = 1;
		P3.NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
		if (P3.NowVirus[VirusNumber - 1].State == 1) {//��״̬Ϊ����
			result[VirusNumber - 1].RecoverNumber++;
			P3.NowVirus[VirusNumber - 1].Exist = 0;
		}
		else if (P3.NowVirus[VirusNumber - 1].State == 2) {//��״̬Ϊ����
			result[VirusNumber - 1].InfectNumber++;
			//��Ⱦʱ��Ϊ0������Ӧ��Ǳ����
			result[VirusNumber - 1].LurkNumber++;
		}
		else if (P3.NowVirus[VirusNumber - 1].State == 3) {//��״̬Ϊ����
			result[VirusNumber - 1].DeathNumber++; sum--;
			continue;
		}
		int tempNumber = VirusNumber;
		while (father = FindFather(root, NULL, tempNumber)) {//��Ǹò����������ȣ�ʹ�䲻���ٴθ�Ⱦ
			P3.HistoryVirus[father->Number - 1] = 1;
			tempNumber = father->Number;
		}
		crowd.push_back(P3);
	}
}

//�������죬�˵ĸ�Ⱦ״̬�ı䣬result��Ӧ�ı�,��������1��ʾ������������Ҫɾ�������򷵻�0
int VariationChange(Node* root,Person& P, vector<VirusState>& result, int VirusNumber, int NewVirusNumber,int t) {
	P.NowVirus[VirusNumber - 1].Exist = 0;
	//ԭ��״̬һ��Ϊ����
	int time = P.NowVirus[VirusNumber - 1].InfectTime;
	result[VirusNumber - 1].InfectNumber--;
	Node* p;//�ҵ����ΪVirusNumber��ָ��p
	if (VirusNumber == 1)p = root;
	else {
		p = FindFather(root, 0, VirusNumber);
		p = p->FirstChild;
		while (p->Number != VirusNumber)p = p->NextSibling;
	}
	if (time <= (p->LurkTime)*24) //����ԭ��Ǳ����
		result[VirusNumber - 1].LurkNumber--;
	else if (time > p->LurkTime*24 && time <= (p->LurkTime + t) * 24)  //����ԭ���ڱ�����
		result[VirusNumber - 1].OutBreakNumber--;
	else {//����ԭ���ڸ�����
		result[VirusNumber - 1].IsolateNumber--;
	}
	//�²�����Ⱦ״̬
	P.NowVirus[NewVirusNumber - 1].Exist = 1;
	P.NowVirus[NewVirusNumber - 1].InfectTime = 0;
	P.NowVirus[NewVirusNumber - 1].State = StateTransfer(root, VirusNumber);
	if (P.NowVirus[NewVirusNumber - 1].State == 1) {//��״̬Ϊ����
		result[NewVirusNumber - 1].RecoverNumber++;
		P.NowVirus[NewVirusNumber - 1].Exist = 0;
	}
	else if (P.NowVirus[NewVirusNumber - 1].State == 2) {//��״̬Ϊ����
		result[NewVirusNumber - 1].InfectNumber++;
		 //����Ӧ��Ǳ����
			result[NewVirusNumber - 1].LurkNumber++;
	}
	else {//��״̬Ϊ����
		result[NewVirusNumber - 1].DeathNumber++;
		return 1;
	}
	P.NowVirus[NewVirusNumber - 1].Isolation = 0;//δ����
	return 0;
}

//������Ⱥ�и�Ⱦ�ò�������,��Ӧ�ı�ÿ�ֲ����ĸ�������״̬������Ⱥ�ĸò�����Ⱦ״̬
void TravelCrowd(Node* root, vector<Person>& crowd, vector<VirusState>& result, Node* p,
	int t, int N, vector<int> VaccineVirus) {
	if (crowd.size() == 0)return;
	int VirusNumber = p->Number;//�������
	int VariationTime = p->VariationTime;
	int VaccineTime = p->VaccineTime;
	int AfterVariationTime = p->AfterVariationTime;
	int ChildNumber = p->ChildNumber;
	int LurkTime = p->LurkTime;
	float Infect = p->Infect;
	//������Ⱥ�и�Ⱦ�ò�������,���������ͳһ��Ⱦ�µ���
	vector<Person>::iterator iter = crowd.begin();
	for (; iter != crowd.end();) {
		if (iter->NowVirus[VirusNumber - 1].Exist) {
			//ԭ��״̬һ��Ϊ����
			int time = iter->NowVirus[VirusNumber - 1].InfectTime;
			result[VirusNumber - 1].InfectNumber--;
			if (time <= LurkTime*24) //����ԭ��Ǳ����
				result[VirusNumber - 1].LurkNumber--;
			else if (time > LurkTime*24 && time <= (LurkTime + t) * 24)  //����ԭ���ڱ�����
				result[VirusNumber - 1].OutBreakNumber--;
			else //����ԭ���ڸ�����
				result[VirusNumber - 1].IsolateNumber--;
			//�˵�״̬�仯
			iter->NowVirus[VirusNumber - 1].State = StateTransfer(root, VirusNumber);
			time = ++iter->NowVirus[VirusNumber - 1].InfectTime;
			if (iter->NowVirus[VirusNumber - 1].State == 1) {//��״̬Ϊ����
				result[VirusNumber - 1].RecoverNumber++;
				iter->NowVirus[VirusNumber - 1].Exist = 0;
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 2) {//��״̬Ϊ����
				result[VirusNumber - 1].InfectNumber++;
				if (time <= LurkTime * 24) //����Ӧ��Ǳ����
					result[VirusNumber - 1].LurkNumber++;
				else if (time > LurkTime * 24 && time <= (LurkTime+t) * 24) //����Ӧ�ڱ�����
					result[VirusNumber - 1].OutBreakNumber++;
				else {//����Ӧ�ڸ�����
					result[VirusNumber - 1].IsolateNumber++;
					iter->NowVirus[VirusNumber - 1].Isolation = 1;
				}
			}
			else if (iter->NowVirus[VirusNumber - 1].State == 3) {//��״̬Ϊ����
				result[VirusNumber - 1].DeathNumber++;
				iter=crowd.erase(iter); sum--;
				continue;
			}
			if (time % 24 == 0&& iter->NowVirus[VirusNumber - 1].Exist ) {//��Ⱦʱ�䵽�����������ſ��Ǳ���
				vector<int> variation;//���ڱ����Ⱦ��Ⱥ�б������
				for (int k = 0; k < result[VirusNumber - 1].InfectNumber; k++) variation.push_back(0);
				Variation(root, VirusNumber, ChildNumber, time / 24, result, variation, VaccineVirus);//�������
				int t = 0, count = 0;//��i�ǵڼ�����ȾVirusNumber��������
				for (; t < result[VirusNumber - 1].InfectNumber; t++) {
					if (crowd[t].NowVirus[VirusNumber - 1].Exist)count++;
					if (t == iter-crowd.begin())break;
				}
				int NewVirusNumber = 0;
				if (count != 0)NewVirusNumber = variation[count - 1];//��Ⱦ��Ⱥ�е�һ�����±�Ϊ0
				if (NewVirusNumber == 0)continue;//��Ⱦʱ��δ�ﵽ��̱���ʱ��ĺ��ӵ�Ҫ�󣬹ʲ������
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


//������������ͬʱ��ÿ�ֲ�������һ����Ⱥ
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

//����������������vector�洢�����Ƴ�����Ĳ�����ţ�1�Ų���û��·����û����������
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

//ɳ��ģ��,t�Ǳ����������ʱ��
void Simulation(Node* root, int N, int t, int days) {
	vector<Person> crowd;
	vector<VirusState> result;
	Person P1(N);
	VirusState VS1(0, 0, 0, 0, 0, 0);
	P1.HistoryVirus[0] = 1;
	P1.NowVirus[0].Exist = 1;
	P1.NowVirus[0].State = StateTransfer(root, 1);
	if (P1.NowVirus[0].State == 1) {//��״̬Ϊ����
		VS1.RecoverNumber=1;
		P1.NowVirus[0].Exist = 0;
	}
	else if (P1.NowVirus[0].State == 2) {//��״̬Ϊ����
		VS1.InfectNumber = 1;
		 //����Ӧ��Ǳ����
		VS1.LurkNumber=1;
	}
	else if (P1.NowVirus[0].State == 3) {//��״̬Ϊ����
		VS1.DeathNumber=1;
		P1.NowVirus[0].Exist = 0;//���������󲡶�Ҳ��������
	}
	crowd.push_back(P1);
	result.push_back(VS1);
	for (int j = 1; j < N; j++) {
		VirusState VS2;
		result.push_back(VS2);
	}
	Node* p = root;
	int hours = 1;
	vector<int> VaccineVirus;//���ڴ洢�����Ƴ�����Ĳ������
	cout << "����  �������  ��Ⱦ����  Ǳ��������  ����������  �������������  ��������  ��������" << endl;
	for (; hours <= days * 24; hours++) {//Ϊ���ڷ������涨ģ�����7��ĸ�Ⱦ���
		TravelVirus(root, p, crowd, result, t, N, VaccineVirus);//ÿСʱ����һ�β�����
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
	cout << "�����벡����������" << endl;
	int N;//�����������Ľ�����
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
	cout << "�����뱬���������ʱ�䣺" << endl;
	int t, days;
	cin >> t;
	cout << "������Ҫ�۲��������" << endl;
	cin >> days;
	Simulation(root, N, t, days);
	system("pause");
	return 0;
}