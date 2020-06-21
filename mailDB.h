#ifndef MAIL_DB_H
#define MAIL_DB_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <queue>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
/////////////////////          Class Mail headers       /////////////////////
/////////////////////////////////////////////////////////////////////////////

class Mail
{
public:
    Mail() { l=0; };
    Mail(int& ID): id(ID) { l=0; };
    ~Mail() {};

    void print();
    bool find(const std::string& keyword);

    // Enable MailDB access private data
    friend class MailDB;
private:
    int id;
    int l;
    std::string subject;
    std::string from;
    std::string to;
    std::string date;
    std::string content;
    std::unordered_set<std::string> content_set;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////        Class MailDB headers       /////////////////////
/////////////////////////////////////////////////////////////////////////////

enum READ_ORDER
{
    FROM = 0,
    DATE = 1,
    ID = 2,
    SUBJECT = 3,
    TO = 4,
    CONTENT = 5
};

enum PRECEDENCE
{
    STRING = 0,
    NOT = 1,
    AND = 2,
    OR = 3,
    R_PAREN = INT32_MAX-2,
    L_PAREN = INT32_MAX-1,
    DUMMY = INT32_MAX
};

struct OPERATOR
{
    OPERATOR() {};
    OPERATOR(std::string s, PRECEDENCE p): obj(s), prec(p) {};
    std::string obj;
    PRECEDENCE prec;

    bool operator<(const OPERATOR& other) const { return prec<other.prec; }
    bool operator<=(const OPERATOR& other) const { return prec<=other.prec; }
    bool operator>(const OPERATOR& other) const { return prec>other.prec; }
};


enum MODE
{
    DEFAULT,
    LESS,
    MORE
};

class MailDB
{
public:
    MailDB() {};
    ~MailDB() {};

    // Thers four member funtions is the interface of this class
    // which is designed for the four commands respectively
    void add(std::string& path);
    void remove(int id);
    void longest();
    void query(std::vector<std::string>& args, MODE mode);

private:
    // We may need more member data or function hear

    // [add]
    bool checkId(std::string& path);
    void readfile(std::string& path, Mail* mail);
    // This set stores added path
    std::unordered_set<std::string> fileAdded;

    // [longest]
    std::multimap<int,int,std::greater<int> > length;

    // [query]
    // 1. query only with expression
    // 2. query with other conditions
    void queryOnlyExpr(std::string& expr);
    void queryWithCond(std::vector<std::string>& args);
    void parseExpr(std::string& expr, std::vector<OPERATOR>& preorder);
    void pre2post(std::vector<OPERATOR>& preorder, std::vector<OPERATOR>& postorder);

    // function for query with conditions
    std::set<int> candidate;
    void print_candidate();
    std::set<int> find_by_date(const std::string& date_l,const std::string& date_u);
    std::set<int> find_by_from(const std::string& from);
    std::set<int> find_by_to(const std::string& to);
    std::vector<std::string> getdate(std::string & str);
    std::string getstring(std::string & str);

    // function for expression
    std::set<int> operator_not (std::string & keyword);
    std::set<int> operator_not (std::set<int> & sset);
    std::set<int> operator_with (std::string & keyword);
    std::set<int> operator_with (std::set<int> & sset );
    std::set<int> operator_or (std::string & keyword1, std::string & keyword2);
    std::set<int> operator_or (std::set<int> & sset1 ,std::set<int> & sset2);
    std::set<int> operator_and (std::string & keyword1, std::string & keyword2);
    std::set<int> operator_and (std::set<int> & sset1 ,std::set<int> & sset2);

    // information of mail
    std::map<int, Mail > mail_id;
    std::unordered_map<std::string, std::set<int> > mail_from;
    std::unordered_map<std::string, std::set<int> > mail_to;
    std::multimap<std::string, int > mail_date;

};

#endif