#include "dowhile.h"

DoWhile::DoWhile(unique_ptr<Expression> condition, unique_ptr<Body> body) :
    m_condition(std::move(condition)), m_body(std::move(body)) {}

void DoWhile::print(int indentation_level) const {
    cout << '\n' << setw(indentation_level) << " " << "Do-While Statement{\n";
    m_condition->print(indentation_level + 2);
    m_body->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* DoWhile::getCondition() const {
    return m_condition->getExpression();
}

vector<ASTNode*> DoWhile::getBody() const {
    return m_body->getStatements();
}