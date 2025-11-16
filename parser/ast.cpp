#include "ast.hpp"

void BaseAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "!!!BaseAST()\n");
}

void EquationAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "Equation(operator: %s\n", op.c_str());

    indent(indentcount + 1, fp);
    fprintf(fp, "lhs: ");
    lhs->dump(indentcount + 1, fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "rhs: ");
    rhs->dump(indentcount + 1, fp);

    indent(indentcount, fp);
    fprintf(fp, ")\n");
}

void ImmediateIntAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "ImmediateInt(value: %s)\n", value.c_str());
}

void VariableAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "Variable(name: %s)\n", name.c_str());
}

void ImmediateStringAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "ImmediateString(value: \"%s\")\n", value.c_str());
}

void DefineVariableAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "DefineVariable(type: %s, dest: %s\n", type.c_str(), dest.c_str());
    indent(indentcount + 1, fp);
    fprintf(fp, "value: ");
    value->dump(indentcount + 1, fp);
    indent(indentcount, fp);
    fprintf(fp, ")\n");
}

void AssignAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "Assign(dest: %s\n", dest.c_str());
    indent(indentcount + 1, fp);
    fprintf(fp, "value: ");
    value->dump(indentcount + 1, fp);
    indent(indentcount, fp);
    fprintf(fp, ")\n");
}

void FunctionCallAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "FunctionCall(name: %s\n", name.c_str());
    indent(indentcount + 1, fp);

    fprintf(fp, "args: [\n");
    for (auto &&i : args)
    {
        indent(indentcount + 2, fp);
        i->dump(indentcount + 2, fp);
    }
    indent(indentcount + 1, fp);
    fprintf(fp, "]\n");
    indent(indentcount, fp);
    fprintf(fp, ")\n");
}

void BaseStatementAST::dump(const int indentcount, FILE *fp)
{
    fputs("!!!BaseStatement()\n", fp);
}

void ExprStatementAST::dump(const int indentcount, FILE *fp)
{
    expr->dump(indentcount, fp);
}

void BlockAST::dump(int indentcount, FILE *fp)
{
    fprintf(fp, "Block: [\n");
    for (auto &&i : statements)
    {
        indent(indentcount + 1, fp);
        i->dump(indentcount + 1, fp);
    }
    indent(indentcount, fp);
    fputs("]\n", fp);
}

void IfStatementAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "If(condition: ");
    condition->dump(indentcount + 1, fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "true: ");
    block->dump(indentcount + 1, fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "false: ");
    elseBlock->dump(indentcount + 1, fp);

    indent(indentcount, fp);
    fputs(")\n", fp);
}

void WhileStatementAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "While(condition: ");
    condition->dump(indentcount + 1, fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "block: ");
    block->dump(indentcount + 1, fp);

    indent(indentcount, fp);
    fputs(")\n", fp);
}

void ReturnStatementAST::dump(const int indentcount, FILE *fp)
{
    fprintf(fp, "Return(expr: ");
    if (expr == nullptr)
    {
        fputs("(void return)", fp);
    }
    else
    {
        expr->dump(indentcount + 1, fp);
    }
    indent(indentcount, fp);
    fputs(")\n", fp);
}

void FunctionDefineAST::dump(const int indentcount, FILE *fp)
{
    fputs("FunctionDef(\n", fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "name: %s\n", name.c_str());

    indent(indentcount + 1, fp);
    fprintf(fp, "retType: %s\n", returnType.c_str());

    // print argument-list
    indent(indentcount + 1, fp);
    fputs("args: [\n", fp);
    for (auto &x : arguments)
    {
        indent(indentcount + 2, fp);
        fprintf(fp, "Arg(type: %s, name: %s)\n", x.second.c_str(), x.first.c_str());
    }
    indent(indentcount + 1, fp);
    fputs("]\n", fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "statements: ");

    if (block != nullptr)
    {
        block->dump(indentcount + 1, fp);
    }
    else
    {
        fputs("\n", fp);
    }

    indent(indentcount, fp);
    fputs(")\n", fp);
}

void ProgramAST::dump(const int indentcount, FILE *fp)
{
    fputs("Program(\n", fp);

    indent(indentcount + 1, fp);
    fprintf(fp, "block: ");
    block->dump(indentcount + 1, fp);

    indent(indentcount, fp);
    fputs(")\n", fp);
}
