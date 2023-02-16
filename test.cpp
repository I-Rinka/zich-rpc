#include "mpack.h"
#include <iostream>
#include <map>
#include <memory>

using namespace std;

int main(int argc, char const *argv[])
{
    char *data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);

    // write the example on the msgpack homepage
    mpack_build_map(&writer);
    mpack_write_cstr(&writer, "compact");
    mpack_write_bool(&writer, false);
    mpack_write_cstr(&writer, "schema");
    mpack_write_uint(&writer, 0);
    mpack_complete_map(&writer);

    mpack_writer_t writer2;
    char *n_data;
    size_t n_size;
    mpack_writer_init_growable(&writer2, &n_data, &n_size);
    mpack_build_map(&writer2);
    mpack_write_cstr(&writer2, "bin");
    mpack_write_bin(&writer2, n_data, size);
    mpack_complete_map(&writer2);

    // finish writing
    if (mpack_writer_destroy(&writer) != mpack_ok)
    {
        fprintf(stderr, "An error occurred encoding the data!\n");
        return 0;
    }

    // use the data

    // parse a file into a node tree
    mpack_tree_t tree;
    mpack_tree_init_data(&tree, n_data, n_size);
    mpack_tree_parse(&tree);
    mpack_node_t root = mpack_tree_root(&tree);

    const char *nn_data = mpack_node_bin_data(mpack_node_map_cstr(root, "bin"));
    size = mpack_node_bin_size(mpack_node_map_cstr(root, "bin"));
    cout << size << *nn_data;


    mpack_tree_init_data(&tree, nn_data, size);
    mpack_tree_parse(&tree);
    root = mpack_tree_root(&tree);

    // extract the example data on the msgpack homepage
    bool compact = mpack_node_bool(mpack_node_map_cstr(root, "compact"));
    int schema = mpack_node_i32(mpack_node_map_cstr(root, "schema"));

    // clean up and check for errors
    if (mpack_tree_destroy(&tree) != mpack_ok)
    {
        fprintf(stderr, "An error occurred decoding the data!\n");
        return 0;
    }

    cout << compact << endl;
    // cout << "schema: " << schema << endl;

    // free(data);
    return 0;
}
