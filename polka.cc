#include "polka.h"

#include <iostream>
#include <stack>

#include "constant.h"
#include "expression.h"
#include "pair.h"

using namespace std;

Polka::Polka()
{}

int Polka::size()
{
    return s.size();
}

int cast_to_int(Expression *e)
{
    auto *c = dynamic_cast<Constant *>(e);
    if (c)
        return c->value;
    std::cerr << e->to_string() << std::endl;
    exit(1);
}

void Polka::push(int x)
{
    return s.push(new Constant(x));
}

Expression *Polka::get_value()
{
    if (s.empty())
    {
        std::cerr << "error" << std::endl;
        exit(1);
    }
    Expression *e = s.top();
    return e;
}

void Polka::push(Expression *a, Expression *b)
{
    s.push(dynamic_cast<Expression *>(new Pair(a, b)));
}

Pair *cast_to_pair(Expression *e)
{
    auto *p = dynamic_cast<Pair *>(e);
    if (p)
    {
        return p;
    }
    std::cerr << "error" << std::endl;
    exit(1);
}

void Polka::push(string token)
{
    if (token == ":")
    {
        if (s.empty())
        {
            std::cerr << "error" << std::endl;
            exit(1);
        }
        auto *c = dynamic_cast<Constant*>(s.top());
        auto *p = dynamic_cast<Pair*>(s.top());
        if (c)
            push(c->value);
        else if (p)
            push(p->fst, p->snd);
        else
        {
            std::cerr << "error" << std::endl;
            exit(1);
        }
    }

    else if (token == "><")
    {
        if (s.size() < 2)
        {
            std::cerr << "error" << std::endl;
            exit(1);
        }
        Expression *a = s.top();
        s.pop();
        Expression *b = s.top();
        s.pop();
        s.push(a);
        s.push(b);
    }

    else if (token == "$1" || token == "$2")
    {
        if (s.empty())
        {
            std::cerr << "error" << std::endl;
            exit(1);
        }
        Pair *p = cast_to_pair(s.top());
        s.pop();
        if (token == "$1")
            s.push(p->fst);
        if (token == "$2")
            s.push(p->snd);
    }

    else if (token == "*" || token == "+" || token == "-" || token == "/")
    {
        if (s.size() < 2)
        {
            std::cerr << "error" << std::endl;
            exit(1);
        }
        Constant *c_a = dynamic_cast<Constant *>(s.top());
        auto topA = s.top();
        s.pop();
        Constant *c_b = dynamic_cast<Constant *>(s.top());
        auto topB = s.top();
        s.pop();
        if (!c_a || !c_b)
            exit(1);
        int a = c_a->value;
        int b = c_b->value;
        if (token == "+")
            s.push(new Constant(b + a));
        if (token == "-")
            s.push(new Constant(b - a));
        if (token == "*")
            s.push(new Constant(b * a));
        if (token == "/")
        {
            if (a == 0)
            {
                std::cerr << "error" << std::endl;
                exit(1);
            }
            s.push(new Constant(b / a));
        }
        //delete c_a;
        //delete c_b;
    }

    else if (token == "@")
    {
        Expression *a = s.top();
        s.pop();
        Expression *b = s.top();
        s.pop();
        s.push(new Pair(a, b));
    }

    else
    {
        std::cerr << "error" << std::endl;
        exit(1);
    }
}

string Polka::to_string()
{
    std::string res = "[";
    std::stack<Expression *> temp;
    while (!s.empty())
    {
        res += s.top()->to_string();
        temp.push(s.top());
        s.pop();
        if (!s.empty())
            res += ",";
    }
    while (!temp.empty())
    {
        s.push(temp.top());
        temp.pop();
    }
    return res + "]";
}
