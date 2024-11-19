#pragma once
#include<iostream>
#include<set>
#include<string>
#include<map>
#include<iomanip>

class DeterministicFiniteAutomaton
{
private:
	const std::set<std::string> states;//Q
	const std::set<std::string> alphabet;//Σ
	const std::map<std::pair<std::string, std::string>, std::string> transitionFunction;//δ
	const std::string initialState;//q0
	const std::set<std::string> finalStates;//F

public:

	DeterministicFiniteAutomaton(const std::set<std::string> states, const std::set<std::string> alphabet,
		const std::map<std::pair<std::string, std::string>, std::string> transitionFunction,
		const std::string initialState,
		const std::set<std::string> finalStates);

	bool VerifyAutomaton();

	bool CheckWord(const std::string word);

	friend std::ostream& operator <<(std::ostream& os,const DeterministicFiniteAutomaton& dfa);
};

