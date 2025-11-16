#ifndef AST_HPP_
#define AST_HPP_

#include <cstdio>
#include <string>
#include <typeinfo>
#include <vector>
#include <map>

#include "../error/error.hpp"

constexpr void indent(const int count, FILE *fp)
{
    for (int i = 0; i < count * 2; i++)
        fputc(' ', fp);
}

/**
 * @brief 式を表現するクラス
 * BNF上でいうexpr1, expr2, expr3, expr4, factor解析時のデータ構造
 */
class BaseAST
{
public:
    BaseAST() {}
    virtual ~BaseAST() {}

    virtual void dump(const int indent, FILE *fp);
};

/**
 * @brief A+Bのような演算式を表現するクラス
 */
class EquationAST : public BaseAST
{
public:
    BaseAST *lhs;
    BaseAST *rhs;
    std::string op;

    EquationAST(BaseAST *lhs, BaseAST *rhs, std::string op) : lhs(lhs), rhs(rhs), op(op) {}
    ~EquationAST()
    {
        delete lhs;
        delete rhs;
    }

    void dump(const int indentcount, FILE *fp);
};

/**
 * @brief 整数の即値を表現するクラス
 */
class ImmediateIntAST : public BaseAST
{
public:
    std::string value; // ビット数制限をなくすために文字列

    ImmediateIntAST(std::string val) : value(val) {}
    ~ImmediateIntAST() {}

    void dump(const int indentcount, FILE *fp);
};

class VariableAST : public BaseAST
{
public:
    std::string name;

    VariableAST(std::string n) : name(n) {}
    ~VariableAST() {}

    void dump(const int indentcount, FILE *fp);
};

class ImmediateStringAST : public BaseAST
{
public:
    std::string value;

    ImmediateStringAST(std::string val) : value(val) {}
    ~ImmediateStringAST() {}

    void dump(const int indentcount, FILE *fp);
};

/**
 * @brief 変数定義のAST
 */
class DefineVariableAST : public BaseAST
{
public:
    std::string type;
    std::string dest;
    BaseAST *value;

    DefineVariableAST(std::string type, std::string dest, BaseAST *val) : type(type), dest(dest), value(val) {}
    ~DefineVariableAST()
    {
        delete value;
    }

    void dump(const int indentcount, FILE *fp);
};

/**
 * @brief 変数代入のAST
 */
class AssignAST : public BaseAST
{
public:
    std::string dest;
    BaseAST *value;

    AssignAST(std::string dest, BaseAST *val) : dest(dest), value(val) {}
    ~AssignAST()
    {
        delete value;
    }

    void dump(const int indentcount, FILE *fp);
};

class FunctionCallAST : public BaseAST
{
public:
    std::string name;
    std::vector<BaseAST *> args;

    FunctionCallAST(std::string name, std::vector<BaseAST *> args) : name(name), args(args) {}
    ~FunctionCallAST()
    {
        for (auto &&i : args)
        {
            delete i;
        }
    }

    void dump(const int indentcount, FILE *fp);
};

class BaseStatementAST
{
public:
    BaseStatementAST() {}
    virtual ~BaseStatementAST() {}

    virtual void dump(const int indentcount, FILE *fp);
};

class ExprStatementAST : public BaseStatementAST
{
public:
    BaseAST *expr;

    ExprStatementAST(BaseAST *expr) : expr(expr) {}
    ~ExprStatementAST()
    {
        delete expr;
    };

    void dump(const int indentcount, FILE *fp);
};

class BlockAST
{
public:
    std::vector<BaseStatementAST *> statements;

    BlockAST(std::vector<BaseStatementAST *> statements) : statements(statements) {};
    ~BlockAST()
    {
        for (auto &&i : statements)
        {
            delete i;
        }
    };

    void dump(const int indentcount, FILE *fp);
};

class IfStatementAST : public BaseStatementAST
{
public:
    BaseAST *condition;
    BlockAST *block;
    BlockAST *elseBlock;

    IfStatementAST(BaseAST *condition, BlockAST *block, BlockAST *elseBlock)
        : condition(condition), block(block), elseBlock(elseBlock) {}
    ~IfStatementAST()
    {
        delete condition;
        delete block;
        delete elseBlock;
    }

    void dump(const int indentcount, FILE *fp);
};

class WhileStatementAST : public BaseStatementAST
{
public:
    BaseAST *condition;
    BlockAST *block;

    WhileStatementAST(BaseAST *condition, BlockAST *block)
        : condition(condition), block(block) {};
    ~WhileStatementAST()
    {
        delete condition;
        delete block;
    }

    void dump(const int indentcount, FILE *fp);
};

class ReturnStatementAST : public BaseStatementAST
{
public:
    BaseAST *expr; // nullable pointer

    ReturnStatementAST(BaseAST *expr) : expr(expr) {}
    ~ReturnStatementAST()
    {
        delete expr;
    }

    void dump(const int indentcount, FILE *fp);
};

class FunctionDefineAST : public BaseStatementAST
{
public:
    std::string name;
    // keyは引数名
    std::string returnType;
    std::map<std::string, std::string> arguments;
    BlockAST *block; // nullable pointer

    FunctionDefineAST(std::string name, std::string returnType,
                      std::map<std::string, std::string> arguments, BlockAST *block)
        : name(name), returnType(returnType), arguments(arguments), block(block) {}
    ~FunctionDefineAST()
    {
        delete block;
    }

    void dump(const int indentcount, FILE *fp);
};

class ProgramAST
{
public:
    BlockAST *block;

    ProgramAST(BlockAST *block) : block(block) {}
    ~ProgramAST()
    {
        delete block;
    }

    void dump(const int indentcount, FILE *fp = stdout);
};
#endif
