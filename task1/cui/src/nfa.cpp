/*
 * File: nfa.cpp
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the NFA class
 */
#include "nfa.h"

/* NFA: constructor */
NFA::NFA() : start_state(nullptr) { this->symbols.insert(0); }

/* mergeStatesFromNFA: merge states from nfa to this
 * @param nfa: nfa to merge
 */
void NFA::mergeStatesFromNFA(const std::shared_ptr<NFA> &nfa) {
    // move all states of nfa to this, won't copy start state
    for (const auto &state : nfa->states) {
        states.insert(state);
    }
    // copy symbol table
    copySymbols(nfa);
}

/* copySymbols: copy symbols from nfa to this
  * @param nfa: nfa to copy
  */
void NFA::copySymbols(const std::shared_ptr<NFA> &nfa) {
    for (auto symbol : nfa->symbols) {
        symbols.insert(symbol);
    }
}

/* copyCleanNFA: copy nfa to this, with or without final states
 * @param nfa: nfa to copy
 * @param withFinal: copy final states or not
 */
void NFA::copyCleanNFA(const std::shared_ptr<NFA> &nfa, bool withFinal) {
    copySymbols(nfa);
    flush();
    std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> state_map;
    for (const auto &state : nfa->states) {
        auto new_state = std::make_shared<NFAState>(fresh());
        if (withFinal && state->is_final) {
            new_state->is_final = true;
            new_state->final_status = state->final_status;
        }
        // use old transitions first
        new_state->transitions = state->transitions;

        this->states.insert(new_state);
        state_map[state] = new_state;
    }
    for (const auto &state : this->states) {
        for (const auto &transition : state->transitions) {
            std::set<std::shared_ptr<NFAState>> new_nextStates;
            for (const auto &oldNext_state : transition.second) {
                if (state_map.find(oldNext_state) == state_map.end()) {
                    new_nextStates.insert(oldNext_state);
                } else {
                    new_nextStates.insert(state_map[oldNext_state]);
                }
            }
            state->transitions[transition.first] = new_nextStates;
        }
    }
    start_state = state_map[nfa->start_state];
}

/* setFinalStatus: set final status for all final states
 * @param str: final status
 */
void NFA::setFinalStatus(const std::string &str) const {
    for (const auto &state : states) {
        if (state->is_final) {
            state->final_status = str;
        }
    }
}

/* addETransitions: add epsilon transitions from src_list to dest
 * @param src_list: source states
 * @param dest: destination state
 */
void addETransitions(const std::vector<std::shared_ptr<NFAState>> &src_list,
                     const std::shared_ptr<NFAState> &dest) {
    for (const auto &state : src_list) {
        // Add epsilon transitions: state -- 0 -> dest
        state->transitions[0].insert(dest);
    }
}

/* concatNFAs: concatenate two NFAs
 * @param nfa1: first NFA
 * @param nfa2: second NFA
 * @return: concatenated NFA
 */
std::shared_ptr<NFA> concatNFAs(const std::shared_ptr<NFA> &nfa1,
                                const std::shared_ptr<NFA> &nfa2) {
    auto nfaConcat = std::make_shared<NFA>();
    // Copy states from nfa1
    nfaConcat->copyCleanNFA(nfa1, true);

    auto copy_nfa2 = std::make_shared<NFA>();
    copy_nfa2->copyCleanNFA(nfa2, true);

    // Add epsilon transitions from final states of nfa1 to nfa2's start state
    for (const auto &state : nfaConcat->states) {
        if (state->is_final)
            addETransitions({state}, copy_nfa2->start_state);
        state->is_final = false;
    }

    // Copy states from nfa2
    nfaConcat->mergeStatesFromNFA(copy_nfa2);

    return nfaConcat;
}

/* unionNFAs: union two NFAs
 * @param nfa1: first NFA
 * @param nfa2: second NFA
 * @return: union NFA
 */
