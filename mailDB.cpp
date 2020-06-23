#include <fstream>
#include "./mailDB.h"
#include "./util.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////    Class Mail member functions    /////////////////////
/////////////////////////////////////////////////////////////////////////////

void
Mail::print()
{
    cout
    << "--------------------------------\n"
    << "Id: " << id << endl
    << "Subject: " << subject << endl
    << "From: " << from << endl
    << "To: " << to << endl
    << "Date: " << date << endl
    << "Length: " << len << endl
    << "Content: \n";

    for (auto iter=contentSet.begin(); iter!=contentSet.end(); iter++)
        cout << *iter << " ";
    cout << endl;

    cout
    << "--------------------------------\n"
    << endl;
}

bool 
Mail::searchContent(string& keyword)
{
    return contentSet.find(keyword) != contentSet.end();
}

bool
Mail::isInDate(string& dateS, string& dateE)
{
    return date>=dateS && date<=dateE;
}

void 
Mail::initContent()
{
    // create local set and content length
    vector<string> contentSplit;
    split(content, contentSplit);
    len = 0;
    for (int i=0; i<contentSplit.size(); i++) {
        contentSet.insert(contentSplit[i]);
        len += contentSplit[i].size();
    }
    split(subject, contentSplit);
    for (int i=0; i<contentSplit.size(); i++) {
        contentSet.insert(contentSplit[i]);
    }
    content.clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////   Class MailDB member functions   /////////////////////
/////////////////////////////////////////////////////////////////////////////

void 
MailDB::add(string& path)
{
    // if the path is readed, ignore this path
    int pos = path.find_last_of("l");
    unsigned id = stoi(path.substr(pos+1));
    if (checkId(id)) {
        cout << "-" << endl;
    }
    else {
        fileAdded.insert(id);

        // parse the email
        Mail* mail = new Mail;
        readfile(path, mail);
        mail->initContent();

        // insert LENTH to the map
        lengthMap.insert(LENGTH(mail->len, mail->id));
        // Insert the new mail in to the container
        mailVec.push_back(mail);

        cout << mailVec.size() << endl;

        #ifdef DEBUG
        mail->print(); 
        #endif
    }
}

void 
MailDB::remove(unsigned id)
{
    Mail* removeMail(0);
    for (auto iter=mailVec.begin(); iter!=mailVec.end(); ++iter)
        if ((*iter)->id == id) {
            removeMail = *iter;
            mailVec.erase(iter);

            auto iterA = fileAdded.find(id);
            if (iterA != fileAdded.end())
                fileAdded.erase(iterA);

            auto iterL = lengthMap.find(LENGTH(removeMail->len, removeMail->id));
            if (iterL != lengthMap.end())
                lengthMap.erase(iterL);

            delete removeMail;
            break;
        }

    if (removeMail) {
        cout << mailVec.size() << endl;
    }
    else
        cout << "-" << endl;
}

void 
MailDB::longest()
{
    if (lengthMap.size()==0) {
        cout << "-" << endl;
    }
    else {
        cout 
        << lengthMap.begin()->second << " "
        << lengthMap.begin()->first << endl;
    }
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
                    argsAug[0] = args[i].substr(3, args[i].size()-4);
                else if (args[i][1] == 't')
                    argsAug[1] = args[i].substr(3, args[i].size()-4);
                else if (args[i][1] == 'd')
                    argsAug[2] = args[i].substr(2);
            }
            argsAug[3] = args[args.size()-1];
            queryWithCond(argsAug);
            break;
        default:
            break;
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
        << endl;
        
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
                cleanStr(mail->from);
                break;
            case DATE:
                split(inputLine, lineSplit);
                date = lineSplit[3];                        // year
                date += getMonthIndex(lineSplit[2]);        // month (in util.h)
                if (lineSplit[1].size()==1)
                    date += "0";
                date += lineSplit[1];                       // day
                lineSplit[5].erase(lineSplit[5].begin()+2); // remove ":"
                date += lineSplit[5];                       // hour minite
                mail->date = date;
                cleanStr(mail->date);
                break;
            case ID:
                split(inputLine, lineSplit);
                mail->id = stoi(lineSplit[1]);
                break;
            case SUBJECT:
                split(inputLine, lineSplit, ':');
                mail->subject = lineSplit[1];
                cleanStr(mail->subject);
                break;
            case TO:
                split(inputLine, lineSplit);
                mail->to = lineSplit[1];
                cleanStr(mail->to);
                break;
            case CONTENT:
                mail->content = "";
                break;
            default:
                if (inputLine.size()==0) break;
                mail->content += inputLine;
                mail->content += " ";
                cleanStr(mail->content);
                break;
        }
        lineCount++;
    }
}

