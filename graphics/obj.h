#pragma once

#include <string>
#include <vector>
#include <istream>
#include <cassert>

using namespace std;

namespace Obj
{

class Statement
{
public:

    virtual ~Statement() {}

};

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

class File
{
public:

    // Comment found at line_no or nullptr if not a Comment.
    shared_ptr<Comment> comment_at(size_t line_no)
    {
        assert(line_no > 0 && line_no <= _statements.size());

        return dynamic_pointer_cast<Comment>(_statements[line_no-1]);
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
        }

        return input;
    }

private:

    vector<shared_ptr<Statement>> _statements;

};

};
