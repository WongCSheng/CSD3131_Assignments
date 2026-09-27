// master_disciple_matching.cpp
#include "master_disciple_matching.h"

#include <algorithm>
#include <limits>
#include <set>

namespace {

int countOverlap(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    std::set<std::string> setA(a.begin(), a.end());
    int count = 0;
    for (const auto& tag : b) {
        if (setA.count(tag)) ++count;
    }
    return count;
}

// Index of a name within a preference-list vector, or -1 if not present.
int rankOf(const std::vector<std::string>& prefs, const std::string& name) {
    for (size_t i = 0; i < prefs.size(); ++i) {
        if (prefs[i] == name) return static_cast<int>(i);
    }
    return -1; // not in the list (unacceptable / not proposed to)
}

} // namespace

bool isAcceptable(const Master& m, const Disciple& d) {
    // Craft mismatch: nothing to teach, mutually unacceptable.
    return m.craft_type == d.craft_interest;
}

double computeScore(const Master& m, const Disciple& d) {
    double score = 0.0;

    // Craft match dominates (guaranteed true by the time this is called,
    // since isAcceptable() has already filtered mismatches out -- kept
    // here so the function is still meaningful if called standalone).
    if (m.craft_type == d.craft_interest) {
        score += 100.0;
    } else {
        score -= 50.0;
    }

    // Personality compatibility: overlap in tags (e.g. "patient",
    // "detail-oriented", "night-owl").
    int overlap = countOverlap(m.personality_tags, d.personality_tags);
    score += overlap * 10.0;

    // Hours commitment: disciple must meet the master's minimum; modest
    // excess is rewarded (capped), shortfall is penalised proportionally.
    double hourDiff = d.hours_per_day - m.min_hours_required;
    if (hourDiff >= 0.0) {
        score += 20.0 + std::min(hourDiff, 4.0) * 2.0;
    } else {
        score -= (-hourDiff) * 10.0;
    }

    // Experience fit: meeting the requirement exactly is ideal; being
    // under-qualified is penalised more than being over-qualified.
    int expDiff = d.experience_level - m.min_experience_required;
    if (expDiff >= 0) {
        score += 15.0 - expDiff * 3.0;
    } else {
        score -= (-expDiff) * 8.0;
    }

    return score;
}

void buildPreferenceLists(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    std::map<std::string, std::vector<std::string>>& masterPrefs,
    std::map<std::string, std::vector<std::string>>& disciplePrefs
) {
    masterPrefs.clear();
    disciplePrefs.clear();

    for (const auto& m : masters) {
        std::vector<std::pair<double, std::string>> scored;
        for (const auto& d : disciples) {
            if (isAcceptable(m, d)) {
                scored.emplace_back(computeScore(m, d), d.name);
            }
        }
        std::sort(scored.begin(), scored.end(),
                  [](const auto& a, const auto& b) {
                      if (a.first != b.first) return a.first > b.first; // highest score first
                      return a.second < b.second;                       // tie-break by name
                  });
        std::vector<std::string> names;
        for (auto& p : scored) names.push_back(p.second);
        masterPrefs[m.name] = names;
    }

    for (const auto& d : disciples) {
        std::vector<std::pair<double, std::string>> scored;
        for (const auto& m : masters) {
            if (isAcceptable(m, d)) {
                scored.emplace_back(computeScore(m, d), m.name);
            }
        }
        std::sort(scored.begin(), scored.end(),
                  [](const auto& a, const auto& b) {
                      if (a.first != b.first) return a.first > b.first;
                      return a.second < b.second;
                  });
        std::vector<std::string> names;
        for (auto& p : scored) names.push_back(p.second);
        disciplePrefs[d.name] = names;
    }
}

