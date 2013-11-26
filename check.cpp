
#include "rtest.h"

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

int main()
{
    return rtest::run();
}
