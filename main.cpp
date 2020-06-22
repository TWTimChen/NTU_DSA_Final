#include <iostream>
#include <stack>
#include <cassert>
#include "mailDB.h"
#include "util.h"

using namespace std;

int main(){
    string inputLine;
    vector<string> lineSplit;
    MailDB mailDB;
    while(getline(cin, inputLine)){
        if(inputLine.size()==0) break;
        split(inputLine, lineSplit);
        if (lineSplit[0]=="add") {
            mailDB.add(lineSplit[1]);
        }
        else if (lineSplit[0]=="remove") {
            unsigned id = stoi(lineSplit[1]);
            mailDB.remove(id);
        }
        else if (lineSplit[0]=="longest") {
            mailDB.longest();
        }
        else if (lineSplit[0]=="query") {
            vector<string> args(lineSplit.begin()+1, lineSplit.end());
            mailDB.query(args);
        }
        inputLine.clear();
    }
}