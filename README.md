# Virus_epidemic
A project applying multiple data structures

Report 

1.需求分析

（1）给定两个病毒的编号，找出一个变异体，这个变异体是这两个病毒在分化之前的最近公共祖先。 
分析：即给出两个树结点的关键码，求出这两个结点的最近公共祖先

（2）若科学家沿某一路径研制疫苗时间比病毒沿某一路径变异所需要的时间短， 则该病毒不会再进行变异。 

分析：给出了叶子结点的限制，即叶子结点必须满足疫苗研制时间比变异时间短

（3）假定所有变异和疫苗研究可以并行进行，即每种病毒在诞生后，经过相应时间后会变异出其所有孩子结点的病毒(病毒会分头行动，并行变异)，假定研制疫苗不一定需要这种病毒已经出现，请编写程序计算每种变异体的疫苗是否能在该变异出现前研制成功，若不能，会落后多长时间。 

分析：计算从根结点到该结点的路径长度，即为该病毒变异时间，再与该病毒变异体的疫苗研制时间比较，若大于该病毒变异体的疫苗研制时间，则能在该种变异体的疫苗是否能在该变异出现前研制成功，否则不能，差值即为落后时间。

（4）对病毒在人群中的传播和变异进行一次沙箱模拟，以天为单位输出，以小时为每一次迭代的粒度。规则如下： 
①潜伏期为整数，以天为单位
②传染能力为一个实数，即每个感染者每小时会传染多少易感者，为简化问题，假定感染者之间互相传播也计算在内
③潜伏期的传染能力为爆发期的十分之一，爆发之后患者将被隔离，隔离后将无法传染，爆发到隔离的时间由输入决定，为一个定值
④致死率为一个三元组（每个感染的人每小时状态会发生一次转移，分别是康复、持续以及死亡，转移到持续状态的即为继续治疗状态，一小时后进行下一次状态转移。 指当前患病的人群第二天在下一天会康复，继续患病还是死亡的概率）
⑤在某种病毒感染到达相应天数后，则相应感染人群携带的病毒即会有 1/k 的概率变异为对应的病毒，该天数包括潜伏期。例: 某个病毒有3个变异体孩子结点，变异时间分别为2、2、3，则在第二天的时候，所有携带该病毒的人群将进行两次概率为1/3的随机，仅第一次随机到的变异为孩子1，仅第二次随机到的变异为孩子2，两次都随机到则等概率随机变异为1和2的一种，在第三天的时候，所有仍携带该病毒且未变异的人群将进行三次概率为1/3的随机，仅随机成功一次的变异为对应病毒，否则随机到了哪几个变异，就等概率随机选择其中一个进行变异。变异概率1/k仅有孩子数量决定，即无论变异体是否到达变异时间，这个k都固定为该结点的孩子数量 
⑥感染孩子结点的病毒并康复后不会再次感染这种病毒及其所有祖先，但是有可能感染除此以外的变异体。 
⑦bonus（选做）：加入疫苗的研制，当某种疫苗研制成功后，将不会再有人感染相应病毒。 
⑧bonus（选做）：将沙箱进行分割，即存在多个沙箱，沙箱之间会有人员流动，如每天随机交换 10%的人员，不同沙箱从爆发到隔离的时间不同。 
请自行决定输入方式，以天为粒度输出每一天每种病毒的感染、康复以及死亡的人数，感染可分为潜伏期、爆发期以及隔离救治期。 

分析：需要为病毒建立存储人群感染情况的类，以及为每个人建立存储所有病毒感染情况的类，需要每小时遍历一次病毒树的每个结点，并且对每个病毒遍历一遍人群，相应改变人群的感染状态，同时记录下病毒的感染数据，存储到result中，便于后续输出，还要每小时进行传染和变异。设计时考虑第一条bonus：加入疫苗的研制，当某种疫苗研制成功后，将不会再有人感染相应病毒。 


