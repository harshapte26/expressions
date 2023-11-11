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
class Variable;

class Visitor {
public: 
    virtual void Visit(Constant &e) = 0;
    virtual void Visit(Op &e) = 0;
    virtual void Visit(Variable &e) = 0;
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

class Variable final: public Expression{

public:

    Variable(char v) : v_(v) {
        RegisterLoader("Variable", Load);
    }
    
    void Accept(Visitor &visitor) override {
        visitor.Visit(*this);
    }

    static std::unique_ptr<Expression> Load(std::istream &s)
    {
        char v;
        s >> v;
        return std::make_unique<Variable>(v);
    }

    char v_;


};

// class Equation : public Expression {
// public:
//   Equation(std::unique_ptr<Expression> expr) : expr_(std::move(expr)) {
//     RegisterLoader("Equation", Load);
//   }

//   void Accept(Visitor &visitor) override {
//     visitor.Visit(*this); 
//   }

//   static std::unique_ptr<Expression> Load(std::istream& s) {
//     return std::make_unique<Equation>(Expression::Load(s));
//   }

//     char op_ = '='; 

// // private:
//   std::unique_ptr<Expression> expr_;
// };

std::unique_ptr<Expression> parse(std::string t)
{   
    
    // if (t == "Equation") {
    //     return std::make_unique<Equation>(parse(t.substr(1))); 
    //     }
    
    // TODO :: logic to handle variable if ()

    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '+' || t[i] == '-')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '*' || t[i] == '/')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    for (int i = t.size(); i >= 0; i--)
        if (t[i] == '^')
            return std::make_unique<Op>(t[i], parse(t.substr(0, i)), parse(t.substr(i + 1)));
    
    // if a string is variable, check for that
    if (t.size() == 1)
            return std::make_unique<Variable>(t[0]);    
    return std::make_unique<Constant>(std::stoi(t));
    // return std::make_unique<Constant>(t);
}

class ComputeVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = e.x_;
    }

    void Visit(Variable &e) {
        result_ = e.v_;
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

    // void Visit(Equation &i) {
    //     i.expr_->Accept(*this);
    //     result_ = result_; 
    //     }

//private:
    double result_;
};

class SimplifyVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = e.x_;
    }

    void Visit(Variable &e) {
        result_ = e.v_;
    }

    void Visit(Op &e) {
        e.l_->Accept(*this);
        char l = result_[0];
        std::cout<<"this is l --> "<<l<<"\n";
        e.r_->Accept(*this);
        double r = result_[0]-'0';
        std::cout<<"this is r --> "<<r<<"\n";
        std::cout<<"this is op --> "<<e.op_<<"\n";
        switch (e.op_)
        {
        case '+':
            result_ = l + r;
            break;
        case '-':
            result_ = l - r;
            break;
        case '*':
            result_ = std::string(r, l);
            std::cout<<"this is result --> "<<result_<<"\n";
            break;
        case '/':
            result_ = l / r;
            break;
        default:
            std::cout << "invalid op " << e.op_;
            abort();
        }
    }

    // void Visit(Equation &i) {
    //     i.expr_->Accept(*this);
    //     result_ = result_; 
    //     }

//private:
    std::string result_;
};

class PrettyPrintVisitor: public Visitor {
public:
    void Visit(Constant &e) {
        result_ = (std::stringstream() << "(" << e.x_ << ")").str();
    }

    void Visit(Variable &e) {
        result_ = (std::stringstream() << "(" << e.v_ << ")").str();
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

//     void Visit(Equation &i) {
//         i.expr_->Accept(*this);
//         result_ = result_ + "= " + result_;
// }

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
    // std::string s = "1+2*3-4";
    // auto e = parse(s);

    // auto v = ComputeVisitor();
    // e->Accept(v);
    // // std::cout << v.result_ << std::endl;

    // auto v2 = PrettyPrintVisitor();
    // e->Accept(v2);
    // std::cout << v2.result_ << std::endl;

    auto g = parse("j-3");
    auto v3 = SimplifyVisitor();
    g->Accept(v3); // Prints 2

    auto v4 = PrettyPrintVisitor();
    g->Accept(v4); // Prints (2*1)=2

    std::cout << v3.result_ << std::endl;
    // std::cout << v4.result_ << std::endl;

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