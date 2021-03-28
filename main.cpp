#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include "math.h"
using namespace std;
//读取命令
string command;
//当前时间
time_t current_time;
//读取文件名
const string File="/Users/kevin/Desktop/文件/app/bin/word_test/bank_learn.txt";
//错词库文件名
const string File_err="/Users/kevin/Desktop/文件/app/bin/word_test/bank_learn";
//总单词数量
int total_size;
//单词类
class WORD{
public:
    string ENG;
    string CHN;
    string chapter;
    int correct;
    int wrong;
    int conti;
    time_t time_log;
public:
    WORD();
    WORD input(ifstream &input);
    WORD input();
    void output(ofstream &output);
    void output();
    void optr(ofstream &output);
    int TIER();
};
//创建空的WORD
inline
WORD::WORD(){
    this->ENG="";
    this->CHN="";
    this->chapter="";
    this->correct=0;
    this->wrong=0;
    this->conti=0;
    this->time_log=time(NULL);
}
//文件输入
inline
WORD WORD::input(ifstream &input){
    input>>ENG>>CHN>>chapter>>correct>>wrong>>conti>>time_log;
    return *this;
}
//标准输入
inline
WORD WORD::input(){
    cin>>ENG>>CHN>>chapter;
    this->correct=0;
    this->wrong=0;
    this->conti=0;
    this->time_log=time(NULL);
    return *this;
}
//文件输出
inline
void WORD::output(ofstream &output){
output<<ENG<<"\t"<<CHN<<"\t"<<chapter<<"\t"<<correct<<"\t"<<wrong<<"\t"<<conti<<"\t"<<time_log<<endl;
}
//标准输出
inline
void WORD::output(){
    cout<<ENG<<"\t"<<CHN<<"\t"<<chapter<<endl;
}
//分级
inline
int WORD::TIER(){
    if (conti<3) return 1;
    if (conti<7) return 2;
    if (conti<10) return 3;
    if (conti<20) return 4;
    return 5;
}
//输出 英文中文位置
inline
void WORD::optr(ofstream &output){
    output<<ENG<<"\t"<<CHN<<"\t"<<chapter<<endl;
}
//词库
vector<WORD> word_bank;
//错词库
//vector<WORD> err_word_bank;
vector<bool> have_err;
//判断哪些考
vector<bool> toTest;
//判断考多少
int testNum=0;
//判断考的位置
vector<int> toTestPos;
//比较间隔日期
const time_t Interval[5]{
    0,86400,172800,604800,2592000
};
//帮助
const string HELP="this is a HELP file.\ncommands is shown below:\n    1.'-t' to start a test.\n    2.'-n' [ENG][CHN][info] to add a new word.\n    3.'-r' to renew the storage document.\n    4.'-e' to end the program.\n    5.'-s' [ENG] to search a word.\n    6.'-sc' [ENG][NUM] to search a similar word.\n";
//清空文件
void fileEmpty(const string fileName){
    fstream file(fileName, ios::out);
    file.close();
}
//文件名
inline
const string OptF(){
    string File_err_opt;
    time_t tt = time(NULL);
    tm* t=localtime(&tt);
    string time=to_string(t->tm_year+1900)+"."+to_string(t->tm_mon+1)+"."+to_string(t->tm_mday)+"-"+to_string(t->tm_hour)+"."+to_string(t->tm_min)+"."+to_string(t->tm_sec);
    File_err_opt=File_err+"_"+time+".txt";
    return File_err_opt;
}