2.概要设计 

（1）
结点Node：存储病毒编号信息和变异到该病毒的路径的信息结点
Insert函数：将p所指的结点插入到root为根结点的树中的对应位置，即作为id的孩子,插入成功返回true
FindFather函数：在左孩子右兄弟的存储下利用前序遍历，实现在root为根结点的树中找到n1的父结点并返回指向父结点的指针,若没找到，返回NULL，其中定义了变量father是root的父结点
FindChild函数：左孩子右兄弟的存储下利用前序遍历实现在root为根结点的树中找到编号n1的孩子结点返回true
LCA函数：在root为根结点的树中找到n1,n2的最近公共祖先，输出编号

（3）
在（1）的基础上，定义了
SumVariationTime函数：求编号n1的结点的变异出现时间，实际是求从根结点到n1结点路径长,time表示原始根结点到root的变异出现时间
IsTimely函数：利用队列的层次序遍历判断各个变异体的疫苗是否能在该变异出现前研制成功。按编号从小到大的顺序依次输出，若能，输出yes, 若不能，输出no并输出会落后多长时间

（4）
在（1）的基础上，定义了
新的病毒树结点Node：依次存储编号Number,到达该结点的路径上的变异时间VariationTime,研制疫苗时间VaccineTime,疫苗研制成功后变异时间AfterVariationTime,孩子数量ChildNumber,潜伏期LurkTime，（潜伏期）传染能力Infect，致死率（康复概率Recover、持续概率Remain、死亡概率Death
Virus类：存储每个人所感染的病毒状态信息，Exist为0表示该病毒在该人体内不存在，为1表示该病毒在该人体内存在；InfectTime表示感染时间，State为1表示康复，为2表示持续，为3表示死亡；Isolation为1表示隔离，为0表示未隔离
Person类：包含了人群中每个个体目前感染病毒信息Virus和历史感染病毒编号。vector<Virus> NowVirus中下标表示病毒编号-1；vector<int> HistoryVirus中下标表示病毒编号-1,为0表示没感染过，初始全部置为0
VirusState类：存储了每种病毒的当前状态InfectNumber表示此时的感染人数, LurkNumber表示此时的潜伏期人数, OutBreakNumber表示此时的爆发期人数, IsolateNumber表示此时的隔离期人数, RecoverNumber表示到目前为止的康复人数, DeathNumber表示到目前为止的死亡人数
Variation函数：人群感染的病毒随机变异,输入病毒编号，孩子数，感染时间,存储感染人群中变异情况到引用variation中，variation下标为感染人群中的序号-1,值表示变异成的病毒序号，若已研制出疫苗，变异时间相应改变。先计算感染时间超过孩子的变异时间的孩子数量，即要随机的次数RandTime，用一个vector保存可能被随机到的变异体编号，然后进行RandTime次随机，第一次被随机到的人即变异为该变异体，再次被随机到的人再进行一次随机，随机决定是保留原来的变异体还是新被随机到的变异体
StateTransfer函数：实现人感染相应病毒的状态随机变化。根据输入的致死率，随机生成感染状态。
InfectOthers函数：实现传染其他人,即在crowd中再找出Infect个没感染过编号为VirusNumber的病毒的人，使其感染,若已研制出疫苗的病毒不再感染。先算出需要新感染的人数，在现有人群中找未感染过该病毒及其祖先的人，使其感染，并随机出状态，相应记录加入result中，同时标记该病毒及其祖先，使其不会再次感染。若原有人群中的未感染人数小于需要新感染的人数，则扩大当前人群，但是总人数始终小于十万人
gcd函数：求最大公约数
lcm函数：求最小公倍数
StateTransfer函数：实现人感染相应病毒的状态变化
VariationChange函数：病毒变异后，改变人的感染状态和result。先找到人原来的状态，从result中减去相应的记录，再在人和result中加上变异后的病毒的记录
TravelCrowd函数：用于遍历人群中感染该病毒的人,相应改变每种病毒的各项人数状态，和人群的该病毒感染状态。先遍历人群中感染该病毒的人,，在循环中先判断该人原来的状态，在result中减去相应的记录，调用StateTransfer函数改变该人的状态，根据新的状态在result中加上相应的记录，然后若该人的该病毒的感染时间到达整天数，才考虑变异，调用Variation函数得到存储感染人群中变异情况的vector variation，再调用VariationChange函数，相应改变人的感染状态和result，遍历完后再调用InfectOthers函数统一感染新的人。
TravelVirus函数：利用前序遍历实现遍历病毒树，同时调用TravelCrowd函数对每种病毒遍历一次人群
TravelVaccine函数：层次序遍历病毒树，vector存储已研制出疫苗的病毒编号
Simulation函数：实现沙箱模拟,t是爆发到隔离的时间，days是观察天数