std::shared_ptr<NFA> unionNFAs(const std::shared_ptr<NFA> &nfa1,
                               const std::shared_ptr<NFA> &nfa2) {
    auto nfaUnion = std::make_shared<NFA>();
    auto copy_nfa1 = std::make_shared<NFA>();
    auto copy_nfa2 = std::make_shared<NFA>();
    copy_nfa1->copyCleanNFA(nfa1, true);
    copy_nfa2->copyCleanNFA(nfa2, true);

    auto start_state = std::make_shared<NFAState>(fresh());
    nfaUnion->states.insert(start_state);
    nfaUnion->start_state = start_state;

    // Add epsilon transitions from the new start state to the start states of
    // nfa1 and nfa2
    addETransitions({start_state}, copy_nfa1->start_state);
    addETransitions({start_state}, copy_nfa2->start_state);
    nfaUnion->mergeStatesFromNFA(copy_nfa1);
    nfaUnion->mergeStatesFromNFA(copy_nfa2);
    return nfaUnion;
}

/* starNFA: create a star NFA
 * @param nfa: NFA to create star
 * @return: star NFA
 */
std::shared_ptr<NFA> starNFA(const std::shared_ptr<NFA> &nfa) {
    auto nfaStar = std::make_shared<NFA>();

    // Copy states from the original NFA
    nfaStar->copyCleanNFA(nfa, true);

    //  Add epsilon transitions to the original start state
    for (const auto &state : nfaStar->states) {
        if (state->is_final && state != nfaStar->start_state)
            addETransitions({state}, nfaStar->start_state);
    }
    // must add after add e, or a new e will be added
    nfaStar->start_state->is_final = true;
    nfaStar->start_state->final_status = nfaStar->start_state->final_status;
    return nfaStar;
}

std::shared_ptr<NFA> plusNFA(const std::shared_ptr<NFA> &nfa) {
    auto nfaPlus = std::make_shared<NFA>();

    // Copy states from the original NFA
    nfaPlus->copyCleanNFA(nfa, true);
    // Create a new start state and
    // add epsilon transitions to the original start state
    auto endState = std::make_shared<NFAState>(fresh(), true);

    for (const auto &state : nfaPlus->states) {
        if (state->is_final)
            addETransitions({state}, endState);
        state->is_final = false;
    }
    addETransitions({endState}, nfaPlus->start_state);
    nfaPlus->states.insert(endState);

    return nfaPlus;
}

/* quesNFA: create a question mark NFA
 * @param nfa: NFA to create question mark
 * @return: question mark NFA
 */
std::shared_ptr<NFA> quesNFA(const std::shared_ptr<NFA> &nfa) {
    auto nfaQues = std::make_shared<NFA>();

    nfaQues->copyCleanNFA(nfa, true);

    auto start_state = std::make_shared<NFAState>(fresh());
    auto final_state = std::make_shared<NFAState>(fresh(), true);

    addETransitions({start_state}, nfaQues->start_state);
    addETransitions({start_state}, final_state);

    for (const auto &state : nfaQues->states) {
        if (state->is_final)
            addETransitions({state}, final_state);
        state->is_final = false;
    }

    nfaQues->states.insert(start_state);
    nfaQues->states.insert(final_state);
    nfaQues->start_state = start_state;
    return nfaQues;
}

/* printNFA: print NFA
 * @param nfa: NFA to print
 */
void printNFA(const NFA &nfa) {
    std::cout << "NFA States:\n";
    std::cout << "NFA Start State:" << nfa.start_state->id << '\n';
    for (const auto &state : nfa.states) {
        std::cout << "State " << state->id;
        if (state->is_final)
            std::cout << " (Final)";
        std::cout << "\n";
        for (const auto &transition : state->transitions) {
            for (const auto &nextState : transition.second) {
                if (transition.first == 0) {
                    std::cout << state->id << " --- "
                              << "esp"
                              << " --> " << nextState->id << '\n';
                } else {
                    std::cout << state->id << " --- " << transition.first
                              << " --> " << nextState->id << '\n';
                }
            }
        }
    }
}