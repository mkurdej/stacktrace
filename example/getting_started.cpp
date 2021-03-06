// Copyright Antony Polukhin, 2016.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/array.hpp>
BOOST_NOINLINE void oops(std::size_t i);
BOOST_NOINLINE void foo(int i);
BOOST_NOINLINE void bar(int i);

BOOST_NOINLINE void oops(std::size_t i) {
    // std::terminate();
    // void (*p)() = 0; p();

    boost::array<int, 5> a = {{0, 1, 2, 3, 4}};
    foo(a[i]);
}

BOOST_NOINLINE void bar(int i) {
    boost::array<int, 5> a = {{0, 1, 2, 3, 4}};
    if (i < 5) {
        if (i >= 0) {
            foo(a[i]);
        } else {
            oops(i);
        }
    }
}

BOOST_NOINLINE void foo(int i) {
    bar(--i);
}

/*
../../../boost/array.hpp:123: T& boost::array<T, N>::operator[](boost::array<T, N>::size_type) [with T = int; long unsigned int N = 5ul; boost::array<T, N>::reference = int&; boost::array<T, N>::size_type = long unsigned int]: Assertion `(i < N)&&("out of range")' failed.
Aborted (core dumped)
*/

/*
Expression 'i < N' is false in function 'T& boost::array<T, N>::operator[](boost::array<T, N>::size_type) [with T = int; long unsigned int N = 5ul; boost::array<T, N>::reference = int&; boost::array<T, N>::size_type = long unsigned int]': out of range
Backtrace:
 0# boost::assertion_failed_msg(char const*, char const*, char const*, char const*, long)
 1# boost::array<int, 5ul>::operator[](unsigned long)
 2# oops(unsigned long)
 3# bar(int)
 4# foo(int)
 5# bar(int)
 6# foo(int)
 7# bar(int)
 8# foo(int)
 9# bar(int)
10# foo(int)
11# bar(int)
12# foo(int)
13# bar(int)
14# foo(int)
15# main
16# __libc_start_main
17# _start
18# ??
*/

//[getting_started_assert_handlers

// BOOST_ENABLE_ASSERT_DEBUG_HANDLER is defined for the whole project
#include <stdexcept>    // std::logic_error
#include <iostream>     // std::cerr
#include <boost/stacktrace.hpp>

namespace boost {
    void assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* file, long line) {
        std::cerr << "Expression '" << expr << "' is false in function '" << function << "': " << (msg ? msg : "<...>") << ".\n"
            << "Backtrace:\n" << boost::stacktrace::stacktrace() << '\n';
        throw std::logic_error("assertion");
    }

    void assertion_failed(char const* expr, char const* function, char const* file, long line) {
        ::boost::assertion_failed_msg(expr, 0 /*nullptr*/, function, file, line);
    }
} // namespace boost
//]


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//[getting_started_terminate_handlers

#include <exception>    // std::set_terminate, std::abort
#include <signal.h>     // ::signal
#include <boost/stacktrace.hpp>
#include <iostream>     // std::cerr

void my_terminate_handler() {
    std::cerr << "Terminate called:\n" << boost::stacktrace::stacktrace() << '\n';
    std::abort();
}

void my_signal_handler(int signum) {
    std::cerr << "Signal " << signum << ", backtrace:\n" << boost::stacktrace::stacktrace() << '\n';
    std::abort();
}
//]

void setup_handlers() {
//[getting_started_setup_handlers
    std::set_terminate(&my_terminate_handler);
    ::signal(SIGSEGV, &my_signal_handler);
//]
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/core/no_exceptions_support.hpp>
int main() {
    setup_handlers();

    BOOST_TRY {
        foo(5); // testing assert handler
    } BOOST_CATCH(...) {
    } BOOST_CATCH_END
}


