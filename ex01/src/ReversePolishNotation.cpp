#include "ReversePolishNotation.hpp"
#include <sstream>
#include <cctype>

ReversePolishNotation::~ReversePolishNotation() {}
ReversePolishNotation::ReversePolishNotation() {}
ReversePolishNotation::ReversePolishNotation(const ReversePolishNotation& other) {}
ReversePolishNotation& ReversePolishNotation::operator=(const ReversePolishNotation& other){ return *this; }

ReversePolishNotation::Node::Node(int v)
    : isOp(false), op(0), val(v), left(NULL), right(NULL) {}

ReversePolishNotation::Node::Node(char o, Node* l, Node* r)
    : isOp(true), op(o), val(0), left(l), right(r) {}

static bool isOperator(const std::string& token) {
    return token.size() == 1
        && (token[0] == '+' || token[0] == '-'
         || token[0] == '*' || token[0] == '/');
}

static bool isInteger(const std::string& token) {
    if (token.empty()) return false;
    size_t i = (token[0] == '-') ? 1 : 0;
    if (i == token.size()) return false;
    for (; i < token.size(); ++i)
        if (!std::isdigit(token[i])) return false;
    return true;
}

ReversePolishNotation::Node* ReversePolishNotation::buildTree(const std::string& expr) {
    std::stack<Node*> stk;
    std::istringstream ss(expr);
    std::string token;

    while (ss >> token) {
        if (isOperator(token)) {
            if (stk.size() < 2) {
                while (!stk.empty()) { freeTree(stk.top()); stk.pop(); }
                throw std::invalid_argument("Error: not enough operands");
            }
            Node* right = stk.top(); stk.pop();
            Node* left  = stk.top(); stk.pop();
            stk.push(new Node(token[0], left, right));
        } else if (isInteger(token)) {
            std::istringstream ns(token);
            int v; ns >> v;
            stk.push(new Node(v));
        } else {
            while (!stk.empty()) { freeTree(stk.top()); stk.pop(); }
            throw std::invalid_argument("Error: invalid token '" + token + "'");
        }
    }

    if (stk.empty())
        throw std::invalid_argument("Error: empty expression");
    if (stk.size() > 1) {
        while (!stk.empty()) { freeTree(stk.top()); stk.pop(); }
        throw std::invalid_argument("Error: too many operands");
    }

    return stk.top();
}

int ReversePolishNotation::evalNode(Node* node) {
    if (!node->isOp)
        return node->val;

    int left  = evalNode(node->left);
    int right = evalNode(node->right);

    switch (node->op) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/':
            if (right == 0)
                throw std::runtime_error("Error: division by zero");
            return left / right;
    }
    throw std::invalid_argument("Error: unknown operator");
}

void ReversePolishNotation::freeTree(Node* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

int ReversePolishNotation::evaluate(const std::string& expr) {
    Node* root = buildTree(expr);
    int result;
    try {
        result = evalNode(root);
    } catch (...) {
        freeTree(root);
        throw;
    }
    freeTree(root);
    return result;
}