void 
MailDB::queryWithCond(vector<string>& args)
{
    #ifdef DEBUG
    cout << "Input Query :" << endl;
    for (int i=0; i<args.size(); i++)
        cout << i+1 << ". " << args[i] << endl;
    cout << endl;
    #endif
    // Condition Filter Pipline
    // 1. initialize buffer
    // 2. from -> to -> date filter
    // 3. pass expr to queryOnlyExpr()
    BufPrev = mailVec;

    if (args[0]!="") {
        cleanStr(args[0]); //tolower
        for (auto iter=BufPrev.begin(); iter!=BufPrev.end(); iter++)
            if ((*iter)->from == args[0])
                BufNext.push_back(*iter);
        swap(BufPrev, BufNext);
        BufNext.clear();
    }
    if (args[1]!="") {
        cleanStr(args[1]); //tolower
        for (auto iter=BufPrev.begin(); iter!=BufPrev.end(); iter++)
            if ((*iter)->to == args[1])
                BufNext.push_back(*iter);
        swap(BufPrev, BufNext);
        BufNext.clear();
    }
    if (args[2]!="") {
        string start, end;
        int pos = args[2].find("~");
        if (pos==0) {
            start = "000001010000";
            end = args[2].substr(1);
        }
        else if (pos==args[2].size()-1) {
            start = args[2].substr(0, pos);
            end = "999912312359";
        }
        else {
            start = args[2].substr(0, pos);
            end = args[2].substr(pos+1);
        }
        
        for (auto iter=BufPrev.begin(); iter!=BufPrev.end(); iter++)
            if ((*iter)->isInDate(start, end))
                BufNext.push_back(*iter);
        swap(BufPrev, BufNext);
        BufNext.clear();
    }

    #ifdef DEBUG
    cout << "After Condition Filters: ";
    for (auto iter=BufPrev.begin(); iter!=BufPrev.end(); iter++)
        cout << (*iter)->id << " ";
    cout << endl;
    #endif

    string expr = args[3];
    queryOnlyExpr(expr);

    // query output
    if (BufPrev.size()==0)
        cout << "-" << endl;
    else {
        set<int> output;
        for (auto iter=BufPrev.begin(); iter!=BufPrev.end(); iter++)
            output.insert((*iter)->id);
        auto iter=output.begin();
        cout << *iter;
        iter++;
        for (iter; iter!=output.end(); iter++)
            cout << " " << *iter;
        cout << endl;
    }
    BufPrev.clear();
    BufNext.clear();
}

void 
MailDB::queryOnlyExpr(string& expr)
{
    vector<OPERATOR> prefix;
    vector<OPERATOR> postfix;

    parseExpr(expr, prefix);
    #ifdef DEBUG
    cerr << "Prefix Expression: ";
    for (unsigned i=0; i<prefix.size(); i++)
        cerr << prefix[i].obj << " ";
    cerr << endl;
    #endif

    pre2post(prefix, postfix);

    #ifdef DEBUG
    cerr << "Postfix Expression: ";
    for (unsigned i=0; i<postfix.size(); i++)
        cerr << postfix[i].obj << " ";
    cerr << endl;
    #endif

    // Expression Filter Pipline
    stack<vector<int> > keyBuf;
    for (int op=0; op<postfix.size(); op++) {
        vector<int> vec1;
        vector<int> vec2;
        string key;
        switch (postfix[op].prec) {
            case STRING:
                vec1.resize(BufPrev.size());
                key = postfix[op].obj;
                for (int i=0; i<BufPrev.size(); i++) {
                    vec1[i] = BufPrev[i]->searchContent(key);
                }
                keyBuf.push(vec1);
                break;
            case NOT:
                vec1 = keyBuf.top();
                keyBuf.pop();
                for (int i=0; i<BufPrev.size(); i++)
                    vec1[i] = !vec1[i];
                keyBuf.push(vec1);
                break;
            case AND:
                vec1 = keyBuf.top();
                keyBuf.pop();
                vec2 = keyBuf.top();
                keyBuf.pop();
                for (int i=0; i<BufPrev.size(); i++)
                    vec1[i] = vec1[i]&&vec2[i];
                keyBuf.push(vec1);
                break;
            case OR:
                vec1 = keyBuf.top();
                keyBuf.pop();
                vec2 = keyBuf.top();
                keyBuf.pop();
                for (int i=0; i<BufPrev.size(); i++)
                    vec1[i] = vec1[i]||vec2[i];
                keyBuf.push(vec1);
                break;
            default:
                break;
        }
    }

    assert(keyBuf.size()==1);
    vector<int> filter = keyBuf.top();
    for (int i=0; i<BufPrev.size(); i++)
        if (filter[i])
            BufNext.push_back(BufPrev[i]);
    swap(BufPrev, BufNext);
}

void
MailDB::parseExpr(string& expr, vector<OPERATOR>& preorder)
{
    unsigned pos=0, pin;
    while (pos<expr.size()) {
        pin = pos;
        while (isalnum(expr[pos]))
            pos++;
        if(pos-pin) {
            string key = expr.substr(pin, pos-pin);
            cleanStr(key);
            preorder.push_back(OPERATOR(key, STRING));
        }
        
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
            << "Unrocognized operator: "
            << expr[pos]
            << endl;
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