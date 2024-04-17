#include "dfa.h"

DFAState::DFAState(std::set<std::shared_ptr<NFAState>> nfa_states)
    : nfa_states(std::move(nfa_states)), id(fresh()), is_final(false) {}

void DFAState::printDFAState() const {
    if (is_final)
        std::cout << "Final Status: \\" << final_status << "\\\n";
    std::cout << "State " << (is_final ? "(Final)" : "") << id << " {";
    bool first = true;
    for (const auto &nfaState : nfa_states) {
        if (!first)
            std::cout << ", ";
        first = false;
        std::cout << nfaState->id;
    }
    std::cout << "}\n";
}

void DFAState::printTransitions() const {
    for (const auto &transition : transitions) {
        std::cout << "  Input " << transition.first << " ";
        if (transition.second != nullptr)
            transition.second->printDFAState();
        else
            std::cout << "nullptr\n";
    }
}

std::set<std::shared_ptr<NFAState>>
epsilonClosure(const std::set<std::shared_ptr<NFAState>> &states) {
    std::set<std::shared_ptr<NFAState>> closure = states;
    std::stack<std::shared_ptr<NFAState>> stack;

    for (const auto &state : states) {
        if (state != nullptr)
            stack.push(state);
    }

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        // Find epsilon transitions
        const auto &transitions = current->transitions.find(0);
        if (transitions != current->transitions.end()) {
            for (const auto &nextState : transitions->second) {
                if (nextState != nullptr &&
                    closure.find(nextState) == closure.end()) {
                    closure.insert(nextState);
                    stack.push(nextState);
                }
            }
            closure.insert(current);
        }
    }

    return closure;
}

std::set<std::shared_ptr<NFAState>>
move(const std::set<std::shared_ptr<NFAState>> &states, char inputSymbol) {
    std::set<std::shared_ptr<NFAState>> result;

    for (const auto &state : states) {
        const auto &transitions = state->transitions.find(inputSymbol);
        if (transitions != state->transitions.end()) {
            for (const auto &nextState : transitions->second) {
                if (nextState != nullptr)
                    result.insert(nextState);
            }
        }
    }

    return result;
}

std::shared_ptr<DFAState>
createDFAState(const std::set<std::shared_ptr<NFAState>> &nfaStates) {
    return std::make_shared<DFAState>(nfaStates);
}

std::shared_ptr<DFA> convertToDFA(const std::shared_ptr<NFA> &nfa) {
    flush();
    // Subset Construction Algorithm
    auto dfa = std::make_shared<DFA>();
    std::map<std::set<std::shared_ptr<NFAState>>, std::shared_ptr<DFAState>>
        dfaStateMap;

    auto initialState = epsilonClosure({nfa->start_state});
    dfa->start_state = createDFAState(initialState);
    dfaStateMap[initialState] = dfa->start_state;

    std::stack<std::shared_ptr<DFAState>> stack;
    stack.push(dfa->start_state);

    while (!stack.empty()) {
        auto dfaState = stack.top();
        stack.pop();

        // set final state for new state
        bool isFinalState = false;
        std::string finalStatus;
        for (const auto &nfaState : dfaState->nfa_states) {
            if (nfaState->is_final) {
                isFinalState = true;
                if (finalStatus.empty())
                    finalStatus = nfaState->final_status;
                // keyword, symbol, num, id
                if (nfaState->final_status != "id")
                    finalStatus = nfaState->final_status;
            }
        }
        dfaState->is_final = isFinalState;
        dfaState->final_status = finalStatus;

        for (auto inputSymbol : nfa->symbols) {
            std::set<std::shared_ptr<NFAState>> nfaTransitions =
                move(dfaState->nfa_states, inputSymbol);
            std::set<std::shared_ptr<NFAState>> epsilonTransitions =
                epsilonClosure(nfaTransitions);

            // no nfaTransitions
            if (epsilonTransitions.empty())
                continue;

            if (dfaStateMap.find(epsilonTransitions) == dfaStateMap.end()) {
                auto newState = createDFAState(epsilonTransitions);
                dfaStateMap[epsilonTransitions] = newState;
                stack.push(newState);
            }
            if (inputSymbol == '\0') {
                continue;
            }
            dfaState->transitions[inputSymbol] =
                dfaStateMap[epsilonTransitions];
        }

        for (const auto &it : dfaStateMap) {
            dfa->dfa_states.insert(it.second);
        }
    }
    dfa->symbols = nfa->symbols;
    dfa->symbols.erase(0);

    // unpoint clean, use bfs
    // create new transistion map
    flush();
    std::set<std::shared_ptr<DFAState>> visited;
    std::stack<std::shared_ptr<DFAState>> stack2;
    std::map<std::shared_ptr<DFAState>, int> newIdMap;
    stack2.push(dfa->start_state);
    visited.insert(dfa->start_state);
    while (!stack2.empty()) {
        auto currentState = stack2.top();
        stack2.pop();
        newIdMap[currentState] = fresh();
        currentState->id = newIdMap[currentState];
        for (const auto &transition : currentState->transitions) {
            if (visited.find(transition.second) == visited.end()) {
                visited.insert(transition.second);
                stack2.push(transition.second);
            }
        }
    }
    dfa->dfa_states = visited;
    for (auto &state : dfa->dfa_states) {
        for (auto &transition : state->transitions) {
            transition.second->id = newIdMap[transition.second];
        }
    }
    return dfa;
}

