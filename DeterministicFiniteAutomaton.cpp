#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const std::set<std::string> states, const std::set<std::string> alphabet, const std::map<std::pair<std::string, std::string>, std::string> transitionFunction, const std::string initialState, const std::set<std::string> finalStates)
	:states{ states }, alphabet{ alphabet }, transitionFunction{ transitionFunction }, initialState{ initialState }, finalStates{ finalStates }
{
}

bool DeterministicFiniteAutomaton::VerifyAutomaton()
{
	if (states.find(initialState) == states.end())
		return false;

	for (const auto& state : finalStates) {
		if (states.find(state) == states.end())
			return false;
	}

	if (alphabet.empty())
		return false;

	for (const auto& transition : transitionFunction) {
		const auto& key = transition.first;
		const auto& resultState = transition.second;

		const std::string& currentState = key.first;
		const std::string& symbol = key.second;

		if (states.find(currentState) == states.end()) {
			return false;
		}

		if (alphabet.find(symbol) == alphabet.end()) {
			return false;
		}

		if (states.find(resultState) == states.end()) {
			return false;
		}
	}

	if (states.empty())
		return false;

	if (finalStates.empty())
		return false;

	return true;
}

bool DeterministicFiniteAutomaton::CheckWord(const std::string word)
{
	std::string currentState = initialState;

	for (const auto& character : word) {
		std::string symbol(1, character);
		if (alphabet.find(symbol) == alphabet.end())
			return false;
		auto it = transitionFunction.find({ currentState,symbol });
		if (it == transitionFunction.end())
			return false;
		currentState = it->second;
	}

	return finalStates.find(currentState) != finalStates.end();
}

std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& dfa) 
{
	os << std::setw(3) << "Q \\ Σ" << " || ";
    for (const auto& symbol : dfa.alphabet) {
        os << std::setw(10) << symbol;
    }
    os << " ||\n";

    os << std::string(12 + dfa.alphabet.size() * 10, '-') << "\n";

    for (const auto& state : dfa.states) {
		if (state == dfa.initialState) {
			os << "->" << std::setw(3) << state << " || ";
		}
		else if (dfa.finalStates.find(state) != dfa.finalStates.end()) {
			os << "*" << std::setw(4) << state << " || ";
		}
		else if (state == dfa.initialState && dfa.finalStates.find(state) != dfa.finalStates.end()) {
			os << "->*" << std::setw(2) << state << " || ";
		}
		else
			os << std::setw(5) << state << " || ";
        for (const auto& symbol : dfa.alphabet) {
            auto it = dfa.transitionFunction.find({ state, symbol });
            if (it != dfa.transitionFunction.end()) {
                os << std::setw(10) << it->second;
            }
            else {
                os << std::setw(10) << "-";
            }
        }
        os << " ||\n";
    }

    return os;
}

