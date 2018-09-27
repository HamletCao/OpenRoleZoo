//
// Created by kier on 2018/9/27.
//

#include <orz/tools/option.h>
#include <orz/tools/resources.h>
#include <orz/io/dir.h>
#include <orz/utils/format.h>

void print_help(const orz::arg::OptionSet &options) {
    std::cout << "Usage: command [option] input_path" << std::endl;
    std::cout << "Option:" << std::endl;
    for (auto &option : options) {
        std::cout << "    " << option << std::endl;
    }
}

int main(int argc, const char *argv[]) {
    orz::arg::OptionSet options;
    auto option_out_dir = options.add(orz::arg::STRING, {"o", "-out_dir"})->
            description("set generated files output dir")->
            value(".");
    auto option_filename = options.add(orz::arg::STRING, {"n", "-fn", "-filename"})->
            description("set output filename")->
            value("orz_resources");
    auto option_help = options.add(orz::arg::BOOLEAN, {"?", "h", "-help"})->
            description("print help documents");
    std::string input_path;

    std::vector<std::string> args(argv + 1, argv + argc);

    if (!options.parse(args)) {
        std::cerr << options.last_error_message() << std::endl;
        return 1;
    }

    if (option_help->found()) {
        print_help(options);
        return 0;
    }

    if (!options.check()) {
        std::cerr << options.last_error_message() << std::endl;
        return 2;
    }

    if (args.size() < 1) {
        print_help(options);
        return 3;
    }

    input_path = args[0];
    auto out_dir = option_out_dir->value().to_string();
    auto filename = option_filename->value().to_string();

    orz::mkdir(out_dir);

    std::string header_filename = orz::Join({out_dir, filename + ".h"}, orz::FileSeparator());
    std::string source_filename = orz::Join({out_dir, filename + ".c"}, orz::FileSeparator());

    orz::resources::compiler compiler;
    if (!compiler.compile(input_path, header_filename, source_filename)) {
        std::cerr << compiler.last_error_message() << std::endl;
        return 4;
    }

    return 0;
}