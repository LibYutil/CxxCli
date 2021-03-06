
#include <cassert>

#include <vector>
#include <string>
#include <iostream>

#include <CxxCli/CxxCli.hpp>

#include "test_header.hxx"

static int test() {
    DECLARE_ARGS("dataset-manager", "dataset", "create", "-l", "EN", "-i", "i0", "-i", "i1", "1")

    using namespace CxxCli;

    int cmd_ds_list = 0;
    std::vector<const char *> cmd_ds_delete;
    int cmd_ds_create = 0;
    std::vector<const char *> cmd_ds_create_l;
    std::vector<std::string> cmd_ds_create_i;
    std::vector<const char *> cmd_ds_create_X;
    int cmd_use_lang = 0;

    int cmd_ann_list = 0;

    auto cmd = Command(
        Sequence(
            Const("dataset-manager"),
            Sequence(
                Branch(
                    Sequence(
                        Const("dataset"),
                        Branch(
                            Const("list") >> [&] { ++cmd_ds_list; },
                            Sequence(Const("delete"), Var() >> [&] (const char * value) { cmd_ds_delete.emplace_back(value); }),
                            Sequence(
                                Const("create") >> [&] { ++cmd_ds_create; },
                                Optional(Sequence(Const("-l"), Var() >> &cmd_ds_create_l)) >> [&] { ++cmd_use_lang; },
                                Loop(Sequence(Const("-i"), Var() >> &cmd_ds_create_i)),
                                Var() >> &cmd_ds_create_X
                            )
                        )
                    ) & UsageAsList,
                    Sequence(
                        Const("annotator"),
                        Branch(
                            Const("list") >> [&] { ++cmd_ann_list; },
                            Sequence(Const("register"), Var("fname"), Var("lname")),
                            Sequence(Const("activate"), Var("uuid") >> [&] (const char * value) { std::cout << "annotator activate " << value << std::endl; }),
                            Sequence(Const("deactivate"), Var("uuid"))
                        )
                    ) & UsageAsList
                )
            ) & UsageAsList
        )
    );

    auto result = cmd.parse(argc, argv);

    static_assert(!details::has_container_methods<int, int>::has_emplace_back, "failing template");
    static_assert(details::has_container_methods<std::vector<int>, int>::has_emplace_back, "failing template");
    static_assert(details::has_container_methods<std::vector<std::string>, const char *>::has_emplace_back, "failing template");

    static_assert(!details::has_container_methods<int, int>::has_push_back, "failing template");
    static_assert(details::has_container_methods<std::vector<int>, int>::has_push_back, "failing template");
    static_assert(details::has_container_methods<std::vector<std::string>, const char *>::has_push_back, "failing template");

    assert(result);

    assert(cmd_ds_list == 0);
    assert(cmd_ds_delete.size() == 0);
    assert(cmd_ds_create == 1);

    assert(cmd_ds_create_l.size() == 1);
    assert(cmd_ds_create_l[0] == argv[4]);

    assert(cmd_ds_create_i.size() == 2);
    assert(cmd_ds_create_i[0] == argv[6]);
    assert(cmd_ds_create_i[1] == argv[8]);

    assert(cmd_ds_create_X.size() == 1);
    assert(cmd_ds_create_X[0] == argv[9]);

    return 0;
}

int main(int argc, char ** argv) { return test(); }
