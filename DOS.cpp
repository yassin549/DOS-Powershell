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