//生成小于size的随机整数√
int RND(int _size){
    int opt;
    srand((int)time(NULL)*rand());
    opt=rand()%_size;
    return opt;
}
//初始化
void boot(){
    ifstream input(File);
    input>>total_size;
    word_bank.resize(total_size);
    have_err.resize(total_size);
    for (int i=0;i<total_size;i++){
        word_bank[i].input(input);
    }
    input.close();
}
//规范化&检查哪些要考
void check(){
    toTest.resize(total_size);
    testNum=0;
    for (int i=0;i<total_size;i++){
        if ((time(NULL)-word_bank[i].time_log)>Interval[word_bank[i].TIER()-1]){
            toTest[i]=true;
            testNum++;
        }else{
            toTest[i]=false;
        }
    }
}
//测试开始
void test(){
    check();
    cout<<"Test start!\n\n";
    if (testNum==0){
        cout<<"No word to test!\n";
        return;
    }
    bool use_hint=false;
    int choose=0;
    int test_right=0;
    int test_total=0;
    have_err.resize(total_size);
    for (int i=0;i<total_size;i++){
        have_err[i]=true;
    }
    while (testNum>0){
        toTestPos.resize(testNum);
        for (int i=0,j=0;i<total_size;i++){
            if (toTest[i]==true){
                toTestPos[j]=i;
                j++;
            }
        }
        choose=RND(testNum);
        cout<<word_bank[toTestPos[choose]].ENG<<"\tTier "<<word_bank[toTestPos[choose]].TIER()<<"\t";
        cin>>command;
        use_hint=false;
        if (toupper(command[0])=='H'){
            //给出提示
            word_bank[toTestPos[choose]].output();
            use_hint=true;
            cin>>command;
        }
        if (toupper(command[0])=='Y'){
            //回答正确
            test_right++;
            test_total++;
            word_bank[toTestPos[choose]].correct++;
            word_bank[toTestPos[choose]].conti++;
            word_bank[toTestPos[choose]].time_log=time(NULL);
            toTest[toTestPos[choose]]=false;
            testNum=testNum-1;
        }else if (toupper(command[0])=='N'){
            //回答错误
            test_total++;
            word_bank[toTestPos[choose]].wrong++;
            word_bank[toTestPos[choose]].conti=0;
            word_bank[toTestPos[choose]].time_log=time(NULL);
            have_err[toTestPos[choose]]=false;
        }else if (toupper(command[0])=='E'){
            //结束测试
            break;
        }
        if (!use_hint) word_bank[toTestPos[choose]].output();
        cout<<endl;
    }
    cout<<"total correct rate:"<<100*(double)test_right/(double)test_total<<"%."<<endl<<endl;
}
//清除时间戳
void clearT(){
    for (int i=0;i<total_size;i++){
        word_bank[i].time_log=0;
    }
}
//查词
void search(){
    cin>>command;
    for (int i=0;i<total_size;i++){
        if (command==word_bank[i].ENG){
            word_bank[i].output();
            return;
        }
    }
    cout<<"not in the word bank!\n";
}
//刷新
void refresh(){
    fileEmpty(File);
    ofstream out(File);
    string err_file_out=OptF();
    const char *err_file_out_pointer=err_file_out.data();
    ofstream out_err(err_file_out_pointer);
    out<<total_size<<endl;
    for (int i=0;i<total_size;i++){
        word_bank[i].output(out);
        if (have_err[i]==false){
            word_bank[i].optr(out_err);
        }
    }
    
}
//二进制里多少个1
int onesinbi(int _in){
    int _out=0;
    while (_in!=0){
        _out=_out+_in%2;
        _in=_in/2;
    }
    return _out;
}
//十进制转二进制
vector<int> DtoB(int _in){
    vector<int> _out;
    _out.resize(0);
    if (_in==0){
        _out.push_back(0);
    }
    while (_in!=0){
        _out.push_back(_in%2);
        _in=_in/2;
    }
    return _out;
}
//元音
const char oo[5]={
    'a','o','i','u','e'
};
//是否是元音
bool isoo(char __chr){
    for (int ___i=0;___i<5;___i++){
        if (__chr==oo[___i]){
            return true;
        }
    }
    return false;
}

