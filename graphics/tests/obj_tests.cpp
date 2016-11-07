#include "min_unit.h"
#include "../obj.h"
#include "../doubles.h"

static const string obj_file {
    "# Vertex list:\n"
    "v -0.5 0.6 -0.7\n"
    "\n"
    " \n"
    "  \n"
    "un \n" // unknown statement
    "f 4 3 2 1\n"
    "# End of file"
};

static const char * test_obj_file()
{
    istringstream input { obj_file };

    shared_ptr<Obj::Comment> comment;
    shared_ptr<Obj::Vertex> vertex;
    shared_ptr<Obj::Face> face;

    Obj::File file;
    input >> file;

    comment = file.comment_at(1);
    mu_assert(comment != nullptr);
    mu_assert(comment->line() == "Vertex list:");

    vertex = file.vertex_at(2);
    mu_assert(vertex != nullptr);
    mu_assert(equals(vertex->x(), -0.5));
    mu_assert(equals(vertex->y(), 0.6));
    mu_assert(equals(vertex->z(), -0.7));

    mu_assert(file.is_line_empty(3));
    mu_assert(file.is_line_empty(4));
    mu_assert(file.is_line_empty(5));
    mu_assert(file.is_line_empty(6)); // unknown statement

    face = file.face_at(7);
    mu_assert(face != nullptr);
    mu_assert(*face == Obj::Face({ 4, 3, 2, 1 }));

    comment = file.comment_at(8);
    mu_assert(comment != nullptr);
    mu_assert(comment->line() == "End of file");

    return nullptr;
}

void all_tests()
{
    mu_test(test_obj_file);
}