3.调试分析  

（1）
测试数据：第一行输入病毒树结点个数N；接下来N-1行依次输入相应编号变异体的父结点编号，由父结点变异为该变异体的变异时间，科学家沿这一路径研制疫苗所需要的时间，以及在疫苗研制成功后病毒沿这一路径变异所需要的时间。

7
1 1 7 2
1 1 3 2
2 2 1 3
2 1 5 2
3 2 1 3
5 2 1 3
  

时间复杂度分析：
由给定编号找到相应病毒的指针的时间复杂度为O(log2(n)),循环找n1病毒的父结点，在父结点为根的子树中找n2的时间复杂度为O((log2(n))^2)

算法改进：
算法中多次运用了父结点，若采用三叉链表，在每个结点中存储指向父结点的指针，算法将更简洁，时间复杂度可以降低为O(log2(n))

（3）
测试数据：第一行输入病毒树结点个数N；接下来N-1行依次输入相应编号变异体的父结点编号，由父结点变异为该变异体的变异时间，科学家沿这一路径研制疫苗所需要的时间，以及在疫苗研制成功后病毒沿这一路径变异所需要的时间。

7
1 1 7 2
1 1 3 2
2 2 1 3
2 1 5 2
3 2 1 3
5 2 1 3


时间复杂度分析：
计算从根结点到n1结点路径长的时间复杂度为O(log2(n)),比较n-1个结点的变异时间与路径长的时间复杂度为O(n),所以总的时间复杂度为O(n*log2(n))

(4)
测试数据：第一行输入病毒树结点个数N；第二行输入根结点的孩子数量,潜伏期，（潜伏期）传染能力，致死率（康复、持续、死亡的概率）；接下来N-2行依次输入相应编号变异体的父结点编号，由父结点变异为该变异体的变异时间，科学家沿这一路径研制疫苗所需要的时间，以及在疫苗研制成功后病毒沿这一路径变异所需要的时间，该变异体的孩子数量,潜伏期，（潜伏期）传染能力，致死率（康复、持续、死亡的概率）

7
2 1 1 0.1 0.8 0.1
1 1 7 2 2 1 1 0.1 0.8 0.1
1 1 3 2 1 1 1 0.1 0.8 0.1
2 2 1 3 0 1 1 0.1 0.8 0.1
2 1 5 2 1 1 1 0.1 0.8 0.1
3 2 1 3 0 1 1 0.1 0.8 0.1
5 2 1 3 0 1 1 0.1 0.8 0.1


时间复杂度分析：
每小时都需要遍历一遍病毒变异树，遍历病毒变异树时又需要遍历一遍整个人群中感染该病毒的人,相应改变每种病毒的各项人数状态，和人群的该病毒感染状态，然后还要考虑感染新的人，需要再次遍历人群，其他均为常数或可忽略不计，若病毒种数为N，病毒树高为H，人数为P，观察时间为D（天），则时间复杂度为O(24*D*N*)

