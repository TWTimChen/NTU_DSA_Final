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

    // 1. set [from]
    getline(inputFile, inputLine);
    split(inputLine, lineSplit);
    
    mail->from = lineSplit[1];

    // 2. set [date]
    getline(inputFile, inputLine);
    split(inputLine, lineSplit);
    
    string date;
    date = lineSplit[3];                        // year
    date += getMonthIndex(lineSplit[2]);        // month (in util.h)
    date += lineSplit[1];                       // day
    lineSplit[5].erase(lineSplit[5].begin()+2); // remove ":"
    date += lineSplit[5];                       // hour minite
    
    mail->date = date ;

    // 3. set [id]
    getline(inputFile, inputLine);
    split(inputLine, lineSplit);
    
    mail->id = stoi(lineSplit[1]);

    // 4. set [subject]
    getline(inputFile, inputLine);
    split(inputLine, lineSplit);
    
    mail->subject = lineSplit[1];

    // 5. set [to]
    getline(inputFile, inputLine);
    split(inputLine, lineSplit);
    
    mail->to = lineSplit[1];

    // 6. set [content]
    getline(inputFile, inputLine); // skip first line
    while (getline(inputFile, inputLine)) {
        // string to lower case
        if (inputLine.size()==0) break;
        transform(
            inputLine.begin(), 
            inputLine.end(), 
            inputLine.begin(), 
            ::tolower
        );
        mail->content.push_back(inputLine);
    }
}

void 
MailDB::queryOnlyExpr(string& expr)
{
    cout << "Execute Query :" << expr << endl;

}

void 
MailDB::queryWithCond(vector<string>& args)
{
    cout << "Execute Query :";
    for (int i=0; i<args.size(); i++)
        cout << args[i] << " ";
    cout << endl;
}