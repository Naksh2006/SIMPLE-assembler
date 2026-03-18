// AUTHOR: Nakshtra Goyal
// ROLL NO: 2401CS44

// Declaration of Authorship
// This project submission is my original work. All source code,
// assembly programs, test files and documentation included in
// this submission were written and tested by me.

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <map>
#include<algorithm>

using namespace std;

// for error
struct errorasm{
    int line;
    string message;
} 
;
// parsed assembly line details
struct parsedl{
    int address;         
    string mnemonic;     
    string label;        
    string operand;     
    int linenum;
} ;

// Gloabal variable and functions
#define MAX_LINE_LENGTH 256
vector<parsedl>  codelines;             // stores parsed lines of assembly program
vector<uint32_t>machcode;               // generated machine code
unordered_map<string, int>symbolstable;     // symbol table
vector<errorasm> errorslist;            // list of errors during assembly
vector<string> warnings;                // list of warnings 
bool has_halt = false;                  // to track if the program contains halt
bool parsenumber(const string &text, int &value);

// removing comments from code after ;
void deletecomments(string &line){
    int x =line.find(';');

    if(x!=-1){
        line = line.substr(0, x);
    }
}
// trim leading nad trailing spaces from string
void extractparts(string &s){
    size_t i = 0;
    while(i < s.length() && isspace(s[i])){
        i++;
    }
    s.erase(0, i);
    while(!s.empty() && isspace(s[s.length() - 1])){
        s.pop_back();
    }
}

// parse a line of assembly into label,mnemonic,operand
parsedl parseline(string line, int linenum) {
    parsedl res;
    res.linenum = linenum;
    res.address = -1;
    // remove comments and trim
    deletecomments(line);
    extractparts(line);
        // if line becomes empty after removing comment
    if(line.size()==0){
       return res;
    }
    // finding label position 
    size_t pos = line.find(':');
    if(pos != string::npos){
       res.label = line.substr(0, pos);
        extractparts(res.label);
        line = line.substr(pos + 1);
        extractparts(line);
    }
    // saving values for mnemonic and operand after  extracting 
    if(line.length() > 0) {
        stringstream l;
        l << line;
        string tempmne;
        l >> tempmne;
        for(char &c : tempmne){
            c = tolower(c);
        }
        res.mnemonic = tempmne;
            // extract operand if exists
        if (!l.eof()) {
            string tempoperand;
            l>>tempoperand;
            res.operand = tempoperand;
                // detect extra operands
            string extra;
            if(l>>extra){
                errorslist.push_back({linenum,"Extra operand"});
                res.mnemonic = "";
            }
        }
    }

    return res;
}
// proper label format
// it should start with alphabet and then alphanumeric 
bool validlabel(const string &label){
    if(label.empty())
        return false;

    if(!isalpha(label[0]))
    return false;

    for(char c : label){
        if(!isalnum(c))
            return false;
    }

    return true;
}
// opcode information structure 
struct opcodedetails {
    string name;
    int opcode;
    bool hasoprnd;
    bool isbrnch;
    bool ispsudo;
};
// opcode table
vector<opcodedetails> opcodetable = {

    {"ldc", 0, true, false, false},
    {"adc", 1, true, false, false},
    {"ldl", 2, true, false, false},
    {"stl", 3, true, false, false},
    {"ldnl", 4, true, false, false},
    {"stnl", 5, true, false, false},

    {"add", 6, false, false, false},
    {"sub", 7, false, false, false},
    {"shl", 8,false, false, false },
    {"shr", 9, false, false, false },

    {"adj", 10, true,false, false},
    {"a2sp", 11, false, false, false},
    {"sp2a", 12, false, false, false },

    {"call", 13, true, true, false},
    {"return", 14, false, false, false},
    {"brz", 15, true, true, false},
    {"brlz", 16, true,true, false},
    {"br", 17, true, true, false},

    {"halt", 18, false,false, false},

    {"set", -1, true, false, true},
    {"data", -1, true, false, true}
} ;
// find opcode entry for
const opcodedetails* findopcode(const string &mnemonic) {
    string temp = mnemonic;
    for (char &c : temp) c = tolower(c); // Convert input to lowercase 
    for (const auto &entry : opcodetable) {
        if (entry.name == temp) { // Match against lowercase table entries
            return &entry;
        }
    }
    return nullptr;
}
//First pass
// building symbol table and assign addresses
void firstpass(){
    int curr_add = 0;

    for(size_t i = 0; i < codelines.size(); i++){
        parsedl &line = codelines[i];
        // process labels
        if(line.label.size() > 0){
            if(!validlabel(line.label)){
                errorslist.push_back({line.linenum,"Invalid label name"});
            }
            else if(symbolstable.find(line.label) != symbolstable.end()){
                errorslist.push_back({line.linenum,"Duplicate label"});
            }
            // SET pseudo instruction assigns constant value
            else if(line.mnemonic == "set"){
                int value;

                if(!parsenumber(line.operand,value)){
                    errorslist.push_back({line.linenum,"Invalid number format"});
                }else{
                    symbolstable[line.label] = value;
                }
            }
            else{
                symbolstable[line.label] = curr_add;
            }
        }

        const opcodedetails *info = findopcode(line.mnemonic);
        // increment address for valid instructions
        if(line.mnemonic.length() > 0 && line.mnemonic != "set" && info != NULL){
            line.address = curr_add;
            curr_add++;
        }
    }
}
// Helper functions for numeric prasising
bool checkdecimaldigi(char c) {
    return (c>='0' && c<='9');
}

