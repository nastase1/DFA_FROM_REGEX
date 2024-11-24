#include "NedeterministicFiniteAutomaton.h"

int NedeterministicFiniteAutomaton::prioritate(char op)
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

bool NedeterministicFiniteAutomaton::verificaParanteze(const std::string& expresie)
{
    std::stack<char> paranteze;
    for (char c : expresie) {
        if (c == '(') {
            paranteze.push(c);
        }
        else if (c == ')') {
            if (paranteze.empty()) {
                return false;
            }
            paranteze.pop();
        }
    }
    return paranteze.empty();
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

std::string NedeterministicFiniteAutomaton::FormaPoloneza( std::string& expresie)
{
    std::string fp;
    std::stack<char> operatori;

    for (size_t i = 0; i < expresie.size(); ++i) {
        char curent = expresie[i];

        if (isalnum(curent)) {
            fp.push_back(curent);

            if (i + 1 < expresie.size() && (isalnum(expresie[i + 1]) || expresie[i + 1] == '(')) {
                while (!operatori.empty() && prioritate(operatori.top()) >= prioritate('.')) {
                    fp.push_back(operatori.top());
                    operatori.pop();
                }
                operatori.push('.');
            }
        }
        else if (curent == '(') {
            operatori.push(curent);
        }
        else if (curent == ')') {
            while (!operatori.empty() && operatori.top() != '(') {
                fp.push_back(operatori.top());
                operatori.pop();
            }
            if (!operatori.empty())
                operatori.pop();
        }
        else if (curent == '*' || curent == '|' || curent == '.') {
            while (!operatori.empty() && prioritate(operatori.top()) >= prioritate(curent)) {
                fp.push_back(operatori.top());
                operatori.pop();
            }
            operatori.push(curent);
        }
    }

    while (!operatori.empty()) {
        fp.push_back(operatori.top());
        operatori.pop();
    }

    return fp;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::buildAutomaton(std::string& expresion)
{
    std::string polishForm = FormaPoloneza(expresion);
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
