// test_matching.cpp
// CSD3131/3130 Assignment Part 1 - Question 1 - Test Cases
//
// Compile:  g++ -std=c++17 -O2 -o test_matching master_disciple_matching.cpp test_matching.cpp
// Run:      ./test_matching

#include "master_disciple_matching.h"

#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>

using namespace std;

static void printAssignment(const map<string, vector<string>>& assignment,
                              const vector<string>& unmatched) {
    cout << "Assignment:\n";
    for (const auto& kv : assignment) {
        cout << "  " << kv.first << ": [";
        for (size_t i = 0; i < kv.second.size(); ++i) {
            cout << kv.second[i] << (i + 1 < kv.second.size() ? ", " : "");
        }
        cout << "]\n";
    }
    cout << "Unmatched disciples: [";
    for (size_t i = 0; i < unmatched.size(); ++i) {
        cout << unmatched[i] << (i + 1 < unmatched.size() ? ", " : "");
    }
    cout << "]\n";
}

// Runs one scenario end-to-end and checks stability.
static void runCase(const string& title,
                     const vector<Master>& masters,
                     const vector<Disciple>& disciples,
                     bool expectStable,
                     const string& note,
                     map<string, vector<string>>* outAssignment = nullptr,
                     vector<string>* outUnmatched = nullptr) {
    cout << "\n======================================================================\n";
    cout << title << "\n";
    cout << "======================================================================\n";
    if (!note.empty()) cout << note << "\n";

    map<string, vector<string>> masterPrefs, disciplePrefs;
    buildPreferenceLists(masters, disciples, masterPrefs, disciplePrefs);

    map<string, vector<string>> assignment;
    vector<string> unmatched;
    stableMatch(masters, disciples, masterPrefs, disciplePrefs, assignment, unmatched);

    printAssignment(assignment, unmatched);

    string reason;
    bool stable = isStable(masters, disciples, masterPrefs, disciplePrefs, assignment, reason);
    cout << "Stable? " << (stable ? "true" : "false");
    if (!reason.empty()) cout << "  (" << reason << ")";
    cout << "\n";

    assert(stable == expectStable && "Stability result did not match expectation");
    cout << "PASS\n";

    if (outAssignment) *outAssignment = assignment;
    if (outUnmatched) *outUnmatched = unmatched;
}

// ---------------------------------------------------------------------
// Case 1: Balanced, no ties, straightforward preferences
// ---------------------------------------------------------------------
void case1() {
    vector<Master> masters = {
        {"Auntie Tan", "Calligraphy", {"patient", "traditional"}, 1, 2.0, 0},
        {"Uncle Wong", "Lantern-Making", {"creative", "night-owl"}, 1, 3.0, 1},
    };
    vector<Disciple> disciples = {
        {"Alice", "Calligraphy", 3.0, 0, {"patient"}},
        {"Ben", "Lantern-Making", 4.0, 1, {"creative", "night-owl"}},
    };
    runCase("Case 1: Balanced, clean match", masters, disciples, true, "");
}

// ---------------------------------------------------------------------
// Case 2: Surplus disciples (more disciples than slots) -- required by brief
// ---------------------------------------------------------------------
void case2() {
    vector<Master> masters = {
        {"Auntie Tan", "Calligraphy", {"patient"}, 1, 2.0, 0},
    };
    vector<Disciple> disciples = {
        {"Alice", "Calligraphy", 3.0, 0, {"patient"}},
        {"Cara", "Calligraphy", 2.5, 0, {"patient"}},
        {"Dylan", "Calligraphy", 2.0, 0, {}},
    };
    vector<string> unmatched;
    runCase("Case 2: Surplus disciples, one slot", masters, disciples, true,
             "Only 1 slot for 3 interested disciples -- two must be unmatched.",
             nullptr, &unmatched);
    assert(unmatched.size() == 2);
}

