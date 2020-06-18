#include <fstream>
#include "./mailDB.h"
#include "./util.h"

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
    << "Id: " << id << endl
    << "Subject: " << subject << endl
    << "From: " << from << endl
    << "To: " << to << endl
    << "Date: " << date << endl
    << "Content: " << endl;

    for (int i=0; i<content.size(); i++)
        cout << content[i] << endl;

    cout 
    << "--------------------------------\n"
    << endl;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////   Class MailDB member functions   /////////////////////
/////////////////////////////////////////////////////////////////////////////

void 
MailDB::add(string& path)
{
    // if the path is readed, ignore this path
    if (checkId(path)) {
        cout << "-" << endl;
    }
    else {
        fileAdded.insert(path);

        Mail* mail = new Mail;
        readfile(path, mail);
        cout << mail->id << endl;
        cout << endl;

        // check mail information
        #ifdef DEBUG
        mail->print(); 
        #endif

        // TO-Do:
        // Insert the new mail in to the container
    }
}

void 
MailDB::remove(unsigned id)
{
    // Note:
    // Remember to delete the mail pointer
    cout << "Execute Remove :" << id << endl;
}

void 
MailDB::longest()
{
    cout << "Execate Remove :" << endl;
}

bool
MailDB::checkId(string& path)
{
    return fileAdded.find(path) != fileAdded.end();
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
                break;
            case DATE:
                split(inputLine, lineSplit);
                date = lineSplit[3];                        // year
                date += getMonthIndex(lineSplit[2]);        // month (in util.h)
                date += lineSplit[1];                       // day
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
                mail->subject = lineSplit[1];
                break;
            case TO:
                split(inputLine, lineSplit);
                mail->to = lineSplit[1];
                break;
            case CONTENT:
                break;
            default:
                if (inputLine.size()==0) break;
                transform(
                    inputLine.begin(), 
                    inputLine.end(), 
                    inputLine.begin(), 
                    ::tolower
                );
                mail->content.push_back(inputLine);
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
    cerr << "Preorder expression: ";
    for (unsigned i=0; i<preorder.size(); i++)
        cerr << preorder[i].obj << " ";
    cerr << endl;
    #endif

    pre2post(preorder, postorder);

    #ifdef DEBUG
    cerr << "Postorder expression: ";
    for (unsigned i=0; i<postorder.size(); i++)
        cerr << postorder[i].obj << " ";
    cerr << endl;
    #endif
}

void 
MailDB::queryWithCond(vector<string>& args)
{
    cout << "Execute Query :";
    for (int i=0; i<args.size(); i++)
        cout << args[i] << " ";
    cout << endl;
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