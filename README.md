
Rioki's Test Harness
====================

[![wercker status](https://app.wercker.com/status/a541f3ea45cf33f0604bf7c46945b745/m "wercker status")](https://app.wercker.com/project/bykey/a541f3ea45cf33f0604bf7c46945b745)

rtest is aimed to make it darn simple to write unit tests in C++.  

Usage
-----

You want to create a command line application for your unit tests. In the 
main.cpp function you simple write:

    #include "rtest.h"
    
    int main()
    {
        return rtest::run();
    }
    
Tests are define through a little macro magic:

    TEST(initial_string_is_empty)
    {
        std::string value;
        CHECK(value.empty());
    }
    
You should group tests into a test suite, to prevent name collisions:

    SUITE(string_test)
    {
        TEST(initial_is_empty)
        {
            std::string value;
            CHECK(value.empty());
        }
        
        TEST(size)
        {
            std::string value = "Hello";
            CHECK_EQUAL(5, value.size())
        }
    }
    
Since it is not obvious to the examples above, the tests can be distributed 
over any number of source files, as long as they are bound in the same 
executable.

To check conditions, three macros currently exists:

    CHECK(CONDITION)
    
This is the simplest of the check macros. It simply checks if a boolean condition
is true or not.
    
    CHECK_EQUAL(EXPECTED, ACTUAL)
    
To check the result of computations it may be useful to actually see the results.
To this end the CHECK_EQUAL takes an expected and an actual value, if they are
not equal the test fails and prints the values.
    
    CHECK_CLOSE(EXPECTED, ACTUAL, EPSILON)

The CHECK_CLOSE is basically like the CHECK_EQUAL, except it allows for a 
uncertainness of EPSILON. This especially useful, since floating point 
computations can be difficult. 
    
    CHECK_THROW(EXPRESSION, EXCEPTION_TYPE) 
    
The CHECK_THROW macro tests if a certain expression threw an exception.

In addition to the check macros any exception during the test will fail the test.

The check macros stop executing the test. So you can use a macro to test
a penitentially dangerous situation such as:

    std::vector<int> values = /* some computation */;
    CHECK(!values.empty());
    CHECK_EQUAL(23, values[0]);
    
The second check would segfault if `values` is empty, but since the first
check fails, the execution is not precoded beyond this point.

Finally you may need to setup and cleanup after tests. To do this you create 
a fixture like so:

    struct DatabaseFixture
    {
        Database db;
        
        DatabaseFixture()
        {
            db.open("localhost", 1234);
            db.create_test_tables();
        }
        
        ~DatabaseFixture()
        {
            db.drop_test_tables();
            db.close();
        }
    };

To use the fixture you defined your test like so:

    TEST_FIXTURE(DatabaseFixture, create_user)
    {
        db.create_user("jsmith", "John Smith", "password123!");
        User user = db.get_user("jsmith");
        CHECK_EQUAL("John Smith", user.dispname);
    }
    
As you can see all member and functions in the fixture are available to test.
    
Install
-------

Get the bleeding edge version: [rtest-latest.tar.gz](http://files.rioki.org/rtest/rtest-latest.tar.gz)

There are two options, you can either build a static library by simply 
calling `make` and install the library with `make install`. To influence 
where the library will be installed you can set the `prefix` variable like
so:

    prefix=$HOME/libs make install

Alternatively you can simple add the files `rtest.h` and `rtest.cpp` to your 
project and build them with it. This is the most portable version and 
recommended, since it reduces the hassle for your users. 

History
-------

If you know UnitTest++, you will notice that the interface resembles UnitTest++.
Actually the interface is a drop in replacement for UnitTest++ (for the features
implemented by rtest).

The reason was that sometime in 2012 UnitTest++ maintenance was changed and 
the library started to grow again. One of the key features of UnitTest++ 
was that is was small and simple.

At some point I realised that all I needed could be implemented in about 100 
lines of code. (Currently ~ 220 lines.) So I wrote for one of my libraries 
a test harness that matched the UnitTest++ interface but was significantly 
simpler. Since then I have not looked back and except for a few minor 
bug fixes have not touched the code.

Finally I decided to package the entire thing into a library for others
to use.

License
-------

Copyright (c) 2013 Sean Farrell <sean.farrell@rioki.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


