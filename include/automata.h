#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>
#include <stack>
#include <spot/twa/twagraph.hh>

#include <exception.h>

namespace lfz {
namespace automata {

typedef unsigned state_num_t;

class AutomataException : public Exception {
public:
    AutomataException(const std::string &msg) noexcept;
    AutomataException(const AutomataException &e) noexcept;
    AutomataException &operator= (const AutomataException &e) noexcept;
    ~AutomataException();

    virtual const char *what() const noexcept override;

private:
    std::string msg_;
};

class State {
public:
    State(const spot::state *state, state_num_t state_num);
    State(State &&s);
    State &operator= (State &&s);
    ~State();

    const spot::state *state() const;
    state_num_t state_num() const;

private:
    const spot::state *state_;
    state_num_t state_num_;
};

class Iterator {
public:
    Iterator(spot::twa_succ_iterator *iter,
             spot::const_twa_graph_ptr graph,
             const std::unordered_map<std::string, int> *bdd_map,
             const std::unordered_map<int, std::string> *reverse_bdd_map);
    Iterator(Iterator &&i);
    Iterator &operator= (Iterator &&i);
    ~Iterator();

    bool first();
    bool next();
    bool done() const;
    State dst() const;

    bool is_true_cond() const;
    bool check_cond(const std::set<std::string> &events) const;
    void get_cond(std::vector<std::set<std::string>> &events) const;

private:
    spot::twa_succ_iterator *iter_;
    spot::const_twa_graph_ptr graph_;
    const std::unordered_map<std::string, int> *bdd_map_;
    const std::unordered_map<int, std::string> *reverse_bdd_map_;
};

struct MCState {
    MCState(int state, int distance, bool acceptance);

    int state;
    int distance;
    bool acceptance;
};


typedef std::vector<std::pair<std::vector<std::set<std::string>>, int>> transitions_t;
typedef std::vector<std::set<std::string>> events_t;
typedef std::set<int> stateSet_t;
typedef std::vector<std::stack<int>> paths_t;

class Automata {
public:
    Automata();
    /**
     * @throws AutomataException
     */
    Automata(const std::string &formula, const std::string &exclusive = "");
    Automata(const Automata &a);
    Automata &operator= (const Automata &a);
    ~Automata();

    /**
     * @throws AutomataException
     */
    void set_formula(const std::string &formula, const std::string &exclusive = "");
    bool valid() const;
    State get_init_state() const;
    Iterator get_iterator(const State &state) const;

    void model_check_events(const std::vector<std::string> &events,
                            std::vector<MCState> &states) const;
    /*
     * Return all transitions (with transition conditions) for an automata
     * state.
     *
     * Return format: [([{"cond1", "cond2", ...}, {"cond1", "cond2", ...}, ...],
     * next_state), ...] where [a, b, ...] is a vector, (a, b) is a pair, and
     * {a, b, ...} is a set.
     */
    void get_state_transitions(int state, transitions_t &transitions) const;

    /*
     * Return all paths from the current state to the accepting states.
     */
    void get_state_paths(int curState, paths_t &paths, std::vector<int> aPath) const;
    
    /*
     * Return transition events between two states.
     */
    void get_state_events(int curState, int nextState, events_t &events) const;

private:
    spot::twa_graph_ptr automata_;
    std::unordered_map<std::string, int> bdd_map_;
    std::unordered_map<int, std::string> reverse_bdd_map_;
    void get_state_set(int curState, stateSet_t &stateSet) const;
    void find_paths(std::stack<int> &sstack, paths_t &paths, std::vector<int> &isVisited, std::stack<int> &path) const;
};

} // namespace automata
} // namespace lfz
