#include <RWConfig.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <vector>

namespace py = pybind11;

namespace {
RWArgConfigLayer pyParseArguments(const RWArgumentParser& parser, const std::optional<std::vector<std::string>> argsOpt) {
    std::vector<std::string> args;
    if (!argsOpt) {
        auto sysm = py::module::import("sys");
        sysm.reload();
        args = py::cast<std::vector<std::string>>(sysm.attr("argv"));
    } else {
        args = *argsOpt;
    }
    std::vector<const char*> cargs;
    cargs.reserve(args.size());
    std::transform(args.begin(), args.end(), std::back_inserter(cargs), [](const auto& s) {return s.c_str();});
    auto res = parser.parseArguments(cargs.size(), cargs.data());
    if (!res) {
        throw std::invalid_argument("Bad arguments");
    }
    return *res;
}
py::tuple pyLoadFile(const RWConfigParser& parser, const std::string& path) {
    auto [layer, parseResult] = parser.loadFile(path);
    if (parseResult.type() != RWConfigParser::ParseResult::GOOD) {
        throw std::invalid_argument(parseResult.what());
    }
    py::tuple layer_unknown{2};
    layer_unknown[0] = layer;
    layer_unknown[1] = parseResult.getUnknownData();
    return layer_unknown;
}
void pySaveFile(const RWConfigParser& parser, const std::string& path, const RWConfigLayer& layer, const std::map<std::string, std::string>& extra) {
    auto parseResult = parser.saveFile(path, layer, extra);
    if (parseResult.type() != RWConfigParser::ParseResult::GOOD) {
        throw std::invalid_argument(parseResult.what());
    }
}
}

void register_config(py::module& m) {
    py::module configm = m.def_submodule("config", "Config submodule");

    py::class_<RWConfigLayer> configLayer(configm, "RWConfigLayer");
    configLayer
            .def(py::init<>())
            .def_static("default", &RWConfigLayer::buildDefault);
#define RWCONFIGARG(_RW_TYPE, _RW_NAME, _RW_DEFAULT, _RW_CONFPATH, _RW_CATEGORY, _RW_ARGMASK, _RW_ARGMETA, _RW_HELP)    \
    configLayer.def_readwrite(#_RW_NAME, &RWConfigLayer::_RW_NAME, _RW_HELP);
#define RWARG(...)
#define RWARG_OPT(...)
#include <RWConfig.inc>
#undef RWARG_OPT
#undef RWARG
#undef RWCONFIGARG

    py::class_<RWArgConfigLayer, RWConfigLayer> argConfigLayer(configm, "RWArgConfigLayer");
    argConfigLayer
            .def(py::init<>());
#define RWCONFIGARG(...)
#define RWARG(_RW_TYPE, _RW_NAME, _RW_CATEGORY, _RW_ARGMASK, _RW_ARGMETA, _RW_HELP) \
    argConfigLayer.def_readwrite(#_RW_NAME, &RWArgConfigLayer::_RW_NAME, _RW_HELP);
#define RWARG_OPT(_RW_TYPE, _RW_NAME, _RW_CATEGORY, _RW_ARGMASK, _RW_ARGMETA, _RW_HELP) \
    argConfigLayer.def_readwrite(#_RW_NAME, &RWArgConfigLayer::_RW_NAME, _RW_HELP);
#include <RWConfig.inc>
#undef RWARG_OPT
#undef RWARG
#undef RWCONFIGARG

    py::class_<RWConfig> config(configm, "RWConfig");
    config
            .def(py::init<>())
            .def_property_readonly_static("NLayers", [](py::object) {return RWConfig::NLayers;})
            .def_property_readonly("missing_keys", &RWConfig::missingKeys)
            .def_readwrite("layers", &RWConfig::layers)
    ;

    py::enum_<RWConfigLayerDefinition>(config, "RWConfigLayerDefinition")
            .value("USER", RWConfigLayerDefinition::LAYER_USER)
            .value("ARGUMENT", RWConfigLayerDefinition::LAYER_ARGUMENT)
            .value("CONFIGFILE", RWConfigLayerDefinition::LAYER_CONFIGFILE)
            .value("DEFAULT", RWConfigLayerDefinition::LAYER_DEFAULT)
            .export_values();

    py::class_<RWArgumentParser> argparser(configm, "RWArgumentParser");
    argparser
            .def(py::init<>())
            .def("print_help", [](const RWArgumentParser& parser) { parser.printHelp(std::cout);})
            .def("parse_arguments", pyParseArguments, py::arg("args")=py::none{})
    ;

    py::class_<RWConfigParser> confparser(configm, "RWConfigParser");
    confparser
            .def(py::init<>())
            .def_property_readonly_static("default_config_path", [](py::object) { return RWConfigParser::getDefaultConfigPath().string();}) // FIXME: return pathlib.Path
            .def("load", pyLoadFile, py::arg("path")=std::string{RWConfigParser::getDefaultConfigPath().string()})
            .def("save", pySaveFile, py::arg("path"), py::arg("layer"), py::arg("extra")=std::map<std::string, std::string>{})
    ;
}
