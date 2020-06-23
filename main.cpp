//
//  main.cpp
//  Mail
//
//  Created by 李季澄 on 2020/6/21.
//  Copyright © 2020 李季澄. All rights reserved.
//

#include <iostream>
#include <stack>
#include <cassert>
#include "mailDB.hpp"
#include "util.hpp"

using namespace std;

int main()
{
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
            // mode = DEFAULT -> do nothing
            // mode = LESS -> 賴昭蓉's
            // mode = MORE -> 李季澄's
            mailDB.query(args, MORE);
        }
        inputLine.clear();
    }
}
