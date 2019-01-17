#ifndef RWGAME_RWRINGBUFFER_HPP
#define RWGAME_RWRINGBUFFER_HPP

#include <array>
#include <type_traits>

template <typename T, size_t N>
class RWRingBuffer {
std::array<T, N> _data{};
size_t _next = 0;
size_t _first = 0;
public:
template <typename R>
class RWRingBufferIterator {
    R& _ring;
    size_t _pos;
public:
    RWRingBufferIterator(R& ring, size_t pos)
        : _ring(ring)
        , _pos(pos) {
    }
    RWRingBufferIterator& operator++() {
        ++_pos;
        if (_pos == N) {
            _pos = 0;
        }
        return *this;
    }
    RWRingBufferIterator& operator+=(size_t offset) {
        _pos += offset;
        _pos %= N;
        return *this;
    }
    RWRingBufferIterator& operator--() {
        ++_pos;
        if (_pos == 0) {
            _pos = N-1;
        }
        return *this;
    }
    RWRingBufferIterator& operator-=(size_t offset) {
        _pos -= offset;
        _pos %= N;
        return *this;
    }
    template <typename = std::enable_if_t<!std::is_const<R>::value>>
    T& operator*() {
        return _ring._data[_pos];
    }
    const T& operator*() const {
        return _ring._data[_pos];
    }
    bool operator!=(const RWRingBufferIterator& other) const {
        return _pos != other._pos;
    }
};
RWRingBuffer() = default;
void clear() {
    _next = 0;
    _first = 0;
}
size_t size() const {
    return (_next - _first) % N;
}
void add(T&& d) {
    _data[_next] = std::forward<T>(d);
    ++_next;
    if (_next == N) {
        _next = 0;
    }
    if (_next == _first) {
        ++_first;
        if (_first == N) {
            _first = 0;
        }
    }
}
T &get(size_t idx) {
    size_t i = (_first + idx) % N;
    return _data[idx];
}
RWRingBufferIterator<RWRingBuffer> begin() {
    return RWRingBufferIterator(*this, _first);
}
RWRingBufferIterator<RWRingBuffer> end() {
    return RWRingBufferIterator(*this, _next);
}
RWRingBufferIterator<const RWRingBuffer> begin() const {
    return RWRingBufferIterator(*this, _first);
}
RWRingBufferIterator<const RWRingBuffer> end() const {
    return RWRingBufferIterator(*this, _next);
}
template <typename R>
class RWRingBufferSlice {
    R& _ring;
    size_t _start;
    size_t _stop;
public:
    RWRingBufferSlice(R& ring, size_t start, size_t stop)
        : _ring(ring)
        , _start(start)
        , _stop(stop) {
    }
    template <typename = std::enable_if_t<!std::is_const<R>::value>>
    RWRingBufferIterator<R> begin() {
        return RWRingBufferIterator(_ring, _start);
    }
    template <typename = std::enable_if_t<!std::is_const<R>::value>>
    RWRingBufferIterator<R> end() {
        return RWRingBufferIterator(_ring, _stop);
    }
    RWRingBufferIterator<R> begin() const {
        return RWRingBufferIterator(_ring, _start);
    }
    RWRingBufferIterator<R> end() const {
        return RWRingBufferIterator(_ring, _stop);
    }
};
RWRingBufferSlice<RWRingBuffer> slice(size_t start, size_t stop) {
    start = (_first + start) % N;
    stop = (_first + stop) % N;
    return RWRingBufferSlice<RWRingBuffer>(*this, start, stop);
}
RWRingBufferSlice<const RWRingBuffer> slice(size_t start, size_t stop) const {
    start = (_first + start) % N;
    stop = (_first + stop) % N;
    return RWRingBufferSlice(*this, start, stop);
}

};

#endif // RWGAME_RWRINGBUFFER_HPP
