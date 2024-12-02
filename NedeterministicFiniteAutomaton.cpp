#include "NedeterministicFiniteAutomaton.h"

int NedeterministicFiniteAutomaton::priority(char op)
{
    if (op == '(')
        return 0;
    if (op == '|')
        return 1;
    if (op == '.')
        return 2;
    if (op == '*')
        return 3;
    return -1;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::createBasicAutomaton(char symbol)
{
    int start = stateCounter++;
    int end = stateCounter++;
    return Automaton{ start, end, {{start, symbol, end}} };
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::alternateAutomaton(const Automaton& a, const Automaton& b)
{
    int start = stateCounter++;
    int end = stateCounter++;
    Automaton result{ start,end,a.transitions };
    result.transitions.insert(result.transitions.end(), b.transitions.begin(), b.transitions.end());
    result.transitions.push_back({ start,'\0',a.startState });
    result.transitions.push_back({ start,'\0',b.startState });
    result.transitions.push_back({ a.startState,'\0',end });
    result.transitions.push_back({ b.startState,'\0',a.startState });

    return result;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::concatenateAutomaton(const Automaton& a, const Automaton& b)
{
    int start = stateCounter++;
    int end = stateCounter++;
    Automaton result{ start,end,a.transitions };
    result.transitions.insert(result.transitions.end(), b.transitions.begin(), b.transitions.end());
    result.transitions.push_back({ a.finalState,'\0',b.startState });
    return result;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::kleeneStarAutomaton(const Automaton& a)
{
    int start = stateCounter++;
    int end = stateCounter++;
    Automaton result{ start, end, a.transitions };
    result.transitions.push_back({ start, '\0', a.startState });
    result.transitions.push_back({ a.finalState, '\0', a.startState });
    result.transitions.push_back({ start, '\0', end });
    result.transitions.push_back({ a.finalState, '\0', end });
    return result;
}

NedeterministicFiniteAutomaton::NedeterministicFiniteAutomaton() :stateCounter{ 0 }
{
}

std::string NedeterministicFiniteAutomaton::PolishForm( std::string& expresion)
{
    std::string pf;
    std::stack<char> operators;

    for (size_t i = 0; i < expresion.size(); ++i) {
        char current = expresion[i];

        if (isalnum(current)) {
            pf.push_back(current);

            if (i + 1 < expresion.size() && (isalnum(expresion[i + 1]) || expresion[i + 1] == '(')) {
                while (!operators.empty() && priority(operators.top()) >= priority('.')) {
                    pf.push_back(operators.top());
                    operators.pop();
                }
                operators.push('.');
            }
        }
        else if (current == '(') {
            operators.push(current);
        }
        else if (current == ')') {
            while (!operators.empty() && operators.top() != '(') {
                pf.push_back(operators.top());
                operators.pop();
            }
            if (!operators.empty())
                operators.pop();
        }
        else if (current == '*' || current == '|' || current == '.') {
            while (!operators.empty() && priority(operators.top()) >= priority(current)) {
                pf.push_back(operators.top());
                operators.pop();
            }
            operators.push(current);
        }
    }

    while (!operators.empty()) {
        pf.push_back(operators.top());
        operators.pop();
    }

    return pf;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::buildAutomaton(std::string& expresion)
{
    std::string polishForm = PolishForm(expresion);
    std::stack<Automaton> SA;

    for (char c : polishForm) {
        if (isalnum(c))
            SA.push(createBasicAutomaton(c));
        else if (c == '|') {
            Automaton b = SA.top();
            SA.pop();
            Automaton a = SA.top();
            SA.pop();
            SA.push(alternateAutomaton(a, b));
        }
        else if (c == '.') {
            Automaton b = SA.top();
            SA.pop();
            Automaton a = SA.top();
            SA.pop();
            SA.push(concatenateAutomaton(a, b));
        }
        else if (c == '*') {
            Automaton a = SA.top();
            SA.pop();
            SA.push(kleeneStarAutomaton(a));
        }
    }

    return SA.top();

}
