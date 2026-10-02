// master_disciple_matching.h
// CSD3131/3130 Assignment Part 1 - Question 1
//
// Singapore Context
// ------------------
// Traditional craft masters (e.g. calligraphy, lantern-making, Peranakan
// tile art, dough-figurine making) are struggling to find committed
// apprentices before these skills are lost. This system matches disciples
// (apprentices) to masters using a stable-matching algorithm derived from
// Gale-Shapley, extended to the Hospital/Residents variant: masters can
// take multiple disciples (capacity > 1), there is typically a surplus of
// disciples relative to available slots, and craft-mismatched pairs are
// treated as mutually unacceptable (incomplete preference lists variant).
//
// Design
// ------
// 1. Each master and disciple has a set of attributes (craft, personality
//    tags, hours commitment, experience level).
// 2. A compatibility score is computed for every (master, disciple) pair
//    that shares a craft; mismatched-craft pairs are excluded entirely.
// 3. Each side's preference list is generated automatically by sorting the
//    other side by score, descending.
// 4. A capacity-aware, disciple-proposing Gale-Shapley algorithm runs on
//    these preference lists to produce a stable assignment.
//
// Stability guarantee (per the assignment brief, generalised to many-to-one):
//   - No disciple is unmatched while some acceptable master has a free
//     slot (first type of instability).
//   - No disciple assigned to master m would rather be with master m',
//     where m' also prefers that disciple over one of its current
//     disciples (second type of instability).

#ifndef MASTER_DISCIPLE_MATCHING_H
#define MASTER_DISCIPLE_MATCHING_H

#include <string>
#include <vector>
#include <map>

struct Master {
    std::string name;
    std::string craft_type;
    std::vector<std::string> personality_tags;
    int capacity;                 // number of disciple slots available
    double min_hours_required;    // minimum daily hours master expects
    int min_experience_required;  // 0 = none, 1 = some, 2 = experienced
};

struct Disciple {
    std::string name;
    std::string craft_interest;
    double hours_per_day;
    int experience_level;         // 0 = none, 1 = some, 2 = experienced
    std::vector<std::string> personality_tags;
};

// Compatibility score between a master and a disciple. Higher = more
// preferred. Craft mismatch is NOT scored here -- it is filtered out
// entirely by isAcceptable() before scoring happens.
double computeScore(const Master& m, const Disciple& d);

// Hard filter applied before scoring: a pairing outside the same craft is
// never acceptable to either side.
bool isAcceptable(const Master& m, const Disciple& d);

// Builds ranked preference lists for both sides (best first), omitting
// unacceptable partners.
//   masterPrefs[master.name]   -> vector of disciple names, best first
//   disciplePrefs[disciple.name] -> vector of master names, best first
void buildPreferenceLists(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    std::map<std::string, std::vector<std::string>>& masterPrefs,
    std::map<std::string, std::vector<std::string>>& disciplePrefs
);

// Capacity-aware, disciple-proposing Gale-Shapley (Hospital/Residents
// problem). Fills 'assignment' (master name -> vector of disciple names)
// and 'unmatched' (disciple names with no master).
void stableMatch(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    const std::map<std::string, std::vector<std::string>>& masterPrefs,
    const std::map<std::string, std::vector<std::string>>& disciplePrefs,
    std::map<std::string, std::vector<std::string>>& assignment,
    std::vector<std::string>& unmatched
);

// Verifies the assignment has no instability of either type. Returns true
// if stable; if false, 'reason' is filled with a human-readable
// explanation of the violation found.
bool isStable(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    const std::map<std::string, std::vector<std::string>>& masterPrefs,
    const std::map<std::string, std::vector<std::string>>& disciplePrefs,
    const std::map<std::string, std::vector<std::string>>& assignment,
    std::string& reason
);

#endif // MASTER_DISCIPLE_MATCHING_H
