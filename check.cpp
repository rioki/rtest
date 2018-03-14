
#include "rtest.h"
#include "rmock.h"

TEST(check_true)
{
    CHECK(true);
}

TEST(check_false)
{
    bool ok = false;
    try
    {
        CHECK(false);
    }
    catch (rtest::impl::Failure& f)
    {
        ok = true;
    }
    if (! ok)
    {
        throw rtest::impl::Failure(__FILE__, __LINE__, "Check no fail.");
    }
}

TEST(check_equal)
{
    CHECK_EQUAL(23, 23);
}

TEST(check_equal_fail)
{
    bool ok = false;
    try
    {
        CHECK_EQUAL(23, 24);
    }
    catch (rtest::impl::Failure& f)
    {
        ok = true;
    }
    if (! ok)
    {
        throw rtest::impl::Failure(__FILE__, __LINE__, "Check no fail.");
    }
}

TEST(check_equal_string1)
{
    CHECK_EQUAL("Hello", "Hello");
}

TEST(check_equal_string2)
{
    std::string str = "Hello";
    CHECK_EQUAL(str, "Hello");
}

TEST(check_equal_string3)
{
    std::string str = "Hello";
    CHECK_EQUAL("Hello", str);
}

TEST(check_equal_types)
{
    float f = 14;
    int i = 14;
    CHECK_EQUAL(f, i);
}

TEST(check_close)
{
    CHECK_CLOSE(17, 17.00001, 0.0001);
}

TEST(check_close_fail)
{    
    bool ok = false;
    try
    {
        CHECK_CLOSE(17, 17.00001, 0.000001);
    }
    catch (rtest::impl::Failure& f)
    {
        ok = true;
    }
    if (! ok)
    {
        throw rtest::impl::Failure(__FILE__, __LINE__, "Check no fail.");
    }
}

TEST(fail)
{
    bool ok = false;
    try
    {
        FAIL("A test.");
    }
    catch (rtest::impl::Failure& f)
    {
        ok = true;
    }
    if (! ok)
    {
        throw rtest::impl::Failure(__FILE__, __LINE__, "Fail fail.");
    }
}

class IntAdder
{
public:

    virtual ~IntAdder() {} // is this a requirement?

    virtual int add(int lhs, int rhs) = 0;

    virtual void dud(int a) = 0;
};

// Emulate the behavior of the mocking macros
class IntAdderNoMacroMock : public rmock::Mock, public IntAdder
{
public:
    IntAdderNoMacroMock() {}

    int add(int lhs, int rhs) override
    {
        return call<int>("add", lhs, rhs);
    }

    void dud(int a) override
    {
        call("dud", a);
    }
};

TEST(mock_no_macros_return)
{
    IntAdderNoMacroMock mock;
    mock.expect_return<int>("add", 3, 1, 2);

    IntAdder& adder = mock;
    int r = adder.add(1, 2);
    CHECK_EQUAL(3, r);
}

TEST(mock_no_macros_void)
{
    IntAdderNoMacroMock mock;
    mock.expect("dud", 13);

    IntAdder& adder = mock;
    adder.dud(13);    
}

MOCK(IntAdder) 
{
public:
    MOCK_FUNCTION2(int, add, int, int);
    MOCK_FUNCTION1_VOID(dud, int);
};

TEST(mock_return)
{
    IntAdderMock mock;

    mock.expect_return<int>("add", 3, 1, 2);

    int r = mock.add(1, 2);
    CHECK_EQUAL(3, r);
}

TEST(mock_void)
{
    IntAdderMock mock;
    mock.expect("dud", 13);

    IntAdder& adder = mock;
    adder.dud(13);    
}

TEST(mock_multiple_calls)
{
    IntAdderMock mock;
    mock.expect_return<int>("add", 3, 1, 2);
    mock.expect_return<int>("add", 4, 2, 2);

    IntAdder& adder = mock;
    int r = adder.add(1, 2);
    CHECK_EQUAL(3, r);
    r = adder.add(2, 2);
    CHECK_EQUAL(4, r);
}

TEST(mock_multiple_calls_wrong_function)
{
    IntAdderMock mock;
    mock.expect_return<int>("add", 4, 2, 2);
    mock.expect_return<int>("add", 3, 1, 2);    

    IntAdder& adder = mock;
    try 
    {
        adder.dud(2);
        FAIL("Expected exception.");
    }
    catch (std::runtime_error& ex) {}
}

TEST(mock_multiple_calls_wrong_arguments)
{
    IntAdderMock mock;    
    mock.expect_return<int>("add", 3, 1, 2);    
    mock.expect_return<int>("add", 4, 2, 2);

    IntAdder& adder = mock;
    try 
    {
        adder.add(1, 2);
        adder.add(2, 3);
        FAIL("Expected exception.");
    }
    catch (std::runtime_error& ex) {}
}

int main()
{
    return rtest::run();
}
