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

std::set<int> NedeterministicFiniteAutomaton::lambdaClosure(int state, const std::vector<std::tuple<int, char, int>>& transitions)
{
        std::set<int> closure = { state };
        std::stack<int> stack;
        stack.push(state);

        while (!stack.empty()) {
            int current = stack.top();
            stack.pop();

            for (const auto& [src, symbol, dest] : transitions) {
                if (src == current && symbol == '\0' && closure.find(dest) == closure.end()) {
                    closure.insert(dest);
                    stack.push(dest);
                }
            }
        }

        return closure;
}

std::map<int, std::set<int>> NedeterministicFiniteAutomaton::computeLambdaClosures()
{
        std::map<int, std::set<int>> closures;

        for (int state = 0; state < stateCounter; ++state) {
            closures[state] = lambdaClosure(state, automaton.transitions);
        }

        return closures;

}

DeterministicFiniteAutomaton NedeterministicFiniteAutomaton::toDeterministic()
{
        auto lambdaClosures = computeLambdaClosures();

        std::map<std::set<int>, std::string> stateMapping; // Map de mul?imi de st?ri c?tre nume de stare
        std::set<std::string> newStates; // St?ri DFA
        std::set<std::string> newFinalStates; // St?ri finale DFA
        std::map<std::pair<std::string, std::string>, std::string> newTransitions; // Func?ia de tranzi?ie DFA

        std::queue<std::set<int>> queue;
        std::set<int> startClosure = lambdaClosures[automaton.startState];
        queue.push(startClosure);
        stateMapping[startClosure] = "q0"; // Prima stare a DFA
        newStates.insert("q0");

        int stateCount = 1;

        while (!queue.empty()) {
            auto currentSet = queue.front();
            queue.pop();
            std::string currentName = stateMapping[currentSet];

            for (char symbol : alphabet) {
                std::set<int> newSet;

                for (int state : currentSet) {
                    for (const auto& [src, sym, dest] : automaton.transitions) {
                        if (src == state && sym == symbol) {
                            newSet.insert(lambdaClosures[dest].begin(), lambdaClosures[dest].end());
                        }
                    }
                }

                if (!newSet.empty()) {
                    if (stateMapping.find(newSet) == stateMapping.end()) {
                        stateMapping[newSet] = "q" + std::to_string(stateCount++);
                        newStates.insert(stateMapping[newSet]);
                        queue.push(newSet);

                        // Verific?m dac? este stare final?
                        for (int finalState : finalStates) {
                            if (newSet.find(finalState) != newSet.end()) {
                                newFinalStates.insert(stateMapping[newSet]);
                            }
                        }
                    }

                    newTransitions[{currentName, std::string(1, symbol)}] = stateMapping[newSet];
                }
            }
        }

        return DeterministicFiniteAutomaton(newStates, alphabet, newTransitions, "q0", newFinalStates);
}