// ---------------------------------------------------------------------
// Case 3: All disciples equally interested in the same craft, two masters
// ---------------------------------------------------------------------
void case3() {
    vector<Master> masters = {
        {"Master Lim", "Puppetry", {"patient"}, 2, 1.0, 0},
        {"Master Koh", "Puppetry", {"patient"}, 2, 1.0, 0},
    };
    vector<Disciple> disciples = {
        {"D0", "Puppetry", 2.0, 0, {"patient"}},
        {"D1", "Puppetry", 2.0, 0, {"patient"}},
        {"D2", "Puppetry", 2.0, 0, {"patient"}},
        {"D3", "Puppetry", 2.0, 0, {"patient"}},
    };
    runCase("Case 3: Rejection-chain stress test (identical disciples, two masters)",
             masters, disciples, true, "");
}

// ---------------------------------------------------------------------
// Case 4: Reversed preferences driven by personality tags
// ---------------------------------------------------------------------
void case4() {
    vector<Master> masters = {
        {"M1", "Pottery", {"calm"}, 1, 1.0, 0},
        {"M2", "Pottery", {"energetic"}, 1, 1.0, 0},
    };
    vector<Disciple> disciples = {
        {"D1", "Pottery", 5.0, 2, {"energetic"}}, // scores higher with M2
        {"D2", "Pottery", 5.0, 2, {"calm"}},      // scores higher with M1
    };
    runCase("Case 4: Personality-driven reversed preferences", masters, disciples, true, "");
}

// ---------------------------------------------------------------------
// Case 5: Minimal 1-to-1 case
// ---------------------------------------------------------------------
void case5() {
    vector<Master> masters = { {"M1", "Origami", {}, 1, 1.0, 0} };
    vector<Disciple> disciples = { {"D1", "Origami", 1.0, 0, {}} };
    runCase("Case 5: Minimal 1-to-1 case", masters, disciples, true, "");
}

// ---------------------------------------------------------------------
// Case 6: Craft mismatch -- disciple should end up unmatched
// ---------------------------------------------------------------------
void case6() {
    vector<Master> masters = { {"M1", "Calligraphy", {}, 1, 1.0, 0} };
    vector<Disciple> disciples = { {"D1", "Pottery", 5.0, 2, {}} }; // mismatch
    vector<string> unmatched;
    runCase("Case 6: Craft mismatch -- disciple should end up unmatched",
             masters, disciples, true,
             "Disciple's only interest (Pottery) doesn't exist among masters.",
             nullptr, &unmatched);
    assert(unmatched.size() == 1 && unmatched[0] == "D1");
}

// ---------------------------------------------------------------------
// Case 7: Master with capacity > 1 (many-to-one, core extension)
// ---------------------------------------------------------------------
void case7() {
    vector<Master> masters = { {"M1", "Weaving", {"patient"}, 3, 1.0, 0} };
    vector<Disciple> disciples = {
        {"D1", "Weaving", 2.0, 1, {"patient"}},
        {"D2", "Weaving", 2.0, 0, {"patient"}},
        {"D3", "Weaving", 1.5, 0, {}},
        {"D4", "Weaving", 1.0, 0, {}}, // weakest -- should be bumped
    };
    map<string, vector<string>> assignment;
    vector<string> unmatched;
    runCase("Case 7: Capacity-3 master with 4 interested disciples", masters, disciples, true,
             "Master should keep the 3 best-scoring disciples and reject the weakest.",
             &assignment, &unmatched);
    assert(assignment["M1"].size() == 3);
    assert(find(unmatched.begin(), unmatched.end(), "D4") != unmatched.end());
}

// ---------------------------------------------------------------------
// Case 8: Identical scores (tie) between two disciples for one master
// ---------------------------------------------------------------------
void case8() {
    vector<Master> masters = { {"M1", "Batik", {"creative"}, 1, 2.0, 1} };
    vector<Disciple> disciples = {
        {"D1", "Batik", 2.0, 1, {"creative"}},
        {"D2", "Batik", 2.0, 1, {"creative"}}, // identical attributes
    };
    vector<string> unmatched;
    runCase("Case 8: Tied scores, deterministic tie-break by name", masters, disciples, true,
             "", nullptr, &unmatched);
    assert(unmatched.size() == 1);
}

