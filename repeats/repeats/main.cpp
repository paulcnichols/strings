#include <fstream>
#include <iostream>
#include <limits>
#include "utils.h"
#include "timer.h"
#include "inverted_index.h"

using namespace std;

struct TestEntry {
    unsigned int num_repeats;
    string filename;
};

/*
 * Test data made with
 *  https://github.com/peterwilliams97/strings/blob/master/make_repeats/make_repeats.py
 */
TestEntry entries[] = {
  { 11, string("C:\\dev\\suffix\\make_repeats\\repeats=11.txt") },
  { 12, string("C:\\dev\\suffix\\make_repeats\\repeats=12.txt") },
  { 13, string("C:\\dev\\suffix\\make_repeats\\repeats=13.txt") },
  { 14, string("C:\\dev\\suffix\\make_repeats\\repeats=14.txt") },
  { 15, string("C:\\dev\\suffix\\make_repeats\\repeats=15.txt") },
  { 16, string("C:\\dev\\suffix\\make_repeats\\repeats=16.txt") },
  { 17, string("C:\\dev\\suffix\\make_repeats\\repeats=17.txt") },
  { 18, string("C:\\dev\\suffix\\make_repeats\\repeats=18.txt") },
  { 19, string("C:\\dev\\suffix\\make_repeats\\repeats=19.txt") },
  { 20, string("C:\\dev\\suffix\\make_repeats\\repeats=20.txt") }
};
const int NUM_TEST_FILES = NUMELEMS(entries);

static vector<string> 
get_filenames() {
    list<string> filenames;
    for (int i = 0; i < NUM_TEST_FILES; i++) {
        filenames.push_back(entries[i].filename);
    }
    return vector<string>(filenames.begin(), filenames.end());
}

static
double 
test_inverted_index(const vector<string> filenames) {
   
    double time0 = get_elapsed_time();

    InvertedIndex *inverted_index = create_inverted_index(filenames);
    cout << "========================================================" << endl;
    
    show_inverted_index("initial", inverted_index);
    cout << "========================================================" << endl;
    
    vector<string> repeats = get_all_repeats(inverted_index);
    cout << "========================================================" << endl;
    
    cout << "Found " << repeats.size() << " repeated strings";
    if (repeats.size() > 0) {
        cout << " of length " << repeats.front().size();
    }
    cout << endl;
    print_vector("Repeated strings", repeats);

    delete_inverted_index(inverted_index);

    double time1 = get_elapsed_time();
    double duration = time1 - time0;
    cout << "duration = " << duration << endl;
    return duration;

}

void 
test() {
    vector<string> filenames = get_filenames();
    test_inverted_index(filenames);
}

void 
show_stats(list<double> durations) {
    vector<double> d = vector<double>(durations.begin(), durations.end()); 
    double min_d = numeric_limits<double>::max();
    double max_d = numeric_limits<double>::min();
    double total = 0.0;
    for (vector<double>::iterator it = d.begin(); it != d.end(); it++) {
        min_d = min(min_d, *it);
        max_d = max(max_d, *it);
        total += *it;
    }
    unsigned int n = d.size();
    double ave = total/(double)n;
    double med = d[n/2]; 
    cout << "min="<< min_d << ", max="<< max_d << ", ave=" << ave << ", med=" << med << endl; 
}
void 
multi_test(int n) {
    vector<string> filenames = get_filenames();
    list<double> durations;
    for (int i = 0; i < n; i++) {
        cout << "========================== test " << i << " of " << n << " ==============================" << endl;
        durations.push_back(test_inverted_index(filenames));
        show_stats(durations);
    }
   
}

int main() {
    //test();
    multi_test(100);
    return 0;
}

