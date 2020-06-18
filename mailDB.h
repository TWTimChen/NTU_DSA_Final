#ifndef MAIL_DB_H
#define MAIL_DB_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <queue>
#include <sstream>

typedef std::pair<int, unsigned> LENGTH; 

class Mail
{
public: 
    Mail() {};
    Mail(unsigned& ID): id(ID) {};
    ~Mail() {};

    bool searchContent(std::string& keyword);
    void print();

    // Enable MailDB access private data
    friend class MailDB;
private:
    unsigned id;
    std::string subject;
    std::string from;
    std::string to;
    std::string date;
    std::vector<std::string> content;
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
    void query(std::vector<std::string>& args) 
    {
        if (args.size()>1) 
            queryWithCond(args);
        else if (args.size()==1)
            queryOnlyExpr(args[0]);
    }
private:
    // We may need more member data or function hear

    // [add]
    bool checkId(std::string& path);
    void readfile(std::string& path, Mail* mail);

    // [query]
    // 1. query only with expression
    // 2. query with other conditions
    // You can costumize your situation
    // and don't forget to change the interface too!

    void queryOnlyExpr(std::string& expr);
    void queryWithCond(std::vector<std::string>& args);
    void parseExpr(std::string& expr);

    // This heap is for the longest() function
    std::priority_queue<LENGTH, std::vector<LENGTH>, std::greater<int> > lengthHeap;

    // This set stores added path
    std::unordered_set<std::string> fileAdded;
    
    // TO-Do:
    // A container store mails
    // and maybe other containers to store content

};

#endif