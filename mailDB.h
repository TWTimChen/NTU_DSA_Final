#ifndef MAIL_DB_H
#define MAIL_DB_H

#include <iostream>
#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <set>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
/////////////////////          Class Mail headers       /////////////////////
/////////////////////////////////////////////////////////////////////////////

class Mail
{
public: 
    Mail() {};
    Mail(unsigned& ID): id(ID) {};
    ~Mail() {};

    void print();

    // Enable MailDB access private data
    friend class MailDB;
private:
    bool searchContent(std::string& keyword);
    bool isInDate(std::string& dateS, std::string& dateE);
    void initContent();

    unsigned id;
    std::string subject;
    std::string from;
    std::string to;
    std::string date;
    std::string content;

    std::unordered_set<std::string> contentSet;
    unsigned len;
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

typedef std::pair<int, unsigned> LENGTH;

struct cmpLENGTH {
    bool operator()(const LENGTH& a, const LENGTH& b) const 
    {
        if (a.first>b.first)
            return true;
        else if (a.first==b.first)
            return a.second<b.second;
        else
            return false;
    }
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
    void remove(unsigned id);
    void longest();
    void query(std::vector<std::string>& args, MODE mode);
private:
    // We may need more member data or function hear

    // [add] 
    bool checkId(unsigned& id);
    void readfile(std::string& path, Mail* mail);
    void setLookUp(std::string& path, Mail* mail);
    // This set stores added path
    std::unordered_set<unsigned> fileAdded;

    // [longest]
    std::set<LENGTH, cmpLENGTH> lengthMap;

    // [query]
    // 1. query only with expression
    // 2. query with other conditions
    // You can costumize your situation
    // and don't forget to change the interface too!
    
    void queryWithCond(std::vector<std::string>& args);
    void queryOnlyExpr(std::string& expr);
    void parseExpr(std::string& expr, std::vector<OPERATOR>& preorder);
    void pre2post(std::vector<OPERATOR>& preorder, std::vector<OPERATOR>& postorder);
    
    // TO-Do:
    // A container store mails
    // and maybe other containers to store content
    std::vector<Mail*> mailVec;
    std::vector<Mail*> BufPrev;
    std::vector<Mail*> BufNext;
};

#endif