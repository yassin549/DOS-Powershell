#include <iostream>
#include <list>
#include <iterator>
#include <string>
#include <queue>
#include <conio.h>
#include <windows.h>
#include <stack>
using namespace std;

// gotoxy function
void gotoxy(int column, int line)
{
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        coord);
}

// files class
class Files
{
public:
    string name;
    string data;
    int size;

    Files(string _name, string _data)
    {
        name = _name;
        data = _data;
        size = data.length();
    }

    void print()
    {
        cout << data << endl;
    }

    bool operator<(const Files &other) const
    {
        return size < other.size;
    }
};

// directory class
class Directory
{
public:
    string name;
    string path;
    list<Directory> *directories;
    list<Files> *files;
    Directory *parent;

    Directory(string _name, Directory *_parent)
    {
        name = _name;
        parent = _parent;
        path = setPath();

        directories = new list<Directory>();
        files = new list<Files>();
    }

    void addFile(string _name, string _data)
    {
        files->push_back(Files(_name, _data));
    }

    void addDirectory(string _name)
    {
        directories->push_back(Directory(_name, this));
    }

    Files *findFile(string _name)
    {
        if (_name.find("V:\\") != string::npos || _name.find("V:") != string::npos)
        {
            return findFile(_name.substr(_name.find("\\") + 1, _name.length() - 1));
        }
        else if (_name.find("\\") != string::npos)
        {
            string name = _name.substr(0, _name.find("\\"));

            for (auto it = directories->begin(); it != directories->end(); ++it)
            {
                if (it->name == name)
                {
                    return it->findFile(_name.substr(_name.find("\\") + 1, _name.length() - 1));
                }
            }
        }
        else
        {
            for (auto it = files->begin(); it != files->end(); ++it)
            {
                if (it->name == _name)
                {
                    return &(*it);
                }
            }
        }
        return nullptr;
    }

    void printPath()
    {
        if (path == "V")
        {
            cout << "V:\\" << endl;
        }
        else
        {
            cout << path << endl;
        }
    }

    string setPath()
    {
        if (parent == nullptr)
        {
            return "V:\\";
        }
        return parent->path + name + "\\";
    }

    // checking if directory already exists
    bool checkDirectory(string _name)
    {
        for (auto it = directories->begin(); it != directories->end(); it++)
        {
            if (it->name == _name)
            {
                return true;
            }
        }
        return false;
    }

    // checking if file already exists
    bool checkFile(string _name)
    {
        for (auto it = files->begin(); it != files->end(); it++)
        {
            if (it->name == _name)
            {
                return true;
            }
        }
        return false;
    }

    // get directory through path
    Directory *getDirectoryFromPath(string _path)
    {
        if (_path == "V:\\" || _path == "V:")
        {
            return this;
        }
        else if (_path.find("\\") != string::npos)
        {
            string name = _path.substr(0, _path.find("\\"));

            if (name == "V:\\" || name == "V:")
            {
                return this->getDirectoryFromPath(_path.substr(_path.find("\\") + 1, _path.length() - 1));
            }

            for (auto it = directories->begin(); it != directories->end(); ++it)
            {
                if (it->name == name)
                {
                    return it->getDirectoryFromPath(_path.substr(_path.find("\\") + 1, _path.length() - 1));
                }
            }
        }
        else
        {
            for (auto it = directories->begin(); it != directories->end(); ++it)
            {
                if (it->name == _path)
                {
                    return &(*it);
                }
            }
        }
        return nullptr;
    }
};

// class to store current condition of text
class Condition
{
public:
    list<list<char>> *lines;
    list<list<char>>::iterator rowItr;
    list<char>::iterator colItr;
    int cursorX, cursorY;

    Condition()
    {
        lines = new list<list<char>>();
        lines->push_back(list<char>());
        rowItr = lines->begin();
        colItr = rowItr->begin();
        cursorX = cursorY = 0;
    }

    void printData()
    {
        for (auto it = lines->begin(); it != lines->end(); ++it)
        {
            for (auto it2 = it->begin(); it2 != it->end(); ++it2)
            {
                cout << *it2;
            }
            cout << endl;
        }
    }
};

// text editor
class TextEditor
{
public:
    int cursorX;
    int cursorY;
    list<list<char>> *lines;
    list<list<char>>::iterator rowItr;
    list<char>::iterator colItr;
    stack<Condition *> *redo;
    deque<Condition *> *undo;

    TextEditor()
    {
        cursorX = cursorY = 0;
        lines = new list<list<char>>();
        lines->push_back(list<char>());
        redo = new stack<Condition *>();
        undo = new deque<Condition *>();
        rowItr = lines->begin();
        colItr = rowItr->begin();
    }

