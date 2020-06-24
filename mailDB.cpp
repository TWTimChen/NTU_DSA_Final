//
//  mailDB.cpp
//  Mail
//
//  Created by 李季澄 on 2020/6/23.
//  Copyright © 2020 李季澄. All rights reserved.
//

#include <fstream>
#include "mailDB.hpp"
#include "./util.hpp"
#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////    Class Mail member functions    /////////////////////
/////////////////////////////////////////////////////////////////////////////

bool
Mail::searchContent(string& keyword)
{
    return false;
}

void
Mail::print()
{
    cout
    << "--------------------------------\n"
    << "Id: " << id << "\n"
    << "Subject: " << subject << "\n"
    << "From: " << from << "\n"
    << "To: " << to << "\n"
    << "Date: " << date << "\n"
    << "Content: \n" << content << "\n";

    cout
    << "--------------------------------\n"
    << "\n";
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////   Class MailDB member functions   /////////////////////
/////////////////////////////////////////////////////////////////////////////

MailDB::MailDB()
{
    from_dict.reserve(10000);
    to_dict.reserve(10000);
    database.reserve(10000);
    
}

void
MailDB::add(string& path)
{
    // if the path is readed, ignore this path
    int pos = static_cast<int>(path.rfind("l"));
    unsigned id = stoi(path.substr(pos + 1));
    
    if (checkId(id))
    {
        cout << "-" << "\n";
    }
    else
    {
        cout << longest_set.size() + 1 << "\n" ;
        fileAdded.insert(id);

        Mail* mail = new Mail;
        readfile(path, mail);
//        cout << mail->id << "\n";
//        cout << "\n";

        // check mail information
        #ifdef DEBUG
//        mail->print();
        #endif
        
        string from = mail->get_from(), date = mail->get_date(), subject = mail->get_subject(), to = mail->get_to(), content = mail->get_content();
        // case insensitive (content will be converted later)
        for (int i = 0; i< static_cast<int>(from.size()); i++)
        {
            from[i] = tolower(from[i]);
        }
        for (int i = 0; i< static_cast<int>(to.size()); i++)
        {
            to[i] = tolower(to[i]);
        }

        // ID 須先轉為四位數 id4
        string id4 = to_string(mail->get_id());
        if (id4.size() == 1)
            id4 = "000" + id4;
        else if (id4.size() == 2)
            id4 = "00" + id4;
        else if (id4.size() == 3)
            id4 = "0" + id4;
        else if (id4.size() == 5) // 10000
            id4 = "0000";

        unordered_set< string > current_set; // 目前有的 unordered_set
    
        // from_dict
        if (from_dict.find(from) == from_dict.end()) // 新的寄件人
        {
            unordered_set< string > *id4_set = new unordered_set< string >({id4});
            pair < string, unordered_set<string>* > from_dict_data(from, id4_set);
            from_dict.insert(from_dict_data);
        }
        else // 已出現過的寄件人
        {
            from_dict[from]->insert(id4); // 在原有的 unordered_set 補上新郵件
        }
        
        // to_dict
        if (to_dict.find(to) == to_dict.end()) // 新的收件人
        {
            unordered_set< string > *id4_set = new unordered_set< string >({id4});
            pair < string, unordered_set<string>* > to_dict_data(to, id4_set);
            to_dict.insert(to_dict_data);
        }
        else // 已出現過的收件人
        {
            to_dict[to]->insert(id4);
        }
        
        // word_dict
        int first_index = 0, char_count = 0;
        string word, converted_content;
        // 先處理第一個字元
        content[0] = tolower(content[0]); //
        if ((content[0] < 'a' or content[0] > 'z') and (content[0] < '0' or content[0] > '9'))
            content[0] = ' ';
        else // 頭無空白
            converted_content += content[0];
        
        for (int index = 1; index < static_cast<int>(content.size()); index++) // start form 1
        {
            content[index] = tolower(content[index]);
            // not alphanumeric
            if ((content[index] < 'a' or content[index] > 'z') and (content[index] < '0' or content[index] > '9'))
                content[index] = ' '; // convert to white space
            if (content[index - 1] != ' ' or content[index] != ' ') // no continous white space
                converted_content += content[index];
        }
        if (converted_content[converted_content.size() - 1] == ' ') // 刪除尾空白
            converted_content = converted_content.substr(0, converted_content.size() - 1);
        for (int index = 0; index < static_cast<int>(converted_content.size()); index++)
        {
            if (converted_content[index] == ' ')
            {
                word = converted_content.substr(first_index, index - first_index);
                if (word_dict.find(word) == word_dict.end()) // new word
                {
                    unordered_set< string > *id4_set = new unordered_set< string >({id4});
                    pair < string, unordered_set<string>* > word_dict_data(word, id4_set);
                    word_dict.insert(word_dict_data);
                }
                else
                {
                    word_dict[word]->insert(id4);
                }
                first_index = index + 1; // first index of the next word
            }
            else if (index == static_cast<int>(converted_content.size()) - 1) // last character
            {
                word = converted_content.substr(first_index, index - first_index + 1);
                if (word_dict.find(word) == word_dict.end()) // new word
                {
                    unordered_set< string > *id4_set = new unordered_set< string >({id4});
                    pair < string, unordered_set<string>* > word_dict_data(word, id4_set);
                    word_dict.insert(word_dict_data);
                }
                else
                {
                    word_dict[word]->insert(id4);
                }
                char_count++;
            }
            else
                char_count++;
            
        }
        vector<int> char_count_id(2, 0);
        char_count_id[0] = char_count;
        if (id4 == "0000")
            char_count_id[1] = 0;
        else
            char_count_id[1] = 10000 - stoi(id4); // 為了讓 id 比較小的能排到最後面
        longest_set.insert(char_count_id);
        
        // subject
        string converted_subject;
        first_index = 0;
        // 先處理第一個字元
        subject[0] = tolower(subject[0]); //
        if ((subject[0] < 'a' or subject[0] > 'z') and (subject[0] < '0' or subject[0] > '9'))
            subject[0] = ' ';
        else
            converted_subject += subject[0];
        
        if (converted_subject[converted_subject.size() - 1] == ' ') // 刪除尾空白
            converted_subject = converted_subject.substr(0, converted_subject.size() - 1);
        
        for (int index = 1; index < static_cast<int>(subject.size()); index++) // start form 1
        {
            subject[index] = tolower(subject[index]);
            // not alphanumeric
            if ((subject[index] < 'a' or subject[index] > 'z') and (subject[index] < '0' or subject[index] > '9'))
                subject[index] = ' '; // convert to white space
            if (subject[index - 1] != ' ' or subject[index] != ' ') // no continous white space
                converted_subject += subject[index];
        }
        
        for (int index = 0; index < static_cast<int>(converted_subject.size()); index++)
        {
            if (converted_subject[index] == ' ')
            {
                
                word = converted_subject.substr(first_index, index - first_index);
                if (word_dict.find(word) == word_dict.end()) // new word
                {
                    unordered_set< string > *id4_set = new unordered_set< string >({id4});
                    pair < string, unordered_set<string>* > word_dict_data(word, id4_set);
                    word_dict.insert(word_dict_data);
                }
                else
                {
                    word_dict[word]->insert(id4);
                }
                first_index = index + 1; // first index of the next word
            }
            else if (index == static_cast<int>(converted_subject.size()) - 1) // last character
            {
                word = converted_subject.substr(first_index, index - first_index + 1);
                if (word_dict.find(word) == word_dict.end()) // new word
                {
                    unordered_set< string > *id4_set = new unordered_set< string >({id4});
                    pair < string, unordered_set<string>* > word_dict_data(word, id4_set);
                    word_dict.insert(word_dict_data);
                }
                else
                {
                    word_dict[word]->insert(id4);
                }
            }
        }
        
        // data 分別裝 From / Date / Subject / To / Content
        vector<string>* data = new vector<string>(5, "");
        (*data)[0] = from ;
        (*data)[1] = date;
        (*data)[2] = converted_subject;
        (*data)[3] = to;
        (*data)[4] = converted_content; // 更新 content
        pair < string, vector<string>* > database_data(id4, data);
        database.insert(database_data); // 新增這筆資料
        all_id.insert(id4);
    }
}

void
MailDB::remove(unsigned id)
{
    // Note:
    // Remember to delete the mail pointer
    //cout << "Execute Remove:" << id << "\n";
    
    auto iter = fileAdded.find(id);
    if (iter != fileAdded.end())
        fileAdded.erase(iter);
    
    string id4 = to_string(id);
    if (id4.size() == 1)
        id4 = "000" + id4;
    else if (id4.size() == 2)
        id4 = "00" + id4;
    else if (id4.size() == 3)
        id4 = "0" + id4;
    else if (id4.size() == 5) // 10000
        id4 = "0000";
    
    if (database.find(id4) == database.end()) // 信件不存在
        cout << "-" << "\n";
    else // 信件存在
    {
        cout << longest_set.size() - 1 << "\n";
        string from = (*database[id4])[0], date = (*database[id4])[1], subject = (*database[id4])[2], to = (*database[id4])[3],              content = (*database[id4])[4]; // database 中的所有資料皆已處理過
        database.erase(id4);
        unordered_set< string > current_set;
        
        // from_dict
        from_dict[from]->erase(id4);
        
        
        // to_dict
        to_dict[to]->erase(id4);

        
        // word_dict
        int first_index = 0, char_count = 0;
        string word;
        for (int index = 0; index < static_cast<int>(content.size()); index++)
        {
            if (content[index] == ' ') // space
            {
                word = content.substr(first_index, index - first_index);
                word_dict[word]->erase(id4);
                first_index = index + 1; // first index of the next word
            }
            else if (index == static_cast<int>(content.size()) - 1)
            {
                word = content.substr(first_index, index - first_index + 1);
                word_dict[word]->erase(id4);
                char_count++;
            }
            else
                char_count++;
        }
        
        // subject
        first_index = 0;
        for (int index = 0; index < static_cast<int>(subject.size()); index++)
        {
            if (subject[index] == ' ') // space
            {
                word = subject.substr(first_index, index - first_index);
                word_dict[word]->erase(id4);
                first_index = index + 1; // first index of the next word
            }
            else if (index == static_cast<int>(subject.size()) - 1)
            {
                word = subject.substr(first_index, index - first_index + 1);
                word_dict[word]->erase(id4);
            }
        }
        
        vector<int> char_count_id(2, 0);
        char_count_id[0] = char_count;
        if (id4 == "0000")
            char_count_id[1] = 0;
        else
            char_count_id[1] = 10000 - stoi(id4);
        longest_set.erase(char_count_id);
        all_id.erase(id4);
    }
}

void
MailDB::longest()
{
    // cout << "Find Longest:" << "\n";
    if (longest_set.size() == 0)
        cout << "-" << "\n";
    else
        cout << 10000 - (*longest_set.rbegin())[1] << " " << (*longest_set.rbegin())[0] << "\n";
}

void
MailDB::query(vector<string>& args, MODE mode)
{
    vector<string> argsAug(4);
    switch (mode) {
        case LESS:
            for (unsigned i=0; i<args.size()-1; i++)
                args[i] = args[i].substr(1);
            queryWithCond(args);
            break;
        case MORE:
            for (unsigned i=0; i<args.size()-1; i++){
                if (args[i][1] == 'f')
                    argsAug[0] = args[i].substr(2);
                else if (args[i][1] == 't')
                    argsAug[1] = args[i].substr(2);
                else if (args[i][1] == 'd')
                    argsAug[2] = args[i].substr(2);
            }
            argsAug[3] = args[args.size()-1];
            queryWithCond(argsAug);
            break;
        default:
            break;
    }
    string from = argsAug[0], to = argsAug[1], date = argsAug[2], expression = argsAug[3];
    
    if (from[0] == '"')
        from = from.substr(1, from.size() - 2);
    if (to[0] == '"')
        to = to.substr(1, to.size() - 2);
    
    for (int i = 0; i< static_cast<int>(from.size()); i++)
    {
        from[i] = tolower(from[i]);
    }
    for (int i = 0; i< static_cast<int>(to.size()); i++)
    {
        to[i] = tolower(to[i]);
    }
    
    vector<OPERATOR> preorder;
    vector<OPERATOR> postorder;

    parseExpr(expression, preorder);
    pre2post(preorder, postorder);
    
//    cerr << "Postorder expression: ";
//    for (unsigned i=0; i<postorder.size(); i++)
//        cerr << postorder[i].obj << " ";
//    cerr << "\n";
    
    // date
    string start_date = "000000000000", end_date = "999999999999"; // 一定會在這個範圍內
    if (date != " ") // 假設存為一字串（包含兩日期）
    {
        if (date.size() == 25)
        {
            start_date = date.substr(0, 12);
            end_date = date.substr(13, 12);
        }
        else if (date.size() == 13 and date[0] == '~')
            end_date = date.substr(1, 12);
        else if (date.size() == 13 and date[12] == '~')
            start_date = date.substr(0, 12);
    }
    // expression
    string word, ids;
    unordered_set<string> current_set, difference_set, bu_difference_set, next_set, common_set, id_set;
    stack< unordered_set<string> > id_stack;
    
    // 差集
    for (unordered_set<string>::iterator element = all_id.begin(); element != all_id.end(); element++)
    {
        if ((*database[*element])[1]  >= start_date and (*database[*element])[1] <= end_date)
        {
            difference_set.insert(*element); // 目前 database 中的所有 id
        }
    }
    
    for (unordered_map< string, vector<string>* >::iterator element = database.begin(); element != database.end(); element++)
    {
        if ((*((*element).second))[1] >= start_date and (*((*element).second))[1] <= end_date)
        {
            difference_set.insert((*element).first); // 目前 database 中的所有 id
        }
    }
    bu_difference_set = difference_set;
    for (int i = 0; i < static_cast<int>(postorder.size()); i++)
    {
        current_set.clear(); // 清空
        next_set.clear();
        common_set.clear();
        id_set.clear();
        word = "";
        
        if (postorder[i].obj != "!" and postorder[i].obj != "&" and postorder[i].obj != "|") // a variable
        {
            for (int j = 0; j < static_cast<int>(postorder[i].obj.size()); j++)
                word += tolower(postorder[i].obj[j]);
            if (word_dict.find(word) != word_dict.end()) // the word exists
            {
                current_set = *(word_dict[word]); // id
            }
            for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
            {
                if ((*database[*element])[1] >= start_date and (*database[*element])[1] <= end_date) // 檢查是否符合時間
                {
                    id_set.insert(*element);
                }
            }
            id_stack.push(id_set);
            current_set.clear(); // 清空
            next_set.clear();
            common_set.clear();
            id_set.clear();
        }
        else if (postorder[i].obj == "!")
        {
            current_set = id_stack.top(); // id only
            id_stack.pop();
            // pop specific elements
            for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
            {
                difference_set.erase(*element);
            }
            id_stack.push(difference_set);
            // push specific elements back
            
            difference_set = bu_difference_set; // 恢復原樣，待下次使用

            current_set.clear(); // 清空
            next_set.clear();
            common_set.clear();
            id_set.clear();
        }
        else if (postorder[i].obj == "&")
        {
            current_set = id_stack.top();
            id_stack.pop();
            next_set = id_stack.top();
            id_stack.pop();
            // insert common elements
            for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
            {
                if (next_set.find(*element) != next_set.end()) // a common element
                    common_set.insert(*element);
            }
            id_stack.push(common_set);
            current_set.clear(); // 清空
            next_set.clear();
            common_set.clear();
            id_set.clear();
        }
        else if (postorder[i].obj == "|")
        {
            current_set = id_stack.top();
            id_stack.pop();
            next_set = id_stack.top();
            id_stack.pop();
            // insert all elements
            for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
                next_set.insert(*element);
            id_stack.push(next_set);
            current_set.clear(); // 清空
            next_set.clear();
            common_set.clear();
            id_set.clear();
        }
    }
    
    // from
    unordered_set<string> from_id;
    if (from_dict.find(from) != from_dict.end()) // sender exists
    {
        for (unordered_set<string>::iterator element = (*from_dict[from]).begin(); element != (*from_dict[from]).end(); element++)
        {
            if ((*database[*element])[1] >= start_date and (*database[*element])[1] <= end_date)
                from_id.insert(*element);
        }
    }
    if (from != "") // sender constraint exists
    {
        current_set = id_stack.top(); // only id
        id_stack.pop();
        for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
        {
            if (from_id.find(*element) != from_id.end())
                common_set.insert(*element);
        }
        id_stack.push(common_set);
        current_set.clear(); // 清空
        next_set.clear();
        common_set.clear();
        id_set.clear();
    }
    
    // to
    unordered_set<string> to_id;
    if (to_dict.find(to) != to_dict.end()) // receiver exists
    {
        for (unordered_set<string>::iterator element = (*to_dict[to]).begin(); element != (*to_dict[to]).end(); element++)
        {
            if ((*database[*element])[1] >= start_date and (*database[*element])[1] <= end_date)
                to_id.insert(*element);
        }
    }
    
    if (to != "") // receiver constraint exsits
    {
        current_set = id_stack.top(); // only id
        id_stack.pop();
        for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
        {
            if (to_id.find(*element) != to_id.end())
                common_set.insert(*element);
        }
        id_stack.push(common_set);
        current_set.clear(); // 清空
        next_set.clear();
        common_set.clear();
        id_set.clear();
    }
    
    
    // answer
    current_set = id_stack.top();
    if (current_set.size() == 0)
        cout << "-" << "\n";
    else
    {
        set<int> answer;
        for (unordered_set<string>::iterator element = current_set.begin(); element != current_set.end(); element++)
            answer.insert(stoi(*element));
        
        int size = static_cast<int>(answer.size());
        for (set<int>::iterator element = answer.begin(); element != answer.end(); element++)
        {
            size--;
            cout << *element;
            if (size != 0)
                cout << " ";
        }
        cout << "\n";
    }
}

bool
MailDB::checkId(unsigned& id)
{
    return fileAdded.find(id) != fileAdded.end();
}

void
MailDB::readfile(string& path, Mail* mail)
{
    ifstream inputFile;
    string inputLine;
    vector<string> lineSplit;
    inputFile.open(path);
    
    if (inputFile.fail()) {
        cout
        << "Invalid path: "
        << path
        << "\n";
        
        delete mail;
        return;
    }

    unsigned lineCount=0;
    string date;

    while (getline(inputFile, inputLine)) {
        switch (lineCount) {
            case FROM:
                split(inputLine, lineSplit);
                mail->from = lineSplit[1];
                break;
            case DATE:
                split(inputLine, lineSplit);
                date = lineSplit[3];                        // year
                date += getMonthIndex(lineSplit[2]);        // month (in util.h)
                if (lineSplit[1].size() == 1)               // day
                    date += ("0" + lineSplit[1]);
                else
                    date += lineSplit[1];
                lineSplit[5].erase(lineSplit[5].begin()+2); // remove ":"
                date += lineSplit[5];                       // hour minite
                mail->date = date ;
                break;
            case ID:
                split(inputLine, lineSplit);
                mail->id = stoi(lineSplit[1]);
                break;
            case SUBJECT:
                split(inputLine, lineSplit);
                for (unsigned i = 1; i < lineSplit.size(); i++)
                {
                    mail->subject += lineSplit[i];
                    mail->subject += " ";
                }
                break;
            case TO:
                split(inputLine, lineSplit);
                mail->to = lineSplit[1];
                break;
            case CONTENT:
                mail->content = "";
                break;
            default:
                if (inputLine.size()==0) break;
                mail->content += inputLine;
                mail->content += " ";
                break;
        }
        lineCount++;
    }
}

void
MailDB::queryOnlyExpr(string& expr)
{
    vector<OPERATOR> preorder;
    vector<OPERATOR> postorder;

    parseExpr(expr, preorder);
    #ifdef DEBUG
//    cerr << "Preorder expression: ";
//    for (unsigned i=0; i<preorder.size(); i++)
//        cerr << preorder[i].obj << " ";
//    cerr << "\n";
    #endif

    pre2post(preorder, postorder);

    #ifdef DEBUG
    cerr << "Postorder expression: ";
    for (unsigned i = 0; i < (postorder.size()); i++)
        cerr << postorder[i].obj << " ";
    cerr << "\n";
    #endif

    // TO-DOs
}

void
MailDB::queryWithCond(vector<string>& args)
{

    #ifdef DEBUG
//    cout << "Input Query :" << "\n";
//    for (int i=0; i<args.size(); i++)
//        cout << i+1 << ". " << args[i] << "\n";
//    cout << "\n";
    #endif
    // TO-DOs

    string expr = args[args.size()-1];
    queryOnlyExpr(expr);

    // TO-DOs
}

void
MailDB::parseExpr(string& expr, vector<OPERATOR>& preorder)
{
    unsigned pos=0, pin;
    while (pos<expr.size()) {
        pin = pos;
        while (isalnum(expr[pos]))
            pos++;
        if(pos-pin)
            preorder.push_back(OPERATOR(expr.substr(pin, pos-pin), STRING));
        
        if (pos==expr.size())
            break;
        
        if (expr[pos]=='(')
            preorder.push_back(OPERATOR("(", L_PAREN));
        else if (expr[pos]==')')
            preorder.push_back(OPERATOR(")", R_PAREN));
        else if (expr[pos]=='!')
            preorder.push_back(OPERATOR("!", NOT));
        else if (expr[pos]=='&')
            preorder.push_back(OPERATOR("&", AND));
        else if (expr[pos]=='|')
            preorder.push_back(OPERATOR("|", OR));
        else {
            cerr
            << "Unrecognized operator: "
            << expr[pos]
            << "\n";
        }
        pos++;
    }
}

void
MailDB::pre2post(vector<OPERATOR>& preorder, vector<OPERATOR>& postorder)
{
    stack<OPERATOR> bufStack;
    bufStack.push(OPERATOR("", DUMMY));

    for (unsigned i=0; i<preorder.size(); i++) {
        switch (preorder[i].prec) {
            case L_PAREN:
                bufStack.push(preorder[i]);
                break;
            case R_PAREN:
                while (bufStack.top().prec != L_PAREN) {
                    postorder.push_back(bufStack.top());
                    bufStack.pop();
                }
                bufStack.pop();
                break;
            case STRING:
                bufStack.push(preorder[i]);
                break;
            case NOT:
                bufStack.push(preorder[i]);
                break;
            default:
                while (bufStack.top() <= preorder[i]) {
                    postorder.push_back(bufStack.top());
                    bufStack.pop();
                }
                bufStack.push(preorder[i]);
                break;
        }
    }
    while (bufStack.top().prec != DUMMY) {
        postorder.push_back(bufStack.top());
        bufStack.pop();
    }
}

