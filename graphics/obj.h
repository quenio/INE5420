#pragma once

#include <memory>
#include <string>
#include <vector>
#include <list>
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

// Empty line in a .obj file
class EmptyLine: public Statement
{
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

class Face: public Statement
{
public:

    Face() {}
    Face(initializer_list<size_t> references): _references(references) {}

    list<size_t> & references() { return _references; }

    // True if a and b match.
    friend bool operator == (const Face &a, const Face &b)
    {
        return a._references == b._references;
    }

    friend istream & operator >> (istream  &input, Face &face)
    {
        string line;
        getline(input, line);

        istringstream iss { line };

        while (iss.good())
        {
            size_t reference;
            iss >> reference;
            face._references.push_back(reference);
        }

        return input;
    }

private:

    list<size_t> _references;

};

// .obj file
class File
{
public:

    // True if line is empty
    bool is_line_empty(size_t line_no) const
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<EmptyLine>(_statements[line_no-1]) != nullptr;
    }

    // Comment found at line_no or nullptr if not a Comment.
    shared_ptr<Comment> comment_at(size_t line_no) const
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Comment>(_statements[line_no-1]);
    }

    // Vertex found at line_no or nullptr if not a Vertex.
    shared_ptr<Vertex> vertex_at(size_t line_no) const
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Vertex>(_statements[line_no-1]);
    }

    // Face found at line_no or nullptr if not a Face.
    shared_ptr<Face> face_at(size_t line_no) const
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Face>(_statements[line_no-1]);
    }

    vector<shared_ptr<Vertex>> vertices() const
    {
        vector<shared_ptr<Vertex>> vertices;

        for (size_t line_no = 1; line_no <= _statements.size(); line_no++)
        {
            const shared_ptr<Vertex> vertex = vertex_at(line_no);
            if (vertex != nullptr) vertices.push_back(vertex);
        }

        return vertices;
    }

    list<shared_ptr<Face>> faces() const
    {
        list<shared_ptr<Face>> faces;

        for (size_t line_no = 1; line_no <= _statements.size(); line_no++)
        {
            const shared_ptr<Face> face = face_at(line_no);
            if (face != nullptr) faces.push_back(face);
        }

        return faces;
    }

    friend istream & operator >> (istream  &input, File &file)
    {
        while (input.good())
        {
            char type;
            input >> noskipws >> type;

            char subtype = ' ';
            if (type != '\n')
            {
                input >> noskipws >> subtype;
            }

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
            else if (type == 'f' && subtype == ' ')
            {
                shared_ptr<Face> face { make_shared<Face>() };
                input >> *face;
                file._statements.push_back(face);
            }
            else
            {
                // If statement is not recognized, then ignore the rest of the line and interpret as an empty line.
                if (type != '\n' && subtype != '\n')
                {
                    string line;
                    getline(input, line);
                }

                file._statements.push_back(make_shared<EmptyLine>());
            }
        }

        return input;
    }

private:

    vector<shared_ptr<Statement>> _statements;

};

};

inline Obj::File obj_file(istream &input)
{
    Obj::File file;
    input >> file;
    return file;
}

inline Obj::File obj_file(const string &str)
{
    istringstream input { str };
    return obj_file(input);
}




