#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const std::set<std::string> states, const std::set<std::string> alphabet, const std::map<std::pair<std::string, std::string>, std::string> transitionFunction, const std::string initialState, const std::set<std::string> finalStates)
	:states(states),alphabet(alphabet),transitionFunction(transitionFunction),initialState(initialState),finalStates(finalStates)
{
}

bool DeterministicFiniteAutomaton::VerifyAutomaton()
{
	if (states.find(initialState) == states.end())
		return false; // Starea initiala nevalida

	for (const auto& state : finalStates) {
		if (states.find(state) == states.end())
			return false; // Starile finale nevalide
	}

	if (alphabet.empty())
		return false; // Alfabet gol

	for (const auto& transition : transitionFunction) {
		const auto& key = transition.first;
		const auto& resultState = transition.second;

		const std::string& currentState = key.first;
		const std::string& symbol = key.second;

		if (states.find(currentState) == states.end()) {
			return false; // Stare invalida
		}

		if (alphabet.find(symbol) == alphabet.end()) {
			return false; // Simbol invalid
		}

		if (states.find(resultState) == states.end()) {
			return false; // Tranzitie spre o stare invalida
		}
	}

	if (states.empty())
		return false; // Nu exista stari

	if (finalStates.empty())
		return false; // Nu exista stari finale

	return true;
}
