#include<iostream>
#include "DeterministicFiniteAutomaton.h"

int main() {
    std::set<std::string> Q = { "q0", "q1", "q2" };
    std::set<std::string> Sigma = { "a", "b" };
    std::map<std::pair<std::string, std::string>, std::string> delta = {
        {{"q0", "a"}, "q1"},
        {{"q0", "b"}, "q0"},
        {{"q1", "a"}, "q2"},
        {{"q1", "b"}, "q0"},
        {{"q2", "a"}, "q2"},
        {{"q2", "b"}, "q2"}
    };
    std::string q0 = "q0";
    std::set<std::string> F = { "q2" };

    DeterministicFiniteAutomaton dfa(Q, Sigma, delta, q0, F);

    std::cout << dfa << "\n";

    std::string word = "aab";
    if (dfa.CheckWord(word)) {
        std::cout << "Cuvantul \"" << word << "\" este acceptat de automat.\n";
    }
    else {
        std::cout << "Cuvantul \"" << word << "\" NU este acceptat de automat.\n";
    }

    return 0;
}
