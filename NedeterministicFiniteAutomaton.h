#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <vector>
#include <tuple>


class NedeterministicFiniteAutomaton
{
private:

    struct Automaton {
        int startState;
        int finalState;
        std::vector<std::tuple<int, char, int>> transitions;
    };

    int stateCounter = 0;

    int priority(char op);

    Automaton createBasicAutomaton(char symbol);

    Automaton alternateAutomaton(const Automaton& a, const Automaton& b);

    Automaton concatenateAutomaton(const Automaton& a, const Automaton& b);

    Automaton kleeneStarAutomaton(const Automaton& a);

public:

	NedeterministicFiniteAutomaton();

    std::string PolishForm(std::string& expresion);

    Automaton buildAutomaton(std::string& expresion);
};




