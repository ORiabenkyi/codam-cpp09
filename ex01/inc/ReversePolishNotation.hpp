#ifndef REVERSE_POLISH_NOTATION_HPP
#define REVERSE_POLISH_NOTATION_HPP

#include <string>
#include <stack>
#include <stdexcept>

class ReversePolishNotation {
public:
    virtual ~ReversePolishNotation() = 0;
    ReversePolishNotation();
    ReversePolishNotation(const ReversePolishNotation& other);
    ReversePolishNotation& operator=(const ReversePolishNotation& other);

    static int evaluate(const std::string& expr);

private:
    struct Node {
        bool    isOp;
        char    op;
        int     val;
        Node*   left;
        Node*   right;

        explicit Node(int v);
        Node(char o, Node* l, Node* r);
    };

    static Node* buildTree(const std::string& expr);
    static int   evalNode(Node* node);
    static void  freeTree(Node* node);
};

#endif