bool checkoctaldigi(char c) {
    return (c>='0' && c<='7');
}

bool checkhexdigi(char c) {
    return ( (c>='0' && c<='9') ||
             (c>='a' && c<='f') ||
             (c>='A' && c<='F') );
}
// conert hex digit to numeric value
int hexvalue(char c) {
    if (c>='0' && c<='9')
        return c-'0';
    if (c>='A' && c<='F')
        return 10 + (c - 'A');
    if (c>='a' && c<='f') return 10 + (c - 'a');
    
    return -1;
}
// convert operand text to integer value
// supports decimal, octal, hex
bool parsenumber(const string &text, int &value) {

    if (text.empty())
        return false;

    size_t index = 0;
    bool negative = false;

    // sign handling
    if(text[index] =='+' || text[index] == '-'){
        negative = (text[index] == '-');
        index++;
    }

    if(index >= text.size())
        return false;

    int base = 10;
    // base detection
    if(text[index] == '0') {
        if(index + 1 < text.size() &&
           (text[index + 1]=='X' || text[index + 1]=='x')) {
            base = 16;
            index+= 2;

        } else if(text.size() > index + 1){
            base = 8;
            index += 1;
        }
    }
    if (index >= text.size())
        return false;

    long long res = 0;
    // convert string to integer
    for(; index<text.size(); index++){
        char c = text[index];
        int digitvalue = -1;
        if(base == 10){
            if (!checkdecimaldigi(c))
                return false;
            digitvalue = c - '0';
        }
        else if(base == 16){
            if (!checkhexdigi(c))
                return false;
            digitvalue = hexvalue(c);
        }
        else if(base == 8){
            if (!checkoctaldigi(c))
                return false;
            digitvalue = c - '0';
        }
        res = res*base + digitvalue;
    }

    if (negative)
        res = -res;

    value = static_cast<int>(res);
    return true;
}
// resolve operand value (number or label)
// a part function for second phase
bool resolveoperand(const parsedl &line,const opcodedetails *info,int &operandvalue){

    // instruction with no operand
    if (!info->hasoprnd){
        if(!line.operand.empty()){
            errorslist.push_back({line.linenum,"Unexpected operand"});
            return false;
        }
    operandvalue = 0;
    return true;
    }
    if(line.operand.empty()){
        errorslist.push_back({line.linenum, "Missing operand"});
        return false;
    }
    // checking if the operand is label.
    if (isalpha(line.operand[0])){
        if (symbolstable.find(line.operand) == symbolstable.end()) {
            errorslist.push_back({line.linenum,"Undefined label"});
            return false;
        }
        operandvalue = symbolstable[line.operand];
    }else{
        if(!parsenumber(line.operand, operandvalue)){
            errorslist.push_back({line.linenum,"Invalid number format"});
            return false;
        }
    }
    // if it is brach instruction then compute offeset.
    if (info->isbrnch){
        operandvalue = operandvalue - (line.address+1);
    }
    return true;
}

