#include <RWRingBuffer.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(RWRingBufferTests)

BOOST_AUTO_TEST_CASE(init) {
    constexpr auto N = 16u;
    RWRingBuffer<int, N> ringbuffer;
    for (auto i = 0u; i < 15u; ++i) {
        BOOST_CHECK_EQUAL(ringbuffer.size(), i);
        ringbuffer.add(i);
        {
            int v = 0;
            for (auto i : ringbuffer) {
                BOOST_CHECK_EQUAL(i, v);
                ++v;
            }
            BOOST_CHECK_EQUAL(v, i+1);
        }
    }
    {
        int v = 2;
        for (auto i : ringbuffer.slice(2, 5)) {
            BOOST_CHECK_EQUAL(i, v);
            ++v;
        }
        BOOST_CHECK_EQUAL(v, 5);
    }
    BOOST_CHECK_EQUAL(ringbuffer.size(), 15);
    ringbuffer.add(15);
    {
        int v = 1;
        for (auto i : ringbuffer) {
            BOOST_CHECK_EQUAL(i, v);
            ++v;
        }
        BOOST_CHECK_EQUAL(v, 15+1);
    }
    {
        int v = 3;
        for (auto i : ringbuffer.slice(2, 5)) {
            BOOST_CHECK_EQUAL(i, v);
            ++v;
        }
        BOOST_CHECK_EQUAL(v, 6);
    }
    BOOST_CHECK_EQUAL(ringbuffer.size(), 15);
    ringbuffer.add(16);
    {
        int v = 2;
        for (auto i : ringbuffer) {
            BOOST_CHECK_EQUAL(i, v);
            ++v;
        }
        BOOST_CHECK_EQUAL(v, 16+1);
    }
    {
        int v = 4;
        for (auto i : ringbuffer.slice(2, 5)) {
            BOOST_CHECK_EQUAL(i, v);
            ++v;
        }
        BOOST_CHECK_EQUAL(v, 7);
    }
    BOOST_CHECK_EQUAL(ringbuffer.size(), 15);
}

BOOST_AUTO_TEST_SUITE_END()