void stableMatch(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    const std::map<std::string, std::vector<std::string>>& masterPrefs,
    const std::map<std::string, std::vector<std::string>>& disciplePrefs,
    std::map<std::string, std::vector<std::string>>& assignment,
    std::vector<std::string>& unmatched
) {
    std::map<std::string, int> capacity;
    for (const auto& m : masters) capacity[m.name] = m.capacity;

    // masterRank[masterName][discipleName] = position in master's pref list
    std::map<std::string, std::map<std::string, int>> masterRank;
    for (const auto& m : masters) {
        const auto& prefs = masterPrefs.at(m.name);
        for (size_t i = 0; i < prefs.size(); ++i) {
            masterRank[m.name][prefs[i]] = static_cast<int>(i);
        }
    }

    assignment.clear();
    for (const auto& m : masters) assignment[m.name] = {};

    std::map<std::string, int> nextProposalIndex;
    for (const auto& d : disciples) nextProposalIndex[d.name] = 0;

    std::vector<std::string> freeDisciples;
    for (const auto& d : disciples) freeDisciples.push_back(d.name);

    // Standard queue-based Gale-Shapley: process free disciples until none
    // remain (either matched, or exhausted their preference list).
    size_t head = 0;
    while (head < freeDisciples.size()) {
        std::string dName = freeDisciples[head++];
        const auto& prefs = disciplePrefs.at(dName);
        int idx = nextProposalIndex[dName];

        if (idx >= static_cast<int>(prefs.size())) {
            continue; // proposed to everyone acceptable, rejected by all
        }

        std::string mName = prefs[idx];
        nextProposalIndex[dName] = idx + 1;

        auto& current = assignment[mName];
        if (static_cast<int>(current.size()) < capacity[mName]) {
            current.push_back(dName);
        } else {
            // Master is full: compare proposer against the master's
            // least-preferred currently-held disciple.
            const auto& ranks = masterRank[mName];
            std::string worstHeld = current[0];
            int worstRank = ranks.count(worstHeld) ? ranks.at(worstHeld) : std::numeric_limits<int>::max();
            for (const auto& name : current) {
                int r = ranks.count(name) ? ranks.at(name) : std::numeric_limits<int>::max();
                if (r > worstRank) { worstRank = r; worstHeld = name; }
            }
            int proposerRank = ranks.count(dName) ? ranks.at(dName) : std::numeric_limits<int>::max();
            if (proposerRank < worstRank) {
                current.erase(std::remove(current.begin(), current.end(), worstHeld), current.end());
                current.push_back(dName);
                freeDisciples.push_back(worstHeld); // bumped disciple tries again
            } else {
                freeDisciples.push_back(dName);     // rejected, try next master
            }
        }
    }

    std::set<std::string> matchedNames;
    for (const auto& kv : assignment) {
        for (const auto& name : kv.second) matchedNames.insert(name);
    }
    unmatched.clear();
    for (const auto& d : disciples) {
        if (!matchedNames.count(d.name)) unmatched.push_back(d.name);
    }
}

bool isStable(
    const std::vector<Master>& masters,
    const std::vector<Disciple>& disciples,
    const std::map<std::string, std::vector<std::string>>& masterPrefs,
    const std::map<std::string, std::vector<std::string>>& disciplePrefs,
    const std::map<std::string, std::vector<std::string>>& assignment,
    std::string& reason
) {
    std::map<std::string, int> capacityOf;
    for (const auto& m : masters) capacityOf[m.name] = m.capacity;

    std::map<std::string, std::map<std::string, int>> masterRank;
    for (const auto& m : masters) {
        const auto& prefs = masterPrefs.at(m.name);
        for (size_t i = 0; i < prefs.size(); ++i) masterRank[m.name][prefs[i]] = static_cast<int>(i);
    }

    std::map<std::string, std::string> currentMasterOf; // disciple -> master
    for (const auto& kv : assignment) {
        for (const auto& dName : kv.second) currentMasterOf[dName] = kv.first;
    }

    for (const auto& d : disciples) {
        const auto& dPrefs = disciplePrefs.at(d.name);
        int currentRank = std::numeric_limits<int>::max();
        std::string currentMaster;
        auto it = currentMasterOf.find(d.name);
        if (it != currentMasterOf.end()) {
            currentMaster = it->second;
            currentRank = rankOf(dPrefs, currentMaster);
        }

        for (const auto& mName : dPrefs) {
            int mRankForD = rankOf(dPrefs, mName);
            if (mRankForD >= currentRank) break; // no better master left to check

            const auto& held = assignment.at(mName);
            bool freeSlot = static_cast<int>(held.size()) < capacityOf[mName];
            if (freeSlot) {
                reason = "Instability: disciple " + d.name + " prefers " + mName +
                          " (which has a free slot) over current master " +
                          (currentMaster.empty() ? std::string("(none)") : currentMaster) + ".";
                return false;
            }

            const auto& ranks = masterRank[mName];
            std::string worstHeld = held[0];
            int worstRank = ranks.count(worstHeld) ? ranks.at(worstHeld) : std::numeric_limits<int>::max();
            for (const auto& name : held) {
                int r = ranks.count(name) ? ranks.at(name) : std::numeric_limits<int>::max();
                if (r > worstRank) { worstRank = r; worstHeld = name; }
            }
            int dRankForM = ranks.count(d.name) ? ranks.at(d.name) : std::numeric_limits<int>::max();
            if (dRankForM < worstRank) {
                reason = "Instability: disciple " + d.name + " prefers " + mName + ", and " +
                          mName + " prefers " + d.name + " over its current disciple " + worstHeld + ".";
                return false;
            }
        }
    }

    reason.clear();
    return true;
}
