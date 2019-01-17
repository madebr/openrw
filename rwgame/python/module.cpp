#include "pyopenrw_priv.hpp"

#include "pyopenrw.hpp"

#include <GitSHA1.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

//#include <iostream>

//class StreamOutWrapper {
//    std::ostream& _ostream;
//public:
//    StreamOutWrapper(std::ostream& ostream) : _ostream(ostream) {}
//    size_t write(const std::string& data) {
//        _ostream.write(data.data(), data.size());
//        return data.size();
//    }
//    void writelines(const std::vector<std::string>& lines) {
//        for (const auto& line : lines) {
//            _ostream.write(line.data(), line.size());
//        }
//    }
//    bool writable() {
//        return true;
//    }
//    void flush() {
//        _ostream.flush();
//    }
//    bool closed() {
//        return false;
//    }
//    void close() {
//        throw std::runtime_error("Operation not supported");
//    }
//    bool seekable() {
//        return false;
//    }
//    size_t seek(size_t, size_t) {
//        throw std::runtime_error("Operation not supported");
//    }
//};

//class StreamOutBufferWrapper {
//    size_t _pos{};
//    std::string _buffer{};
//public:
//    StreamOutBufferWrapper(size_t capacity=4096) {
//        _buffer.reserve(capacity);
//    }
//    size_t write(const std::string& data) {
//        _buffer.insert(_pos, data);
//        _pos += data.size();
//        return data.size();
//    }
//    void writelines(const std::vector<std::string>& lines) {
//        for (const auto& line : lines) {
//            _buffer.insert(_pos, line);
//            _pos += line.size();
//        }
//    }
//    bool writable() const {
//        return true;
//    }
//    void read(std::optional<size_t> size) {
//        size_t to_read = _buffer.size() - _pos;
//        if (size.has_value()) {
//            if (*size < to_read) {
//                to_read = *size;
//            }
//        }
//        std::string res = _buffer.substr(_pos, to_read);
//        _pos += to_read;
//    }
//    bool readable() const {
//        return true;
//    }
//    void flush() {
//    }
//    bool closed() const {
//        return false;
//    }
//    void close() {
//        throw std::runtime_error("Operation not supported");
//    }
//    void detach() {
//        throw std::runtime_error("Operation not supported");
//    }
//    size_t tell() const {
//        return _pos;
//    }
//    bool seekable() const {
//        return true;
//    }
//    size_t seek(ssize_t pos, size_t whence=0) {
//        switch (whence) {
//        case 0:
//            if ((pos < 0) || (static_cast<size_t>(pos) > _buffer.size())) {
//                throw std::invalid_argument("bad pos");
//            }
//            _pos = pos;
//            break;
//        case 1: {
//            ssize_t newpos = _pos + pos;
//            if ((newpos < 0) || (static_cast<size_t>(newpos) > _buffer.size())) {
//                throw std::invalid_argument("bad pos");
//            }
//            _pos = newpos;
//            break;
//        }
//        case 2: {
//            ssize_t newpos = _buffer.size() + pos;
//            if ((newpos < 0) || (static_cast<size_t>(newpos) > _buffer.size())) {
//                throw std::invalid_argument("bad pos");
//            }
//            _pos = newpos;
//            break;
//        }
//        default:
//            throw std::invalid_argument("bad whence");
//        }
//        return _pos;
//    }
//    const std::string &data() const {
//        return _buffer;
//    }
//};

void register_streamutils(py::module& m) {
    py::module mutil = m.def_submodule("streamutils", "Bind utils");
    py::add_ostream_redirect(mutil, "ostream_redirect");

//    py::class_<StreamOutWrapper> pyStreamOutWrapper(mutil, "StreamOutWrapper");
//    pyStreamOutWrapper
//            .def("write", &StreamOutWrapper::write, py::arg("data"))
//            .def("writelines", &StreamOutWrapper::writelines, py::arg("lines"))
//            .def("writable", &StreamOutWrapper::writable)
//            .def("flush", &StreamOutWrapper::flush)
//            .def("closed", &StreamOutWrapper::closed)
//            .def("close", &StreamOutWrapper::close)
//            .def("seekable", &StreamOutWrapper::seekable)
//            .def("seek", &StreamOutWrapper::seek, py::arg("offset"), py::arg("whence"))
//    ;

//    py::class_<StreamOutBufferWrapper> pyStreamOutBufferWrapper(mutil, "StreamOutBufferWrapper");
//    pyStreamOutBufferWrapper
//            .def(py::init<size_t>(), py::arg("capacity")=4096)
//            .def("write", &StreamOutBufferWrapper::write, py::arg("data"))
//            .def("writelines", &StreamOutBufferWrapper::writelines, py::arg("lines"))
//            .def("writable", &StreamOutBufferWrapper::writable)
//            .def("read", &StreamOutBufferWrapper::read)
//            .def("readable", &StreamOutBufferWrapper::readable)
//            .def("flush", &StreamOutBufferWrapper::flush)
//            .def("closed", &StreamOutBufferWrapper::closed)
//            .def("close", &StreamOutBufferWrapper::close)
//            .def("tell", &StreamOutBufferWrapper::tell)
//            .def("seekable", &StreamOutBufferWrapper::seekable)
//            .def("seek", &StreamOutBufferWrapper::seek, py::arg("position"), py::arg("whence")=0)
//            .def("__repr__", [](const StreamOutBufferWrapper& buffer) {return "<StreamOutBufferWrapper:data=\"" + buffer.data() + "\",size=" + std::to_string(buffer.data().size()) + ",pos=" + std::to_string(buffer.tell()) + ">";})
//    ;
}

void pyopenrw_register_module(py::module& m) {
    m.attr("__version__")= kGitSHA1Hash;
    m.attr("EMBEDDED") = PYOPENRW_EMBEDDED;
    register_streamutils(m);

    register_logging(m);
    register_config(m);
    register_game(m);
}