void DFA::printDFA() const {
    std::cout << "Start ";
    start_state->printDFAState();

    std::stack<std::shared_ptr<DFAState>> stack;
    std::set<std::shared_ptr<DFAState>> visited;

    stack.push(start_state);
    visited.insert(start_state);

    while (!stack.empty()) {
        auto currentState = stack.top();
        stack.pop();
        if (currentState == nullptr)
            continue;
        currentState->printDFAState();
        currentState->printTransitions();

        for (const auto &transition : currentState->transitions) {
            if (visited.find(transition.second) == visited.end()) {
                visited.insert(transition.second);
                stack.push(transition.second);
            }
        }
    }
}

void DFA::acceptString(const std::string &str) const {
    std::shared_ptr<DFAState> currentState = start_state;
    for (const auto symbol : str) {
        bool found = false;
        for (const auto &transition : currentState->transitions) {
            if (transition.first == symbol) {
                currentState = transition.second;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "String " << str << " is not accepted\n";
            return;
        }
    }
    if (currentState->is_final) {
        std::cout << "String " << str << " is accepted as "
                  << currentState->final_status << std::endl;
    } else {
        std::cout << "String " << str << " is not accepted\n";
    }
}

void DFA::setFinalStatus(const std::string &str) const {
    for (auto &state : dfa_states) {
        state->final_status = str;
    }
}

void DFA::printStatus() const {
    std::cout << "count of DFA states: " << dfa_states.size() << "\n";
    std::cout << "count of symbols: " << symbols.size() << "\n";
}

// Function to minimize the DFA
std::shared_ptr<DFA> DFA::minimizeDFA() {
    std::unordered_map<std::shared_ptr<DFAState>, int> state_to_partition;
    std::unordered_map<int, std::unordered_set<std::shared_ptr<DFAState>>>
        partition_map;
    state_to_partition[nullptr] = -1;
    // count the different final status
    int current_partition = 1;
    std::unordered_map<std::string, int> final_status_map;
    for (const auto &state : this->dfa_states) {
        if (state->is_final) {
            if (final_status_map.find(state->final_status) ==
                final_status_map.end()) {
                final_status_map[state->final_status] = current_partition;
                current_partition++;
            }
            state_to_partition[state] = final_status_map[state->final_status];
            partition_map[state_to_partition[state]].insert(state);
        } else {
            state_to_partition[state] = 0;
            partition_map[state_to_partition[state]].insert(state);
        }
    }
    // int current_partition = 2;
    // // Partition the states into final and non-final states
    // for (const auto &state : this->dfa_states) {
    //     state_to_partition[state] = (state->is_final) ? 1 : 0;
    //     partition_map[state_to_partition[state]].insert(state);
    // }
    bool changed = true;
    while (changed) {
        changed = false;

        for (auto &[partition, states] : partition_map) {
            if (states.size() <= 1) {
                continue;
            }
            for (auto inputSymbol : this->symbols) {
                std::unordered_map<
                    int, std::unordered_set<std::shared_ptr<DFAState>>>
                    temp_partition_map;
                for (const auto &state : states) {
                    auto next_state = state->transitions[inputSymbol];
                    // maybe nullptr, but it's ok~
                    temp_partition_map[state_to_partition[next_state]].insert(
                        state);
                }
                // if size == 1, we won't change the partition
                // tmp_map size is 1: point to themselves or other same
                // partition
                if (temp_partition_map.size() > 1) {
                    // next state of them are not in the same partition
                    // remove the old partition, and add new partition
                    changed = true;
                    partition_map.erase(partition);
                    for (const auto &[new_partition, new_states] :
                         temp_partition_map) {
                        for (const auto &new_state : new_states) {
                            state_to_partition[new_state] = current_partition;
                        }
                        partition_map[current_partition] = new_states;
                        current_partition++;
                    }
                    break;
                }
            }
            if (changed)
                break;
        }
    }

    // Create new DFA
    auto new_dfa = std::make_shared<DFA>();
    std::map<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>>
        new_state_map;
    new_dfa->symbols = this->symbols;
    // Create new states
    flush();
    for (const auto &[partition, states] : partition_map) {
        int id = fresh();
        // // Debug
        // std::cout << "Partition " << partition << " {\n";
        // for (const auto &state : states) {
        //     std::cout << "  DFAState Id: " << state->id << std::endl
        //               << "  Is Final: " << state->is_final << std::endl;
        //     if (state->is_final)
        //         std::cout << "  Final Status: " << state->final_status
        //                   << std::endl;
        //     state->printDFAState();
        //     //            state->printTransitions();
        // }
        // std::cout << "}\n" << id << std::endl;
        auto new_state = std::make_shared<DFAState>(id);
        for (const auto &state : states) {
            new_state_map[state] = new_state;
            if (state == this->start_state)
                new_dfa->start_state = new_state;
            if (state->is_final) {
                new_state->is_final = true;
                new_state->final_status = state->final_status;
            }
        }
        new_dfa->dfa_states.insert(new_state);
    }

    // after partition, we can create new transitions
    for (const auto &[partition, states] : partition_map) {
        // Create transitions
        for (const auto &state : states) {
            for (const auto &[inputSymbol, nextState] : state->transitions) {
                if (nextState == nullptr || inputSymbol == '\0') {
                    continue;
                }
                new_state_map[state]->transitions[inputSymbol] =
                    new_state_map[nextState];
            }
        }
    }
    return new_dfa;
}