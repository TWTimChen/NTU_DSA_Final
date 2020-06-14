#include<iostream>
#include<unordered_set>
#include<set>
#include<string>
#include<map>
#include<unordered_map>
#include<vector>
#include <algorithm> //intersection
using namespace std;


class email{
public:
    int id;
    string from;
    string to;
    string date;
    unordered_set<string> content_set;
    //unordered_set<string> content_uset;

    email(const int& i, const string& f, const string& t, const string& d, string& content):id(i),from(f),to(t),date(d){
        content_init(&content[0]);
    }
    email(){}
    void content_init(char* content);
    bool find(const string& keyword);

};
void email::content_init(char* content){
    char* p = content;
    //cout<<p;
    while(*p != '\0'){
        char* n = p;
        while(isalnum(*n)){
            ++n;
        }
        string s(p, n);
        content_set.emplace(s);
        //content_uset.emplace(s);

        while(!(isalnum(*n)) && *n != '\0'){
            ++n;
        }
        p = n;
    }
}
bool email::find(const string& keyword){
    return !(content_set.find(keyword) == content_set.end());
}

map<int, email > mail_id;
unordered_map<string, set<int> > mail_from;
unordered_map<string, set<int> > mail_to;
multimap<string, int > mail_date;

void add(const int & id, const string & from, const string & to, const string & date, string & content){

    email mail(id,from,to,date,content);
    mail_id[id] = mail;

    unordered_map<string, set<int> >::iterator it = mail_from.find(from);
    if(it == mail_from.end()){
        set<int> s;
        it = mail_from.insert(mail_from.begin(), pair<string, set<int> >(from,s));
    }
    it->second.insert( id );

    it = mail_to.find(to);
    if(it == mail_to.end()){
        set<int> s;
        it = mail_to.insert(mail_to.begin(), pair<string, set<int> >(to,s));
    }
    it->second.insert( id );

    mail_date.emplace(date,id);
}
set<int> find_by_date(const string& date_l,const string& date_u){
    set<int> result;
    multimap<string,int>::iterator itlow = mail_date.lower_bound(date_l);
    multimap<string,int>::iterator itup = mail_date.upper_bound(date_u);
    for(multimap<string,int>::iterator it = itlow; it!=itup ; it++){
        result.insert(it->second);
    }
    return result;

}
set<int> find_by_from(const string& from){
    unordered_map<string, set<int> >::iterator f = mail_from.find(from); //找mail_from有沒有這位sender
    if( f == mail_from.end()){
        set<int> s;
        return s;
    }
    else { return f->second;}
}

set<int> find_by_to(const string& to){
    unordered_map<string, set<int> >::iterator t = mail_to.find(to); //找mail_to有沒有這位sender
    if( t == mail_to.end()){
        set<int> s;
        return s;
    }
    else { return t->second;}
}

// 之後要把 query(條件) 合併。
// find_by_to, find_by_from, find_by_date 取得符合子條件的子set
// 取所有子條件的交集，得到候選組合set
// 組合set中，再根據 stack operation 找到所有id


/*
void query_d(const string& date_l,const string& date_u,const string keyword){

}

void query_ft(const string& from, const string& to, const string& keyword){

    set<int> set1 = find_by_to(to);
    set<int> set2 = find_by_from(from);
    set<int> candidate;
    set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(candidate, candidate.begin()) );

    set<int> ::iterator c = candidate.begin();
    int find_or_not = 0;
    for(c ; c!=candidate.end(); ++c){
        if( mail_id[*c].find( keyword )){
            find_or_not = 1;
            cout<< *c <<' ';
        }
    }
    if(find_or_not == 0) {cout<< '-'; }
    cout<< '\n';
}

void query_f(const string& from, const string& keyword){

    set<int> set1 = find_by_from(from);
    set<int> ::iterator c = set1.begin();
    int find_or_not = 0;
    for(c ; c!=set1.end(); ++c){
        if( mail_id[*c].find( keyword )){
            find_or_not = 1;
            cout<< *c <<' ';
        }
    }
    if(find_or_not == 0) {cout<< '-'; }
    cout<< '\n';
}

void query_t(const string& to, const string& keyword){
    set<int> set1 = find_by_to(to);
    set<int> ::iterator c = set1.begin();
    int find_or_not = 0;
    for(c ; c!=set1.end(); ++c){
        if( mail_id[*c].find( keyword )){
            find_or_not = 1;
            cout<< *c <<' ';
        }
    }
    if(find_or_not == 0) {cout<< '-'; }
    cout<< '\n';

}
*/

void remove(const int& id){
    string from = mail_id[id].from;
    string to = mail_id[id].to;
    string date = mail_id[id].date;

    mail_id.erase(id);
    mail_from[from].erase(id);
    mail_to[to].erase(id);

    multimap<string,int>::iterator itlow = mail_date.lower_bound(date);
    multimap<string,int>::iterator itup = mail_date.upper_bound(date);
    for (multimap<string,int>::iterator it=itlow; it!=itup; ++it){
        if(it->second == id){
            mail_date.erase(it);
            break;
        }
    }
}

int main(){
    string s = "hello test content valid! >< hey";
    //ps: 要避免內文傳 const string，不然之後要存set很麻煩
    add(1,"erin","anna","202006121240",s) ;

    s = "hi test today! >< #ok";
    add(2,"david","May","202006121248",s) ;

    s = "epic content like! >< hello dsa";
    add(3,"mike","tom","202006120900",s) ;

    s = "epic content like! >< hello dsa";
    add(4,"erin","tom","202006120900",s) ;

    s = "epic today like! >< ok dsa";
    add(5,"erin","tommy","202006120900",s) ;

    s = "epic today like! hello >< ok dsa";
    add(6,"erin","tommy","202006120900",s) ;

    s = "tubular part placed in a split-female die";
    add(7,"erin","tommy","202005050000",s) ;

    s = "and expanded with a polyurethane";
    add(8,"erin","tommy","201907142300",s) ;

    s = "oldest method of bending a tube is to first pack";
    add(9,"erin","tommy","201903142000",s) ;

    s = "its inside with loose particles and then bend it into a suitable fixture";
    add(10,"erin","tommy","201902142300",s) ;

    remove(1);
/*
    cout<<"debug"<<endl;

    query_ft("erin","tommy","hello");

    cout<<"debug"<<endl;

    query_ft("erin","tom","hello");

    cout<<"debug"<<endl;

    query_ft("erin","tommy","like");

    cout<<"debug"<<endl;

    query_f("erin","like");
*/

}


