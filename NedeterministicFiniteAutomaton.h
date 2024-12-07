#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <vector>
#include <tuple>
#include<set>
#include<map>
#include"DeterministicFiniteAutomaton.h"
#include<queue>

class NedeterministicFiniteAutomaton
{
private:

    struct Automaton {
        int startState;
        int finalState;
        std::vector<std::tuple<int, char, int>> transitions;
        std::set<int> finalStates; // Set de stări finale
        std::set<char> alphabet;
    };

    int stateCounter = 0;

    Automaton automaton;

    int priority(char op);

    Automaton createBasicAutomaton(char symbol);

    Automaton alternateAutomaton(const Automaton& a, const Automaton& b);

    Automaton concatenateAutomaton(const Automaton& a, const Automaton& b);

    Automaton kleeneStarAutomaton(const Automaton& a);

    std::set<char> alphabet;
public:

	NedeterministicFiniteAutomaton();

    std::string PolishForm(std::string& expresion);

    Automaton buildAutomaton(std::string& expresion);

    std::set<int> lambdaClosure(int state, const std::vector<std::tuple<int, char, int>>& transitions);

    std::map<int, std::set<int>> computeLambdaClosures();

    DeterministicFiniteAutomaton toDeterministic();

};