// ---------------------------------------------------------------------
// Case 9: Larger randomised instance (for timing / empirical Big-O check)
// ---------------------------------------------------------------------
void case9() {
    mt19937 rng(42);
    vector<string> crafts = {"Calligraphy", "Pottery", "Weaving", "Puppetry", "Lantern-Making"};
    vector<string> tagPool = {"patient", "creative", "calm", "energetic", "traditional", "night-owl"};

    uniform_int_distribution<int> craftDist(0, (int)crafts.size() - 1);
    uniform_int_distribution<int> capDist(1, 3);
    uniform_real_distribution<double> hourDist(0.5, 5.0);
    uniform_int_distribution<int> expDist(0, 2);

    vector<Master> masters;
    for (int i = 0; i < 10; ++i) {
        masters.push_back({
            "M" + to_string(i), crafts[craftDist(rng)],
            {tagPool[i % tagPool.size()], tagPool[(i + 1) % tagPool.size()]},
            capDist(rng), hourDist(rng), expDist(rng)
        });
    }
    vector<Disciple> disciples;
    for (int i = 0; i < 20; ++i) {
        disciples.push_back({
            "D" + to_string(i), crafts[craftDist(rng)], hourDist(rng), expDist(rng),
            {tagPool[i % tagPool.size()], tagPool[(i + 2) % tagPool.size()]}
        });
    }
    runCase("Case 9: Larger randomised instance (10 masters, 20 disciples)",
             masters, disciples, true,
             "Surplus of disciples expected; used to sanity-check timing at scale.");
}

// ---------------------------------------------------------------------
// Case 10: Stability checker correctly flags a DELIBERATELY broken assignment
// ---------------------------------------------------------------------
void case10() {
    vector<Master> masters = {
        {"M1", "Pottery", {}, 1, 1.0, 0},
        {"M2", "Pottery", {}, 1, 1.0, 0},
    };
    vector<Disciple> disciples = {
        {"D1", "Pottery", 5.0, 2, {}}, // strong candidate
        {"D2", "Pottery", 1.0, 0, {}}, // weak candidate
    };

    map<string, vector<string>> masterPrefs, disciplePrefs;
    buildPreferenceLists(masters, disciples, masterPrefs, disciplePrefs);

    // Manually construct a BAD assignment: give the weak disciple to M1
    // while the strong disciple D1 -- who prefers M1 more, and whom M1
    // prefers more -- sits unmatched. This is the "first type of
    // instability" from the assignment brief.
    map<string, vector<string>> badAssignment = { {"M1", {"D2"}}, {"M2", {}} };

    string reason;
    bool stable = isStable(masters, disciples, masterPrefs, disciplePrefs, badAssignment, reason);

    cout << "\n======================================================================\n";
    cout << "Case 10: Stability checker on a deliberately bad assignment\n";
    cout << "======================================================================\n";
    cout << "Assignment under test: M1=[D2], M2=[]\n";
    cout << "Stable? " << (stable ? "true" : "false") << "  (" << reason << ")\n";
    assert(!stable && "Checker should have flagged this as unstable");
    cout << "PASS -- checker correctly detected the instability\n";
}

// ---------------------------------------------------------------------
// Case 11 (bonus): Master requires more hours than any disciple offers
// ---------------------------------------------------------------------
void case11() {
    vector<Master> masters = { {"M1", "Calligraphy", {}, 1, 8.0, 2} }; // unrealistic demand
    vector<Disciple> disciples = {
        {"D1", "Calligraphy", 1.0, 0, {}},
        {"D2", "Calligraphy", 1.5, 0, {}},
    };
    runCase("Case 11: Master's requirements exceed what any disciple offers",
             masters, disciples, true,
             "Still produces a (heavily-penalised-score) match -- algorithm doesn't crash "
             "on unrealistic inputs, it just ranks everyone low.");
}

int main() {
    case1();
    case2();
    case3();
    case4();
    case5();
    case6();
    case7();
    case8();
    case9();
    case10();
    case11();
    cout << "\n======================================================================\n";
    cout << "ALL TEST CASES PASSED\n";
    cout << "======================================================================\n";
    return 0;
}