    void loadFile(Files *file){
        string data = file->data;
        string line = "";

        // copying the data into lines
        for (int i = 0; i < data.length(); i++){
            if (data[i] == '\n'){
                rowItr = lines->insert(rowItr, list<char>());
                for (int j = 0; j < line.length(); j++){
                    rowItr->push_back(line[j]);
                }
                line = "";
                rowItr++;
            }
            else{
                line += data[i];
            }
        }

        // if last line is not empty
        if (line.length() > 0){
            rowItr = lines->insert(rowItr, list<char>());
            for (int j = 0; j < line.length(); j++){
                rowItr->push_back(line[j]);
            }
        }
        rowItr = lines->begin();
        colItr = rowItr->begin();

        // removing last lines if it is empty
        auto temp = lines->begin();
        for (auto it = lines->begin(); it != lines->begin(); it++){
            temp++;
        }
        temp++;
        if (temp->empty()){
            lines->erase(temp);
        }
    }

    // save current condition of text
    void saveCondition()
    {
        Condition *condition = new Condition();

        auto r_itr = condition->lines->begin();
        for (auto it = lines->begin(); it != lines->end(); it++, r_itr++){

            condition->lines->push_back(list<char>());
            for (auto it2 = it->begin(); it2 != it->end(); it2++)
            {
                r_itr->push_back(*it2);
            }
        }

        condition->rowItr = condition->lines->begin();
        for (int i = 0; i < cursorY; i++){
            condition->rowItr++;
        }
        condition->colItr = condition->rowItr->begin();
        for (int i = 0; i < cursorX; i++){
            condition->colItr++;
        }
        condition->cursorX = cursorX;
        condition->cursorY = cursorY;

        undo->push_back(condition);

        if (undo->size() > 5){
            undo->pop_front();
        }
    }

    // load previous condition of text
    void loadCondition(Condition *condition){

        lines = condition->lines;
        rowItr = lines->begin();
        cursorX = condition->cursorX;
        cursorY = condition->cursorY;
        for (int i = 0; i < cursorY; i++){
            rowItr++;
        }
        colItr = rowItr->begin();
        for (int i = 0; i <= cursorX; i++){
            colItr++;
        }
    }

    void print(){
        for (auto it = lines->begin(); it != lines->end(); ++it){
            for (auto it2 = it->begin(); it2 != it->end(); ++it2){
                cout << *it2;
            }
            cout << endl;
        }
    }

    void save(Files *file)
    {
        string data = "";
        for (auto it = lines->begin(); it != lines->end(); ++it){
            for (auto it2 = it->begin(); it2 != it->end(); ++it2){
                data += *it2;
            }
            data += "\n";
        }
        data.pop_back();
        file->data = data;
    }

    void moveUp(){
        if (cursorY != 0){
            cursorY--;
            rowItr--;
            if (cursorX >= rowItr->size()){
                cursorX = rowItr->size();
            }
            colItr = rowItr->begin();
            for (int i = 0; i < cursorX; i++){
                colItr++;
            }
        }
    }

    void moveDown(){
        if (cursorY != lines->size() - 1){
            cursorY++;
            rowItr++;
            if (cursorX >= rowItr->size()){
                cursorX = rowItr->size();
            }
            colItr = rowItr->begin();
            for (int i = 0; i < cursorX; i++){
                colItr++;
            }
        }
    }

    void moveLeft(){
        if (cursorX == 0 && cursorY != 0){
            rowItr--;
            cursorY--;
            cursorX = rowItr->size();
            colItr = rowItr->begin();
            for (int i = 0; i < cursorX; i++){
                colItr++;
            }
        }
        else if (cursorX != 0){
            cursorX--;
            colItr--;
        }
    }

    void moveRight(){
        if (rowItr->size() == 0){
            return;
        }
        if (cursorX > rowItr->size() - 1 && cursorY != lines->size() - 1){
            rowItr++;
            cursorY++;
            cursorX = 0;
            colItr = rowItr->begin();
        }
        else if (cursorX <= rowItr->size() - 1){
            cursorX++;
            colItr++;
        }
    }

    void removeBack(){
        if (cursorX == 0 && cursorY != 0){

            // copy data of this line
            list<char> *temp = new list<char>();
            for (auto it = colItr; it != rowItr->end(); ++it){
                temp->push_back(*it);
            }

            // erase this line
            rowItr = lines->erase(rowItr);

            // move to previous line
            rowItr--;
            cursorY--;
            cursorX = rowItr->size();
            colItr = rowItr->begin();
            for (int i = 0; i < cursorX; i++){
                colItr++;
            }

            // paste data of next line to current line
            for (auto it = temp->begin(); it != temp->end(); ++it){
                rowItr->push_back(*it);
            }
        }
        else if (cursorX != 0){
            cursorX--;
            colItr--;
            colItr = rowItr->erase(colItr);
        }
    }

