#include <fstream>
#include "./mailDB.h"
#include "./util.h"

using namespace std;


void toLowerCase(char & c){
    if(isalpha(c))
        c = tolower(c);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////    Class Mail member functions    /////////////////////
/////////////////////////////////////////////////////////////////////////////

bool
Mail::find(const string& keyword){
    return !(content_set.find(keyword) == content_set.end());
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
    << "Content: \n" << content << endl
    << "Content_set: \n";


    unordered_set<string> ::iterator it = content_set.begin();
    for(it; it!=content_set.end(); it++)
        cout<<*it<<' ';
    cout<<'\n';
    cout << "Length:" << l << endl;

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
    Mail* mail = new Mail;
    readfile(path, mail);

    //mail->print();
    mail = NULL;
}

void
MailDB::remove(int id)
{
    if(mail_id.find(id) == mail_id.end()){
        cout<< "-"<<'\n';
        return;
    }
    string from = mail_id[id].from;
    string to = mail_id[id].to;
    string date = mail_id[id].date;
    int l = mail_id[id].l;

    mail_id.erase(id);


    if(mail_from[from].size() == 1)
        mail_from.erase(from);
    else
        mail_from[from].erase(id);

    if(mail_to[to].size() == 1)
        mail_to.erase(to);
    else
        mail_to[to].erase(id);

    multimap<string,int>::iterator itlow = mail_date.lower_bound(date);
    multimap<string,int>::iterator itup = mail_date.upper_bound(date);
    for (multimap<string,int>::iterator it=itlow; it!=itup; ++it){
        if(it->second == id){
            mail_date.erase(it);
            break;
        }
    }

    multimap<int,int>::iterator itlow2 = length.lower_bound(l);
    multimap<int,int>::iterator itup2 = length.upper_bound(l);
    for (multimap<int,int>::iterator it = itlow2; it!=itup2; ++it){
        if(it->second == id){
            length.erase(it);
            break;
        }
    }

    cout << mail_id.size() <<'\n';
}

void
MailDB::longest()
{
    if (length.size() == 0)
        cout<<"-"<<'\n';
    else{
        multimap<int,int>::iterator it = length.begin();
        int smallest_id = it->second;
        int largest_length = it->first;
        for(it; it->first == largest_length; it++ ){
            //cout<< it->second;
            if(it->second < smallest_id)
                smallest_id = it->second;
        }
        cout<< smallest_id << " " <<largest_length <<'\n';
    }
}

void
MailDB::query(vector<string>& args)
{
    for (unsigned i=0; i<args.size()-1; i++){
        for_each(args[i].begin(), args[i].end(), toLowerCase);
        args[i] = args[i].substr(1);
    }
    for_each(args[args.size()-1].begin(), args[args.size()-1].end(), toLowerCase);
    queryWithCond(args);
}


void
MailDB::readfile(string& path, Mail* mail)
{
    ifstream inputFile;
    string inputLine;
    vector<string> lineSplit;

    inputFile.open(path,ios::in);

    if (inputFile.fail()) {
        cout
        << "Invalid path: "
        << path
        << endl;

        delete mail;
        return;
    }

    unsigned lineCount=0;
    string from, date, to;
    int id;

    while (getline(inputFile, inputLine)) {
        switch (lineCount) {
            case FROM:
                split(inputLine, lineSplit);
                from = lineSplit[1];
                for_each(from.begin(), from.end(), toLowerCase);
                mail->from = from;
                break;
            case DATE:
                split(inputLine, lineSplit);
                date = lineSplit[3];                        // year
                date += getMonthIndex(lineSplit[2]);        // month (in util.h)
                if(lineSplit[1].size() == 1)
                    date += ("0" + lineSplit[1]);
                else
                    date += lineSplit[1];                       // day
                lineSplit[5].erase(lineSplit[5].begin()+2); // remove ":"
                date += lineSplit[5];                       // hour minite
                mail->date = date ;
                break;
            case ID:
                split(inputLine, lineSplit);
                id = stoi(lineSplit[1]);
                if(mail_id.find(id)!=mail_id.end()){
                    cout<< "-"<<'\n';
                    return;
                }
                mail->id = id;
                break;
            case SUBJECT:
                split(inputLine, lineSplit);
                for(int i =0;i<lineSplit.size();i++){
                    mail->subject += lineSplit[i];
                    mail->subject += " ";
                }
                break;
            case TO:
                split(inputLine, lineSplit);
                to = lineSplit[1];
                for_each(to.begin(), to.end(), toLowerCase);
                mail->to = to;
                break;
            case CONTENT:
                mail->content = "";
                break;
            default:
                if (inputLine.size()==0) break;
                mail->content += " ";
                mail->content += inputLine;
                break;
        }
        lineCount++;
    }

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

    //content_set initiate
    // change uppercase to lowercase & ignore
    char* p = &(mail->content)[0];
    string s;
    int tmp = 0;
    while(*p!='\0'){
        if(isalnum(*p)){
            tmp++;
            if(isupper(*p))
                s+= tolower(*p);
            else
                s+= *p;
        }
        else if(!(isalnum(*p))){
            if (s.length()!=0){
                mail->content_set.emplace(s);
                s.clear();
            }
        }
        p++;
    }
    mail->content_set.emplace(s);
    mail->l=tmp;
    length.insert( pair<int,int>( tmp ,id ) );

//put into subject
    p = &(mail->subject)[0];
    s.clear();
    while(*p!='\0'){
        if(isalnum(*p)){
            if(isupper(*p))
                s+= tolower(*p);
            else
                s+= *p;
        }
        else if(!(isalnum(*p))){
            if (s.length()!=0){
                mail->content_set.emplace(s);
                s.clear();
            }
        }
        p++;
    }
    mail->content_set.emplace(s);

    mail_id[id] = *mail;
    cout << mail_id.size() << '\n';
}

set<int>
MailDB:: operator_not (string & keyword){
    set<int> subset = candidate;
    set<int>::iterator it = subset.begin();
    while(it != subset.end()){
        if( mail_id[*it].find(keyword) )
            it = subset.erase(it);
        else
            it++;
    }
    return subset;
}
void
MailDB:: operator_not (set<int> & sset){
    set<int> tmp = candidate;
    set_difference(candidate.begin(), candidate.end(), sset.begin(), sset.end(), inserter(tmp, tmp.begin()) );
    sset = tmp;
    // set<int>::iterator it1 = tmp.begin();
    // set<int>::iterator it2 = sset.begin();

    // for(it2; it2!=sset.end() && it1 != tmp.end(); it2++){
    //     while(*it1 < *it2 ){
    //         it1++;
    //         if(it1 == sset.end())
    //                 break;
    //     }
    //     if(*it2 == *it1){
    //         it1 = tmp.erase(it1);
    //     }
    // }
    // sset = tmp;
}

set<int>
MailDB:: operator_with (string & keyword){
    set<int> subset = candidate;
    set<int>::iterator it = subset.begin();
    while(it != subset.end()){
        if( !mail_id[*it].find(keyword) )
            it = subset.erase(it);
        else
            it++;
    }
    return subset;
}

set<int>
MailDB:: operator_or (string & keyword1, string & keyword2){
    set<int> subset = candidate;
    set<int>::iterator it = subset.begin();
    while(it != subset.end()){
        if( !( mail_id[*it].find(keyword1) || mail_id[*it].find(keyword2) ))
            it = subset.erase(it);
        else
            ++it;
    }
    return subset;
}

void
MailDB:: operator_or (string & keyword, set<int> & sset){
    set<int>::iterator it = candidate.begin();
    while(it!= candidate.end()){
        if( (mail_id[*it].find(keyword)) )
            sset.emplace(*it);
        ++it;
    }
}

void
MailDB:: operator_or (set<int> & sset1, set<int> & sset2){
    // set<int>::iterator it = sset1.begin();
    // set<int>::iterator it2 = sset2.begin();
    // for(it; it!=sset1.end(); it++){
    //     it2 = sset2.insert(it2, *it);
    // }
    set<int> tmp;
    set_union(sset1.begin(),sset1.end(),sset2.begin(),sset2.end(),inserter(tmp,tmp.begin()));
    sset2 = tmp;
}
set<int>
MailDB:: operator_and (string & keyword1, string & keyword2){
    set<int> subset = candidate;
    set<int>::iterator it = subset.begin();
    while(it != subset.end()){
        if( !( mail_id[*it].find(keyword1) && mail_id[*it].find(keyword2) ))
            it = subset.erase(it);
        else
            ++it;
    }
    return subset;
}

void
MailDB:: operator_and (string & keyword, set<int> & sset){
    set<int>::iterator it = sset.begin();
    while(it!= sset.end()){
        if( !(mail_id[*it].find(keyword)) )
            it = sset.erase(it);
        else
            ++it;
    }
}

void
MailDB:: operator_and (set<int> & sset1, set<int> & sset2){
    set<int> tmp;
    // set<int>::iterator it1; //smaller
    // set<int>::iterator it2;
    // if(sset1.size() < sset2.size()){
    //     it1 = sset1.begin();
    //     it2 = sset2.begin();
    //     for(it1; it1!=sset1.end() && it2 != sset2.end(); it1++){
    //         while(*it2 < *it1 ){
    //             it2++;
    //             if(it2 == sset2.end())
    //                     break;
    //         }
    //         if(*it2 == *it1){
    //             tmp.insert(*it1);
    //         }
    //     }
    // }
    // else{
    //     it1 = sset2.begin();
    //     it2 = sset1.begin();
    //     for(it1; it1!=sset2.end() && it2 != sset1.end(); it1++){
    //         while(*it2 < *it1 ){
    //             it2++;
    //             if(it2 == sset1.end())
    //                     break;
    //         }
    //         if(*it2 == *it1){
    //             tmp.insert(*it1);
    //         }
    //     }
    // }
    set_intersection(sset1.begin(),sset1.end(),sset2.begin(),sset2.end(),inserter(tmp,tmp.begin()));
    sset2 = tmp;
}

void
MailDB::queryOnlyExpr(string& expr)
{
    vector<OPERATOR> preorder;
    vector<OPERATOR> postorder;

    parseExpr(expr, preorder);
    pre2post(preorder, postorder);

    // TO-DOs
    stack<OPERATOR> keywords;
    stack<set<int> > subset;
    for(int i = 0 ; i < postorder.size(); i++){
        //cout<<"";
        if(postorder[i].prec == STRING ){
            keywords.push( postorder[i] );
        }

        else if(postorder[i].prec == NOT){
            if(keywords.top().prec == STRING){
                subset.push( operator_not(keywords.top().obj ) );
                keywords.pop();
                keywords.push(OPERATOR("",DUMMY));
            }
            else if(keywords.top().prec == DUMMY){
                operator_not( subset.top() );
            }
        }
        else if(postorder[i].prec == OR){
            OPERATOR tmp_top = keywords.top();
            keywords.pop();
            // case1 : string,set
            if ( tmp_top.prec == STRING ){
                if ( keywords.top().prec == DUMMY )
                    operator_or( tmp_top.obj, subset.top() );
            // case2 : string ,string
                else if (keywords.top().prec == STRING){
                    subset.push( operator_or( tmp_top.obj, keywords.top().obj ));
                    keywords.pop();
                    keywords.push(OPERATOR("",DUMMY));
                }
            }
            else if( tmp_top.prec == DUMMY){
            // case3 : set ,set
                if( keywords.top().prec == DUMMY){
                    set<int> tmp_set = subset.top();
                    subset.pop();
                    operator_or(tmp_set, subset.top());
                }
            // case4 : set ,string
                else if( keywords.top().prec == STRING){
                    operator_or( keywords.top().obj, subset.top() );
                    keywords.pop();
                    keywords.push(OPERATOR("",DUMMY));
                }
            }

        }
        else if(postorder[i].prec == AND){
            OPERATOR tmp_top = keywords.top();
            keywords.pop();
            // case1 : string,set
            if ( tmp_top.prec == STRING ){
                if ( keywords.top().prec == DUMMY )
                    operator_and( tmp_top.obj, subset.top() );
            // case2 : string ,string
                else if (keywords.top().prec == STRING){
                    subset.push( operator_and( tmp_top.obj, keywords.top().obj ));
                    keywords.pop();
                    keywords.push(OPERATOR("",DUMMY));
                }
            }
            else if( tmp_top.prec == DUMMY){
            // case3 : set ,set
                if( keywords.top().prec == DUMMY){
                    set<int> tmp_set = subset.top();
                    subset.pop();
                    operator_and(tmp_set, subset.top());
                }
            // case4 : set ,string
                else if( keywords.top().prec == STRING){
                    operator_and( keywords.top().obj, subset.top() );
                    keywords.pop();
                    keywords.push(OPERATOR("",DUMMY));
                }
            }
        }
    }
    set<int> result;
    if( keywords.top().prec == STRING )
        result = operator_with( keywords.top().obj );
    else
        result = subset.top();

    if(result.size() == 0)
        cout<< "-" ;

    else{
        set<int>::iterator it= result.begin();
        cout<< *it ;
        it++;
        for(it; it!=result.end(); ++it)
            cout<<" "<< *it ;
    }
}

void
MailDB::queryWithCond(vector<string>& args)
{
    candidate.clear();
    bool flag = 0;
    if(args.size() !=1){
        for (int i=0; i<args.size()-1; i++){
            if(args[i][0] == 'f'){
                string from = getstring(args[i]);
                set<int> f = find_by_from(from);
                if(!flag)
                    candidate = f;
                else{
                    set<int> tmp;
                    set_intersection(f.begin(), f.end(), candidate.begin(), candidate.end(), inserter(tmp, tmp.begin()) );
                    candidate = tmp; //之後改成in-place intersection應該可以快一些..
                }
                flag = 1;
            }
            else if(args[i][0] == 't'){
                string to = getstring(args[i]);
                set<int> t = find_by_to(to);

                if(!flag)
                    candidate = t;
                else{
                    set<int> tmp;
                    set_intersection(t.begin(), t.end(), candidate.begin(), candidate.end(), inserter(tmp, tmp.begin()) );
                    candidate = tmp; //之後改成in-place intersection應該可以快一些..
                }
                flag = 1;
            }
            else if(args[i][0] == 'd'){
                vector<string> date = getdate(args[i]);
                set<int> d = find_by_date(date[0],date[1]);
                if(flag == 0)
                    candidate = d;
                else{
                    set<int> tmp;
                    set_intersection(d.begin(), d.end(), candidate.begin(), candidate.end(), inserter(tmp, tmp.begin()) );
                    candidate = tmp;
                }
                flag = 1;
            }
        }
    }
    else{
        map<int, Mail> ::iterator it = mail_id.begin();
        set<int> :: iterator it2 = candidate.begin();//為了加速insert 因為mail_id本來就是有序的
        for(it ; it!= mail_id.end(); it++){
            it2 = candidate.insert(it2, it->first);
        }
    }
    if (candidate.size() == 0){
        cout<< "-" <<'\n';
        return;
    }

    string expr = args[args.size()-1];
    queryOnlyExpr(expr);
    cout << '\n';

}

set<int>
MailDB:: find_by_date(const string& date_l,const string& date_u){
    set<int> result;
    multimap<string,int>::iterator itlow = mail_date.lower_bound(date_l);
    multimap<string,int>::iterator itup = mail_date.upper_bound(date_u);
    for(multimap<string,int>::iterator it = itlow; it!=itup ; it++){
        result.insert(it->second);
    }
   return result;
}

set<int>
MailDB:: find_by_from(const string& from){
    unordered_map<string, set<int> >::iterator f = mail_from.find(from); //找mail_from有沒有這位sender
    if( f == mail_from.end()){
        set<int> s;
        return s;
    }
    else { return f->second; }
}

set<int>
MailDB:: find_by_to(const string& to){
    unordered_map<string, set<int> >::iterator t = mail_to.find(to); //找mail_to有沒有這位sender
    if( t == mail_to.end()){
        set<int> s;
        return s;
    }
    else { return t->second; }
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

vector<string>
MailDB:: getdate(string & str){
    vector<string> s(2);
    if(str[1]=='~'){
        s[0] = "000000000000";
        s[1] = str.substr(2,12);
    }
    else if(str.length() == 14){
        s[0] = str.substr(1,12);
        s[1] = "999912312359";
    }
    else{
        s[0] = str.substr(1,12);
        s[1] = str.substr(14,12);
    }
    return s;
}


string
MailDB:: getstring(string & str){
    string s=str.substr(2,str.length()-3);
    return s;
}

