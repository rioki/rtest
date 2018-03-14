// 
// Copyright (c) 2018 Sean Farrell <sean.farrell@rioki.org>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#ifndef _RMOCK_H_
#define _RMOCK_H_

#include <stdexcept>
#include <functional>
#include <string>
#include <list>
#include <queue>
#include <sstream>
#ifdef _STD_ANY_
#include <any>
#else
#include <experimental/any>
namespace std
{
    using std::experimental::any;
    using std::experimental::any_cast;
}
#endif

namespace rmock
{
    class Mock
    {
    public:
        virtual ~Mock();

        template <typename ... Args>
        void call(const std::string& id, Args ... args);

        template <typename Return, typename ... Args>
        Return call(const std::string& id, Args ... args);

        void expect(const std::string& id);

        template <typename ... Args>
        void expect(const std::string& id, Args ... args);

        template <typename Return, typename ... Args>
        void expect_return(const std::string& id, Return ret, Args ... args);

    private:
        struct CallInfo
        {
            std::string            id;
            std::any               return_value;
            std::list<std::any>    arguments;
            std::list<std::string> sarguments;
        };
        std::queue<CallInfo> calls;

        template <typename ... Args>
        void check_call(const std::string& id, Args ... args);
    };

    inline
    std::list<std::any> arg_pack() {
        std::list<std::any> list;
        return list;
    }

    template<typename T>
    std::list<std::any> arg_pack(T first) {
        std::list<std::any> list;
        list.push_front(first);
        return list;
    }

    template<typename T, typename... Args>
    std::list<std::any> arg_pack(T first, Args... remainder) {
        std::list<std::any> list = arg_pack(remainder...);
        list.push_front(first);
        return list;
    }

    template <typename T>
    std::string to_string(T v)
    {
        std::stringstream buff;
        buff << v;
        return buff.str();
    }

    inline
    std::list<std::string> string_pack() {
        std::list<std::string> list;
        return list;
    }

    template<typename T>
    std::list<std::string> string_pack(T first) {
        std::list<std::string> list;
        list.push_front(to_string(first));
        return list;
    }

    template<typename T, typename... Args>
    std::list<std::string> string_pack(T first, Args... remainder) {
        std::list<std::string> list = string_pack(remainder...);
        list.push_front(to_string(first));
        return list;
    }

    template<typename Iter>
    bool arg_check(Iter begin, Iter end) {        
        return begin == end;
    }

    template<typename Iter, typename T>
    bool arg_check(Iter begin, Iter end, T first) {
        if (begin == end)
        {
            return false;
        }

        T expected = std::any_cast<T>(*begin);
        if (expected != first)
        {
            return false;
        }

        begin++;

        return begin == end;
    }

    template<typename Iter, typename T, typename... Args>
    bool arg_check(Iter begin, Iter end, T first, Args... remainder) {
        if (begin == end)
        {
            return false;
        }

        T expected = std::any_cast<T>(*begin);
        if (expected != first)
        {
            return false;
        }

        begin++;

        return arg_check(begin, end, remainder...);
    }

    inline
    std::ostream& operator << (std::ostream& os, const std::list<std::string>& anys)
    {
        auto i = anys.begin();
        while (i != anys.end())
        {
            os << "'" << *i << "'";
            i++;
            if (i != anys.end())
            {
                os << ", ";
            }
        }
        return os;
    }

    template <typename ... Args>
    void Mock::call(const std::string& id, Args ... args)
    {
        check_call(id, args...);

        calls.pop();        
    }

    template <typename Return, typename ... Args>
    Return Mock::call(const std::string& id, Args ... args)
    {
        check_call(id, args...);

        Return ret = std::any_cast<Return>(calls.front().return_value);

        calls.pop();

        return ret;
    }

    template <typename ... Args>
    void Mock::expect(const std::string& id, Args ... args)
    {
        CallInfo info;
        info.id         = id;
        info.arguments  = arg_pack(args...);
        info.sarguments = string_pack(args...);
        calls.push(info);
    }

    template <typename Return, typename ... Args>
    void Mock::expect_return(const std::string& id, Return ret, Args ... args)
    {
        CallInfo info;
        info.id           = id;
        info.return_value = ret;
        info.arguments    = arg_pack(args...);
        info.sarguments   = string_pack(args...);
        calls.push(info);
    }

    template <typename ... Args>
    void Mock::check_call(const std::string& id, Args ... args)
    {
        if (calls.empty())        
        {
            throw std::runtime_error("No further calls registered.");
        }
        
        if (id != calls.front().id)
        {
            std::stringstream buff;
            buff << "Expected a call to '" << calls.front().id << "' but got a call to '" << id << "'." << std::endl;
            throw std::runtime_error(buff.str());
        }

        bool bargs = arg_check(calls.front().arguments.begin(), calls.front().arguments.end(), args...);
        if (!bargs)
        {
            auto sargs = string_pack(args...);

            std::stringstream buff;
            buff << "Expected arguments " << calls.front().sarguments << " but got " << sargs << " on call to '" << calls.front().id << "'." << std::endl;
            throw std::runtime_error(buff.str());
        }
    }
}

#define MOCK(TYPE) \
    class TYPE##Mock : public ::rmock::Mock, public TYPE

#define MOCK_FUNCTION(RET, NAME) RET NAME () override { return call<RET>(#NAME); }
#define MOCK_FUNCTION_VOID(NAME) void NAME () override { return call(#NAME); }

#define MOCK_FUNCTION1(RET, NAME, T1) RET NAME (T1 a1) override { return call<RET>(#NAME, a1); }
#define MOCK_FUNCTION1_VOID(NAME, T1) void NAME (T1 a1) override { return call(#NAME, a1); }

#define MOCK_FUNCTION2(RET, NAME, T1, T2) RET NAME (T1 a1, T2 a2) override { return call<RET>(#NAME, a1, a2); }
#define MOCK_FUNCTION2_VOID(NAME, T1, T2) void NAME (T1 a1, T2 a2) override { return call(#NAME, a1, a2); }

#endif
