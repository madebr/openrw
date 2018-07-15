#include "names.hpp"
\
#include "librw/streams.hpp"

#include <rw.h>
#include <rwgta.h>

//#include "librw/streams.hpp"

#include <rw/filesystem.hpp>

#include <yaml-cpp/yaml.h>
#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

int export_yaml(YAML::Emitter &emitter, const rw::Texture *texture, rw::uint32 version, const rwfs::path &yamlPath) {
    auto raster = texture->raster;
    auto image = LibRWSharedPtr(raster->toImage());

    rwfs::path filePath = yamlPath / texture->name;
    filePath.replace_extension(".tga");

    rw::writeTGA(image.get(), filePath.c_str());

    emitter << YAML::BeginMap;
    emitter << YAML::Key << "texture" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "version" << YAML::Value << YAML::Hex << version;
    emitter << YAML::Key << "contents" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "filename" << YAML::Value << filePath.filename().string();
    emitter << YAML::Key << "width" << YAML::Value << raster->width;
    emitter << YAML::Key << "height" << YAML::Value << raster->height;
    emitter << YAML::Key << "platform" << YAML::Value << BIMAP_PLATFORM_STR.toString(raster->platform);
    emitter << YAML::Key << "name" << YAML::Value << texture->name;
    emitter << YAML::Key << "mask" << YAML::Value << texture->mask;
    emitter << YAML::Key << "filter" << YAML::Value << BIMAP_TEXTURE_FILTERMODE_STR.toString(texture->getFilter());
    emitter << YAML::Key << "addressU" << YAML::Value << texture->getAddressU();
    emitter << YAML::Key << "addressV" << YAML::Value << texture->getAddressV();
    emitter << YAML::Key << "format" << YAML::Value << BIMAP_RASTER_FORMAT_STR.toString(raster->format);
    emitter << YAML::Key << "mipmap" << YAML::Value << BIMAP_RASTER_MIPMAP_STR.toString(raster->format);
    emitter << YAML::Key << "palette" << YAML::Value << BIMAP_RASTER_PALETTE_STR.toString(raster->format);
    emitter << YAML::Key << "raster type" << YAML::Value << BIMAP_RASTER_TYPE_STR.toString(raster->type);
    emitter << YAML::Key << "depth" << YAML::Value << raster->depth;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;

    return 0;
}

int export_yaml(YAML::Emitter& emitter, rw::TexDictionary* dictionary, rw::uint32 version, const rwfs::path &yamlPath) {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "texture dictionary" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "version" << YAML::Value << YAML::Hex << version;
    emitter << YAML::Key << "contents" << YAML::Value;
    emitter << YAML::BeginSeq;

    FORLIST(dictList, dictionary->textures) {
        auto texture = LLLinkGetData(dictList, rw::Texture, inDict);
        int exportRes = export_yaml(emitter, texture, version, yamlPath);
        if (exportRes) {
            return exportRes;
        }
    }

    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
    return 0;
}

int export_yaml(YAML::Emitter& emitter, const rwfs::path& path, const rwfs::path& yamlPath) {
    LibRWStreamFile stream;
    auto res = stream.open(path, LibRWOpenMode::READ);
    if (!res) {
        std::cerr << "Opening stream failed!\n";
        return 1;
    }

    rw::uint32 version;
    if (!rw::findChunk(&stream, rw::ID_TEXDICTIONARY, nullptr, &version)) {
        std::cerr << "No Texture dictionary found\n";
        return 1;
    }
    auto dictionary = LibRWSharedPtr(rw::TexDictionary::streamRead(&stream));
    if (!dictionary) {
        std::cerr << "Failed to read texture dictionary\n";
        return 1;
    }
    auto exportRes = export_yaml(emitter, dictionary.get(), version, yamlPath);
    if (exportRes) {
        return exportRes;
    }

    LibRWStreamFile outStream;
    outStream.open("out.txd", "wb");
    dictionary->streamWrite(&outStream);

    return 0;
}

int export_file(const rwfs::path& txdPath, const rwfs::path& yamlPath) {
    rw::Engine::init();
    gta::attachPlugins();
    rw::Engine::open();
    rw::Engine::start(nullptr);

    YAML::Emitter emitter;
//    emitter << YAML::BeginDoc;

    int res = export_yaml(emitter, txdPath, yamlPath);

//    emitter << YAML::EndDoc;
    std::ofstream ofs((yamlPath / "contents.yml").string());
    ofs << emitter.c_str() << '\n';
    if (!ofs) {
        return 1;
    }
    ofs.close();
//    std::cout << emitter.c_str();

    rw::Engine::stop();
    rw::Engine::close();

    return res;
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Show this help message")
        ("input,i", po::value<rwfs::path>()->value_name("PATH")->required(), "Input texture")
        ("output,o", po::value<rwfs::path>()->value_name("PATH")->required(), "Output path")
    ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")) {
            std::cout << desc;
            return EXIT_SUCCESS;
        }
        po::notify(vm);
    } catch (po::error& ex) {
        std::cerr << "Error parsing arguments: " << ex.what() << std::endl;
        std::cerr << desc;
        return EXIT_FAILURE;
    }

    auto input = vm["input"].as<rwfs::path>();
    auto output = vm["output"].as<rwfs::path>();
    if (!boost::filesystem::is_directory(output)) {
        try {
            boost::filesystem::create_directories(output);
        } catch (boost::system::system_error& e) {
            std::cerr << "Failed to create output directory\n";
            return EXIT_FAILURE;
        }
    }

    auto exportRes = export_file(input, output);
    if (exportRes) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
