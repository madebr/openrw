/**
  * Simple implementation of xxd -i INPUTFILE > OUTPUTFILE
  * Windows does not have xxd, so provide simple implementation.
  *
  * USAGE:
  * embedder [--help] -i SOURCE_FILE [-h HEADER_FILE] -s SOURCE_FILE [-n VARIABLE_NAME]
  */

#include <boost/program_options.hpp>

#include <rw/filesystem.hpp>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

/**
 * @brief uppercase_remove_unknown Uppercase all letters and change all non-alnums with underscores
 * @param s String to modify. Will be modified in-place.
 */
void uppercase_remove_unknown(std::string &s) {
    std::transform(s.begin(), s.end(), s.begin(), [](char c) {
        if (!::isalnum(c)) {
            return '_';
        }
        return static_cast<char>(::toupper(c));
    });
}

int main(int argc, const char *argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Show this help message")
        ("input,i", po::value<rwfs::path>()->value_name("PATH")->required(), "Input file")
        ("header,h", po::value<rwfs::path>()->value_name("PATH"), "Output header file")
        ("source,s", po::value<rwfs::path>()->value_name("PATH")->required(), "Output source file")
        ("name,n", po::value<std::string>()->value_name("NAME"), "Variable name")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc;
        return 0;
    }

    try {
        po::notify(vm);
    } catch (po::error &ex) {
        std::cout << "Error parsing arguments: " << ex.what() << std::endl;
        std::cout << desc;
        return 1;
    }

    auto input = vm["input"].as<rwfs::path>();
    auto source = vm["source"].as<rwfs::path>();
    std::string variableName;
    if (vm.count("name")) {
        variableName = vm["name"].as<std::string>();
    } else {
        variableName = input.string();
        uppercase_remove_unknown(variableName);
    }

    std::ifstream ifs(input.string(), std::ios_base::binary);
    ifs.seekg(0, std::ios_base::end);
    auto fileSize = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    if (vm.count("header")) {
        auto header = vm["header"].as<rwfs::path>();
        std::string protection = header.string();
        uppercase_remove_unknown(protection);

        std::ofstream hfs(header.string());
        hfs << "#ifndef " << protection << "\n"
            << "#define " << protection << "\n"
            << "\n"
            << "extern const unsigned char " << variableName << "[" << fileSize << "];\n"
            << "\n"
            << "#endif\n";
        if (!hfs) {
            std::cerr << "Failed to write header file\n";
            return 2;
        }
        hfs.close();
    }

    std::ofstream sfs(source.string());
    sfs << "const unsigned char " << variableName << "[] = {";
    sfs << std::hex;
    for (auto p = 0; p < fileSize; ++p) {
        if (!(p % 8)) {
            sfs << "\n   ";
        }
        sfs << " 0x" << std::setw(2) << std::setfill('0') << ifs.get() << ",";
    }
    sfs << "\n};\n";
    if (!sfs) {
        std::cerr << "Failed to write source file\n";
        return 3;
    }
    if (ifs.tellg() != fileSize) {
        std::cerr << "Input file corrupt!\n";
        return 4;
    }
    return 0;
}