    void removeForward(){
        if (cursorX == rowItr->size() && cursorY != lines->size() - 1){

            // copy data of next line
            list<char> *temp = new list<char>();
            auto tempItr = rowItr;
            tempItr++;
            for (auto it = tempItr->begin(); it != tempItr->end(); ++it){
                temp->push_back(*it);
            }

            // erase next line
            // rowItr = lines->erase(tempItr);
            lines->erase(tempItr);

            // paste data of next line to current line
            for (auto it = temp->begin(); it != temp->end(); ++it){
                rowItr->push_back(*it);
            }

            // reset column iterator
            colItr = rowItr->begin();
            for (int i = 0; i < cursorX; i++){
                colItr++;
            }
        }
        else if (cursorX != rowItr->size()){
            colItr = rowItr->erase(colItr);
        }
    }

    void addNewLine(){

        // first copy the rest of the list
        list<char> *temp = new list<char>();
        for (auto it = colItr; it != rowItr->end(); ++it){
            temp->push_back(*it);
        }

        // erase it in the current line and paste in the next line
        rowItr->erase(colItr, rowItr->end());
        rowItr = lines->insert(++rowItr, *temp);
        colItr = rowItr->begin();

        cursorY++;
        cursorX = 0;
    }

    void addChar(char c){
        rowItr->insert(colItr, c);
        cursorX++;
    }

    void undoCommand(){
        if (!undo->empty()){
            Condition *currentCondition = undo->back();
            undo->pop_back();
            redo->push(currentCondition);
            loadCondition(currentCondition);
            system("cls");
            print();
        }
    }

    void redoCommand(bool doIt){
        if (!redo->empty() && doIt){
            Condition *currentCondition = redo->top();
            redo->pop();
            loadCondition(currentCondition);
            system("cls");
            print();
        }
    }

    // clears redo stack when some key is pressed interrupting undo queue
    bool clearRedoStack(bool doIt){
        if (doIt){
            while (!redo->empty()){
                redo->pop();
            }
        }
        return false;
    }

    void run(Files *file){
        loadFile(file);

        system("color F0");
        system("cls");

        print();

        bool modify = false;
        bool doIt = false;

        if (rowItr->empty()){
            rowItr->push_back(' ');
            colItr = rowItr->begin();
        }

        while (true){

            // cout << rowItr->size() << endl;
            // cout << cursorX << endl;
            // cout << *colItr << endl;

            gotoxy(cursorX, cursorY);
            char c = getch();

            bool capsLockState = GetKeyState(VK_CAPITAL);

            if (c == 0 || GetAsyncKeyState(VK_SHIFT) || c == 9 || capsLockState)
                continue;
            else if (c == 72)
            {
                moveUp();
            }
            else if (c == 80)
            {
                moveDown();
            }
            else if (c == 75)
            {
                moveLeft();
            }
            else if (c == 77)
            {
                moveRight();
            }
            else if (c == 13)
            { // enter key pressed
                saveCondition();
                addNewLine();
                modify = true;
            }
            else if (c == 27)
            { // escape key pressed
                break;
            }
            else if (c == 8)
            { // backspace pressed
                saveCondition();
                removeBack();
                modify = true;
            }
            else if (c == 83)
            { // delete pressed
                saveCondition();
                removeForward();
                modify = true;
            }
            else if (c == 26)
            { // CTRL + Z
                undoCommand();
                doIt = true;
            }
            else if (c == 25)
            { // CTRL + Y
                redoCommand(doIt);
            }
            else
            { // insert character
                saveCondition();
                addChar(c);
                modify = true;
            }

            if (modify)
            {
                system("cls");
                print();
                modify = false;
                doIt = clearRedoStack(doIt);
            }
        }

        save(file);
        system("color 07");
        system("cls");
    }
};

// tree class
class Tree
{
public:
    Directory *root;
    Directory *current;
    string prompt = "> ";
    queue<Files *> *printFiles;
    priority_queue<Files *> *printFilesPriority;
    int timer;

    Tree()
    {
        current = root = new Directory("V", nullptr);
        printFiles = new queue<Files *>();
        printFilesPriority = new priority_queue<Files *>();
        timer = 0;
    }

    void addFile(string _name, string _data)
    {
        root->files->push_back(Files(_name, _data));
    }

    void addDirectory(string _name)
    {
        root->directories->push_back(Directory(_name, root));
    }

    void printPath()
    {
        cout << root->path << endl;
    }

    // dir function
    void dir()
    {
        if (current->directories->size() == 0 && current->files->size() == 0)
        {
            cout << "no files" << endl;
        }
        else
        {
            for (auto it = current->directories->begin(); it != current->directories->end(); ++it)
            {
                cout << "           DIR         " << it->name << endl;
            }
            cout << endl
                 << endl;
            for (auto it = current->files->begin(); it != current->files->end(); ++it)
            {
                cout << "           File(s)         "
                     << "(" << it->size << ") bytes " << it->name << endl;
            }

            cout << endl
                 << "\t\t(" << current->directories->size() << ") Dir(s)" << endl;
            cout << "\t\t(" << current->files->size() << ") File(s)" << endl;
        }
    }

