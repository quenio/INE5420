#include "min_unit.h"
#include "../obj.h"
#include <sstream>

static const string obj_file {
    "# This is a comment.\n"
    "# Another comment."
};

static const char * test_obj_file()
{
    istringstream input { obj_file };

    Obj::File file;
    input >> file;

    shared_ptr<Obj::Comment> first_comment = file.comment_at(1);
    mu_assert(first_comment != nullptr);
    mu_assert(first_comment->line() == "This is a comment.");

    shared_ptr<Obj::Comment> second_comment = file.comment_at(2);
    mu_assert(second_comment != nullptr);
    mu_assert(second_comment->line() == "Another comment.");

    return nullptr;
}

void all_tests()
{
    mu_test(test_obj_file);
}
