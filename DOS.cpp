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
