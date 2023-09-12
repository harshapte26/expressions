#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <map>
#include <memory>

#include <gtest/gtest.h>

class Constant;
class Op;

class Visitor {
public: 
    virtual void Visit(Constant &e) = 0;
    virtual void Visit(Op &e) = 0;
};

class Expression
{
public:
    virtual ~Expression() = default;

    virtual void Accept(Visitor &visitor) = 0;
    
    using Loader = std::unique_ptr<Expression>(*)(std::istream &);

    static void RegisterLoader(std::string key, Loader loader) {
        loaders_[key] = loader;
    }

    static std::unique_ptr<Expression> Load(std::istream &s) {
        std::string key;
        s >> key;
        const auto i = loaders_.find(key);
        if (i == loaders_.end())
        {
            std::cerr << "error: found unexpected token " << key << std::endl;
            abort();
        }
        return i->second(s);
    }

private:
    static std::map<std::string, Loader> loaders_;
};

std::map<std::string, Expression::Loader> Expression::loaders_ = {};

class Constant final : public Expression
{
public:
    Constant(double x) : x_(x) {
        RegisterLoader("Constant", Load);
    }

    void Accept(Visitor &visitor) override {
        visitor.Visit(*this);
    }

    static std::unique_ptr<Expression> Load(std::istream &s)
    {
        double x;
        s >> x;
        return std::make_unique<Constant>(x);
    }

// private:
    double x_;
};

class Op final : public Expression
{
public:
    Op(char op, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op_(op), l_(std::move(l)),
          r_(std::move(r)) 
          {
              RegisterLoader("Op", Load);
          }

    void Accept(Visitor &visitor) override {
        visitor.Visit(*this);
    }

    static std::unique_ptr<Expression> Load(std::istream &s)
    {
        char op;
        s >> op;
        return std::make_unique<Op>(op, std::move(Expression::Load(s)), std::move(Expression::Load(s)));
    }

// private:
    char op_;
    std::unique_ptr<Expression> l_;
    std::unique_ptr<Expression> r_;
};

std::unique_ptr<Expression> parse(std::string t)
{
    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '+' || t[i] == '-')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '*' || t[i] == '/')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '^')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    return std::make_unique<Constant>(std::stoi(t));
}

class ComputeVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = e.x_;
    }

    void Visit(Op &e) {
        e.l_->Accept(*this);
        double l = result_;
        e.r_->Accept(*this);
        double r = result_;
        switch (e.op_)
        {
        case '^':
            result_ = std::pow(l, r);
            break;
        case '+':
            result_ = l + r;
            break;
        case '-':
            result_ = l - r;
            break;
        case '*':
            result_ = l * r;
            break;
        case '/':
            result_ = l / r;
            break;
        default:
            std::cout << "invalid op " << e.op_;
            abort();
        }
    }
//private:
    double result_;
};

class PrettyPrintVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = (std::stringstream() << "(" << e.x_ << ")").str();
    }

    void Visit(Op &e) {
        std::stringstream result;
        result << "(";
        e.l_->Accept(*this);
        result << result_ << e.op_;
        e.r_->Accept(*this);
        result << result_ << ")";
        result_ = result.str();
    }

    std::string result_;
};

class SaveVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = (std::stringstream() << "(" << e.x_ << ")").str();
    }

    void Visit(Op &e) {
        std::stringstream result;
        result << "(";
        e.l_->Accept(*this);
        result << result_ << e.op_;
        e.r_->Accept(*this);
        result << result_ << ")";
        result_ = result.str();
    }

    std::string result_;
};

TEST(Expressions, Test2) {
    std::string s = "1+2*3-4";
    auto e = parse(s);

    auto v = ComputeVisitor();
    e->Accept(v);
    std::cout << v.result_ << std::endl;

    auto v2 = PrettyPrintVisitor();
    e->Accept(v2);
    std::cout << v2.result_ << std::endl;

    // std::cout << e->compute() << std::endl;
    // std::cout << e->PrettyPrint2() << std::endl;
    // auto ss = std::stringstream();
    // e->Save(ss);
    // std::cout << ss.str() << std::endl;
    // ss.seekg(0);
    // auto ee = Expression::Load(ss);
    // std::cout << ee->PrettyPrint2() << std::endl;
    // auto ss_i1 = std::stringstream("Op + Constant 3 Constant 4");
    // auto ee1 = Expression::Load(ss_i1);
    // std::cout << ee1->PrettyPrint2() << std::endl;
}