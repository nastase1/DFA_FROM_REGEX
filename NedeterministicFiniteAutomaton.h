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

    int prioritate(char op);

    bool verificaParanteze(const std::string& expresie);

    Automaton createBasicAutomaton(char symbol);

    Automaton alternateAutomaton(const Automaton& a, const Automaton& b);

    Automaton concatenateAutomaton(const Automaton& a, const Automaton& b);

    Automaton kleeneStarAutomaton(const Automaton& a);

public:

	NedeterministicFiniteAutomaton();

    std::string FormaPoloneza(std::string& expresie);

    Automaton buildAutomaton(std::string& fp);
};




