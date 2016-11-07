#pragma once

#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <cassert>

using namespace std;

namespace Obj
{

// Any statement in a .obj file
class Statement
{
public:

    virtual ~Statement() {}

};

// Comment statement in a .obj file
class Comment: public Statement
{
public:

    Comment() {}
    Comment(const string &line): _line(line) {}

    friend istream & operator >> (istream  &input, Comment &comment)
    {
        getline(input, comment._line);
        return input;
    }

    string & line() { return _line; }

private:

    string _line;

};

// Vertex statement in a .obj file
class Vertex: public Statement
{
public:

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }

    friend istream & operator >> (istream  &input, Vertex &vertex)
    {
        string line;
        getline(input, line);

        istringstream iss { line };
        iss >> vertex._x >> vertex._y >> vertex._z;

        return input;
    }

private:

    double _x, _y, _z;

};

// .obj file
class File
{
public:

    // Comment found at line_no or nullptr if not a Comment.
    shared_ptr<Comment> comment_at(size_t line_no)
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Comment>(_statements[line_no-1]);
    }

    // Vertex found at line_no or nullptr if not a Vertex.
    shared_ptr<Vertex> vertex_at(size_t line_no)
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Vertex>(_statements[line_no-1]);
    }

    friend istream & operator >> (istream  &input, File &file)
    {
        while (input.good())
        {
            char type;
            input >> noskipws >> type;

            char subtype;
            input >> noskipws >> subtype;

            if (type == '#')
            {
                shared_ptr<Comment> comment { make_shared<Comment>() };
                input >> *comment;
                file._statements.push_back(comment);
            }
            else if (type == 'v' &&  subtype == ' ')
            {
                shared_ptr<Vertex> vertex { make_shared<Vertex>() };
                input >> *vertex;
                file._statements.push_back(vertex);
            }
        }

        return input;
    }

private:

    vector<shared_ptr<Statement>> _statements;

};

};
