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
NedeterministicFiniteAutomaton::createBasicAutomaton(char symbol) {
    int start = stateCounter++;
    int end = stateCounter++;

    Automaton result{ start, end, {{start, symbol, end}}, {end}, {} };
    if (symbol != '\0') {
        result.alphabet.insert(symbol); // Adăugăm simbolul în alfabet
    }
    result.finalStates = {end};
    return result;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::alternateAutomaton(const Automaton& a, const Automaton& b) {
    int start = stateCounter++;
    int end = stateCounter++;

    Automaton result{ start, end, a.transitions };
    result.transitions.insert(result.transitions.end(), b.transitions.begin(), b.transitions.end());
    result.transitions.push_back({ start, '\0', a.startState });
    result.transitions.push_back({ start, '\0', b.startState });
    result.transitions.push_back({ a.finalState, '\0', end });
    result.transitions.push_back({ b.finalState, '\0', end });

    result.finalStates = { end };
    result.alphabet.insert(a.alphabet.begin(), a.alphabet.end());
    result.alphabet.insert(b.alphabet.begin(), b.alphabet.end());

    return result;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::concatenateAutomaton(const Automaton& a, const Automaton& b) {
    int start = stateCounter++;
    int end = stateCounter++;

    Automaton result{ start, end, a.transitions };
    result.transitions.insert(result.transitions.end(), b.transitions.begin(), b.transitions.end());
    result.transitions.push_back({ a.finalState, '\0', b.startState });

    result.finalStates = { end };
    result.alphabet.insert(a.alphabet.begin(), a.alphabet.end());
    result.alphabet.insert(b.alphabet.begin(), b.alphabet.end());

    return result;
}

NedeterministicFiniteAutomaton::Automaton
NedeterministicFiniteAutomaton::kleeneStarAutomaton(const Automaton& a) {
    int start = stateCounter++;
    int end = stateCounter++;

    Automaton result{ start, end, a.transitions };
    result.transitions.push_back({ start, '\0', a.startState });
    result.transitions.push_back({ a.finalState, '\0', a.startState });
    result.transitions.push_back({ start, '\0', end });
    result.transitions.push_back({ a.finalState, '\0', end });

    result.finalStates = { end };
    result.alphabet.insert(a.alphabet.begin(), a.alphabet.end());

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

    Automaton finalAutomaton = SA.top();
    finalAutomaton.finalStates = { finalAutomaton.finalState };
    automaton = finalAutomaton; // Salvează automatul construit
    return finalAutomaton;

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


DeterministicFiniteAutomaton NedeterministicFiniteAutomaton::toDeterministic() {
    auto lambdaClosures = computeLambdaClosures();
    std::cout << "Lambda-închideri pentru fiecare stare:\n";
    for (const auto& [state, closure] : lambdaClosures) {
        std::cout << "Stare " << state << ": { ";
        for (int c : closure) std::cout << c << " ";
        std::cout << "}\n";
    }
    // Map to track state sets to DFA states
    std::map<std::set<int>, std::string> stateMapping;
    std::set<std::string> newStates;
    std::set<std::string> newFinalStates;
    std::map<std::pair<std::string, std::string>, std::string> newTransitions;

    // Queue for processing DFA states
    std::queue<std::set<int>> queue;

    // Initial DFA state is the lambda closure of the NFA start state
    std::set<int> startClosure = lambdaClosures[automaton.startState];
    stateMapping[startClosure] = "q0";
    newStates.insert("q0");
    queue.push(startClosure);

    static int stateCount = 0;

    // Process DFA states
    while (!queue.empty()) {
        auto currentSet = queue.front();
        queue.pop();
        std::string currentName = stateMapping[currentSet];

        // Parcurgem fiecare simbol din alfabet
        for (char symbol : automaton.alphabet) {
            std::set<int> newSet;

            // Pentru fiecare stare din setul curent
            for (int state : currentSet) {
                // Găsim toate tranzițiile din această stare pe simbolul curent
                for (const auto& [src, sym, dest] : automaton.transitions) {
                    if (src == state && sym == symbol) {
                        // Adăugăm lambda-închiderea stării de destinație
                        newSet.insert(lambdaClosures[dest].begin(), lambdaClosures[dest].end());
                    }
                }
            }

            // Dacă am găsit un set nou de stări
            if (!newSet.empty()) {
                // Dacă setul de stări nu există încă în DFA, îl adăugăm
                if (stateMapping.find(newSet) == stateMapping.end()) {
                    std::string newName = "q" + std::to_string(stateCount++);
                    stateMapping[newSet] = newName;
                    newStates.insert(newName);
                    queue.push(newSet);

                    // Verificăm dacă este stare finală
                    for (int finalState : automaton.finalStates) {
                        if (newSet.find(finalState) != newSet.end()) {
                            newFinalStates.insert(newName);
                            break;
                        }
                    }
                }

                // Adăugăm tranziția din DFA
                newTransitions[{currentName, std::string(1, symbol)}] = stateMapping[newSet];
            }
        }
    }


    // Convert alphabet to std::set<std::string>
    std::set<std::string> stringAlphabet;
    for (char c : automaton.alphabet) {
        stringAlphabet.insert(std::string(1, c));
    }

    // Return the DFA
    return DeterministicFiniteAutomaton(
        newStates,           // DFA states
        stringAlphabet,      // DFA alphabet
        newTransitions,      // DFA transition function
        "q0",                // Initial state
        newFinalStates       // Final states
    );
}