//单词距离
double Dist(string _worda,string _wordb){
    string _sub="";
    string _main="";
    string _subo="";
    vector<int> pos;
    int contmax;
    int contmaxmax;
    double score=0;
    if (_worda.size()>=_wordb.size()){
        _main=_worda;
        _subo=_wordb;
    }else{
        _main=_wordb;
        _subo=_worda;
    }
    _sub.resize(_main.size());
    int Full=pow(2,_main.size());
    double scoremin=9999999;
    for (int __i=0;__i<Full;__i++){
        contmax=0;
        contmaxmax=0;
        score=0;
        if (onesinbi(__i)==_subo.size()){
            pos=DtoB(__i);
            for (int __j=0,__k=0;__j<_sub.size();__j++){
                if (pos[__j]==1){
                    _sub[__j]=_subo[__k];
                    __k++;
                    contmax++;
                    if (contmaxmax<contmax){
                        contmaxmax=contmax;
                    }
                }else{
                    _sub[__j]='-';
                    contmax=0;
                }
                if (_main[__j]!=_sub[__j]){
                    score=score+10;
                    if ((_main[__j]=='t'&&_sub[__j]=='d')||(_main[__j]=='d'&&_sub[__j]=='t')){
                        score=score-5;
                    }else if((_main[__j]=='k'&&_sub[__j]=='g')||(_main[__j]=='g'&&_sub[__j]=='k')){
                        score=score-5;
                    }else if((_main[__j]=='p'&&_sub[__j]=='b')||(_main[__j]=='b'&&_sub[__j]=='p')){
                        score=score-5;
                    }else if((_main[__j]=='f'&&_sub[__j]=='v')||(_main[__j]=='v'&&_sub[__j]=='f')){
                        score=score-5;
                    }else if(((_main[__j]=='c'&&_sub[__j]=='s')||(_main[__j]=='s'&&_sub[__j]=='c'))||((_main[__j]=='c'&&_sub[__j]=='z')||(_main[__j]=='z'&&_sub[__j]=='c'))||((_main[__j]=='s'&&_sub[__j]=='z')||(_main[__j]=='z'&&_sub[__j]=='s'))){
                        score=score-5;
                    }else if(isoo(_main[__j])&&isoo(_sub[__j])){
                        score=score-2;
                    }
                }
            }
            score=score/pow(contmaxmax,2);
            if (scoremin>score){
                scoremin=score;
            }
        
        }
    }
    return scoremin;
}
double Dist(WORD _worda,WORD _wordb){
    return Dist(_worda.ENG,_wordb.ENG);
}
double Dist(string _worda,WORD _wordb){
    return Dist(_worda,_wordb.ENG);
}
double Dist(WORD _worda,string _wordb){
    return Dist(_worda.ENG,_wordb);
}
int main() {
//读取单词表
    boot();
    check();
//操作
com:
    cout<<"->";
    cin>>command;
    //测试
    if (command=="-t"){
        test();
        goto com;
    }
    //帮助
    if (command=="-h"){
        cout<<HELP;
        goto com;
    }
    //清除时间戳
    if (command=="-c"){
        clearT();
        goto com;
    }
    //查词
    if (command=="-s"){
        search();
        goto com;
    }
    //查相近的词
    if (command=="-sc"){
        string sword;
        int num;
        int use;
        cin>>sword>>num;
        vector<double> dis;
        vector<int> posi;
        dis.resize(num);
        posi.resize(num);
        for (int i=0;i<num;i++){
            dis[i]=9999999;
            posi[i]=9999999;
        }
        double dist;
        for (int i=0;i<total_size;i++){
            dist=Dist(word_bank[i].ENG,sword);
            if (dis[0]>dist){
                dis[0]=dist;
                posi[0]=i;
            }
            for (int j=0;j<num-1;j++){
                if (dis[j]<dis[j+1]){
                    dist=dis[j];
                    dis[j]=dis[j+1];
                    dis[j+1]=dist;
                    use=posi[j];
                    posi[j]=posi[j+1];
                    posi[j+1]=use;
                }
            }
        }
        for (int i=0;i<num;i++){
            word_bank[posi[num-1-i]].output();
        }
        goto com;
    }
    //添加新词
    if (command=="-n"||command=="new"){
        //need more
        WORD ipt;
    redo:
        ipt.input();
        ipt.output();
        cout<<"is it correct?\t";
        cin>>command;
        if (toupper(command[0])=='Y'){
            word_bank.push_back(ipt);
            total_size++;
            goto com;
        }else{
            cout<<"try again\n";
            goto redo;
        }
    }
    //刷新
    if (command=="-r"||command=="renew"||command=="refresh"){
        refresh();
        goto com;
    }
    //结束程序
    if (command=="-e"||command=="end"){
        refresh();
        return 0;
    }
    cout<<"command not found!"<<endl;
    goto com;
    return 0;
}