// second pass
// generate machine code
void secondpass()
{
    for (size_t i = 0;i<codelines.size(); i++){
        const parsedl &line = codelines[i];

        if (line.mnemonic.size()==0)
            continue;

        const opcodedetails *info = findopcode(line.mnemonic);
        if(info && info->name == "halt"){
            has_halt = true;
        }

        if(info == NULL)
        {
            errorslist.push_back({line.linenum, "Invalid mnemonic"});
            continue;
        }
        if (info->ispsudo && info->name == "set") continue;

        int operandvalue = 0;

        bool ok = resolveoperand(line, info, operandvalue);
        // warning for infinite loop branch
        if(ok && info->isbrnch){
            if(operandvalue == -1){
                string msg = "Line ";
                msg += to_string(line.linenum);
                msg += ": branch instruction will cause infinite loop";
                warnings.push_back(msg);
            }
        }
        if (!ok)
            continue;

        unsigned int instruction = 0;
        // data pseudo instruction
        if (line.mnemonic == "data"){
            instruction = operandvalue;
        }
        // combine opcode and operand
        else
        {
            unsigned int upper = (operandvalue & 0xFFFFFF);
            upper = upper << 8;
            unsigned int lower = (info->opcode & 0xFF);

            unsigned int combined = upper | lower;
            instruction = combined;
        }

        machcode.push_back(instruction);
    }
}
// write listing file (.lst)
void writelistingfile(const string &basename){
    // to open output file named basename .lst
    ofstream lst;
    lst.open(basename + ".lst");
    if (!lst){
        cout << "Error opening listing file\n";  // if cannot be opened print error and exit
        return;
    }
    size_t codeindex = 0;  //index to track machine code instruction
    int curraddress = 0;    // current memory addrs

    for(size_t i = 0; i < codelines.size(); i++)
    {
        const parsedl &line = codelines[i];
        const opcodedetails *info = findopcode(line.mnemonic); // opcode information from mnemonic
        // print label
        if (line.label.size() > 0){
            int addr = symbolstable[line.label];
            lst << hex << setw(8) << setfill('0') << addr; // label address in hexadecimal
            lst << "          ";
            lst << line.label << ":" << endl;
        }
        if (line.mnemonic.size() == 0)
            continue;           // skip empty lines


        if (info != NULL && info->name != "set"){
           lst << hex << setw(8) << setfill('0') << curraddress;        // address of instruction
           lst << " ";

            unsigned int value = machcode[codeindex];           // reterive machince code 
            lst << hex << setw(8) << setfill('0') << value;     // machine code in hexadecimal

            lst << " ";
            lst << line.mnemonic;           // printing mnemonic

            if(line.operand.size() > 0)
            {
                lst<<" ";
                lst<<line.operand;
            }
            lst << endl;
            codeindex = codeindex + 1;      // incrementing machine code
            curraddress = curraddress + 1;  // incrementing address for next instruction
        }
    }
    lst.close();
}
// to write log file containing errors/warnings
void writelogfile(const string &basename) {
    // logfile basename.log
    ofstream log(basename + ".log");

    if(!log){
        cout << "Error opening log file" <<endl;        // if file cannot be opeened report error
        return;
    }
    sort(errorslist.begin(), errorslist.end(), // sort errors by line number
     [](const errorasm &a, const errorasm &b){
         return a.line < b.line;
     });

    if(errorslist.size()==0) { // if no erors

    log << "Assembly successful" << endl;
        // print warnings if exist
    if(!warnings.empty()){
        log << endl;
        for(size_t i = 0; i < warnings.size(); i++){
            log << "Warning: " << warnings[i] << endl;
        }
    }
    }else{                          // for errors
        log << "Assembly failed" << endl;
        log << "\n";
        for(size_t i = 0;i < errorslist.size(); i++){
            const auto &err = errorslist[i];
            log << "Line ";
            log << err.line;
            log << ": ";
            log << err.message;
            log << "\n";
        }
    }
    log.close();
}
// to write object file containing machine code
void writeobjectfile(const string &basename){
    ofstream obj;
    obj.open(basename + ".o", ios::binary);
    // check if file opened successfully
    if (!obj){
        cout << "Error opening object file" << endl;
        return;
    }
    for(size_t i = 0;i < machcode.size(); i++){
        unsigned int code = machcode[i]; // get machine instructions
        const char *ptr = (const char *)&code;
        obj.write(ptr, sizeof(code)); // writing 4 bytes
    }

    obj.close();
}
// main function

int main(int argc, char *argv[]){
    // to check command line argument
    if(argc!=2){
        cout << "Usage: ./asm file.asm" <<endl;
        return 0;
    }
    string filename;
    filename = argv[1];
    ifstream input;
    input.open(filename);

    if(!input){
        cout << "Cannot open input file" << endl;
        return 0;
    }
    string line;
    int linenum = 0;
    // reading assembly file line by line
    while(true){
        if (!getline(input, line))
            break;

        linenum = linenum + 1;
        parsedl parsed = parseline(line, linenum);
        codelines.push_back(parsed);
    }

    input.close();
    // assembler passes
    firstpass();
    secondpass();
    // check halt existence
    if(!has_halt){
        warnings.push_back("Program has no HALT instruction (possible infinite execution)");
    }
    // extracting file base name
    size_t pos = filename.find_last_of('.');
    string basename = filename.substr(0, pos);

    // if errors exist only log file will be created
    if(errorslist.size()>0){
        writelogfile(basename);
        return 0;
    }
    // otherwise generate outputs
    writeobjectfile(basename);
    writelistingfile(basename);
    writelogfile(basename);

    return 0;
}