    // mkdir function
    void mkdir(string _name)
    {

        if (_name.empty())
        {
            cout << "Invalid syntax" << endl;
            return;
        }

        if (_name.find("\\") != string::npos || _name == "V:\\" || _name == "V:")
        {
            cout << "Invalid syntax" << endl;
            return;
        }

        // checking if directory already exists
        if (!current->checkDirectory(_name) && !current->checkFile(_name))
        {
            current->directories->push_back(Directory(_name, current));
        }
        else
        {
            cout << "Directory or name already exists" << endl;
        }
    }

    // rmdir function
    void rmdir(string _name)
    {
        if (!current->checkDirectory(_name))
        {
            cout << "Directory not found" << endl;
            return;
        }
        for (auto it = current->directories->begin(); it != current->directories->end(); ++it)
        {
            if (it->name == _name)
            {
                current->directories->erase(it);
                break;
            }
        }
    }

    // mkfile function
    void mkfile(string _name)
    {

        if (_name.empty())
        {
            cout << "Invalid syntax" << endl;
            return;
        }
        string text = _name.substr(_name.find(".") + 1, _name.length() - 1);
        if (text != "txt")
        {
            _name = _name.substr(0, _name.find(".")) + ".txt";
        }
        // checking for duplicate file names
        if (!current->checkFile(_name) && !current->checkDirectory(_name))
        {
            // add txt extension in the file name if it is not provieded

            cout << "Enter file data: ";
            string fileData = getInput();
            current->addFile(_name, fileData);
        }
        else{
            cout << "File or name already exists" << endl;
        }
    }

    // rmfile function
    void rmfile(string _name)
    {
        if (!current->checkFile(_name))
        {
            cout << "File not found" << endl;
            return;
        }
        for (auto it = current->files->begin(); it != current->files->end(); ++it)
        {
            if (it->name == _name)
            {
                current->files->erase(it);
                break;
            }
        }
    }

    string getNameWithExt(string name){
        if (name.find(".") != string::npos){
            if (name.substr(name.find("."), name.size() - 1) != "txt"){
                name = name.substr(0, name.find(".")) + ".txt";
            }
        }
        else{
            name += ".txt";
        }
        return name;
    }
    // rename function
    void rename(string oldname)
    {
        string currentnName = oldname.substr(0, oldname.find(" "));
        currentnName = getNameWithExt(currentnName);
        cout<<currentnName<<endl;
        string newName = oldname.substr(oldname.find(" ")+1, oldname.length() - 1);
        newName = getNameWithExt(newName);
        cout << newName << endl;
        if (current->checkFile(currentnName))
        {
            for (auto it = current->files->begin(); it != current->files->end(); ++it)
            {
                if (it->name == currentnName)
                {
                    it->name = newName;
                    return;
                }
            }
        }
        else if (current->checkDirectory(currentnName))
        {
            for (auto it = current->directories->begin(); it != current->directories->end(); ++it)
            {
                if (it->name == currentnName)
                {
                    // do update the path of the directory as well
                    it->path = it->parent->path + newName + "\\";
                    it->name = newName;
                    return;
                }
            }
        }
        else
        {
            cout << "file or directory not found" << endl;
        }
    }

    void attrib(string att)
    {
        Files *file = current->findFile(att);
        if (file == nullptr)
        {
            cout << "file not found" << endl;
            return;
        }
        cout << "file name is " << file->name << endl;
        cout << "file data is " << file->data << endl;
    }
    void find(string filename)
    {

        Files *file = current->findFile(filename);
        if (file == nullptr)
        {
            cout << "file not found" << endl;
            return;
        }
        cout << "file name is " << file->name << endl;
        cout << "file data is " << file->data << endl;
    }

    // findf function
    void findf(string input)
    {

        string name = input.substr(0, input.find(" "));
        string text = input.substr(input.find(" ") + 1, input.length() - 1);

        Files *file = current->findFile(name);
        if (file == nullptr)
        {
            cout << "file not found" << endl;
            return;
        }
        string fileData = file->data;
        if (fileData.find(text) != string::npos)
        {
            cout << "text found" << endl;
            cout << "file name is " << file->name << endl;
            cout << "file data is " << file->data << endl;
        }
        else
        {
            cout << "text not found" << endl;
        }
    }

    // findStr function
    void findStr(string text)
    {
        for (auto it = current->files->begin(); it != current->files->end(); ++it)
        {
            if (it->data.find(text) != string::npos)
