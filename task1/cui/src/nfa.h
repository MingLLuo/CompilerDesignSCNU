/*
 * File: nfa.h
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the NFA class
 */
#ifndef NFA_H
#define NFA_H
#include "util.h"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

class NFAState {
  public:
    int id;
    bool is_final;
    std::string final_status;
    std::unordered_map<char, std::set<std::shared_ptr<NFAState>>> transitions;

    explicit NFAState(int state_id, bool final_state = false)
        : id(state_id), is_final(final_state) {}
};

class NFA {
  public:
    std::set<std::shared_ptr<NFAState>> states;
    std::shared_ptr<NFAState> start_state;
    std::set<char> symbols;

    NFA();

    ~NFA() = default;

    void mergeStatesFromNFA(const std::shared_ptr<NFA> &nfa);

    void copyCleanNFA(const std::shared_ptr<NFA> &nfa, bool withFinal);

    void copySymbols(const std::shared_ptr<NFA> &nfa);
    void setFinalStatus(const std::string &str) const;
};

void addETransitions(const std::vector<std::shared_ptr<NFAState>> &src_list,
                     const std::shared_ptr<NFAState> &dest);

std::shared_ptr<NFA> concatNFAs(const std::shared_ptr<NFA> &nfa1,
                                const std::shared_ptr<NFA> &nfa2);

std::shared_ptr<NFA> unionNFAs(const std::shared_ptr<NFA> &nfa1,
                               const std::shared_ptr<NFA> &nfa2);
std::shared_ptr<NFA> starNFA(const std::shared_ptr<NFA> &nfa);

std::shared_ptr<NFA> plusNFA(const std::shared_ptr<NFA> &nfa);
std::shared_ptr<NFA> quesNFA(const std::shared_ptr<NFA> &nfa);

#endif