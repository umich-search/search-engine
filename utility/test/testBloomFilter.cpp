#include "BloomFilter.h"
#include <iostream>
#include <fstream>
#include "mString.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 1) {
        cout << "Usage: " << argv[0] << endl;
        return 1; 
    }
    String filename, stdInput;
    String input;
    cout << "Running bloom filter...\n";
    // cout << "Enter 'd' for default init, or 'c' for custom: ";
    // cin >> input;
    // if (input != "d") {
    //     cout << "Enter estimated number of objects: ";
    //     cin >> size;
    //     cout << "Enter false positive error rate: ";
    //     cin >> error;
    //     cout << "Enter filename to initialize bloom filter: ";
    //     cin >> filename;
    // }
    // else {
        int size = 1000000;
        double error = .0001;
        filename = "seen.txt";
    // }
    cout << "Initializing bloom filter...\n\n";

    Bloomfilter bf(size, error);
    ifstream fin(filename.cstr());
    if (!fin.is_open()) {
        cout << "Error opening: " << filename << endl;
        return 1;
    }

    uint64_t counter = 0;
    while(fin >> stdInput) {
        input = String(stdInput.cstr());
        bf.insert(input);
        counter++;
    }

    cout << "Bloom filter initialized:\n\tItems inserted: " << counter << "\n\tEstimated Total Size: " << size << "\n\tFalse positive error rate: " << strerror( errno ) << "\n\tFilename: " << filename <<
        "\nEnter a string to check if the bloom filter contains or enter 'q' to quit.\n";

    while (true) {
        cin >> stdInput;
        input = String(stdInput.cstr());
        if (input == "q") return 0;
        bf.contains(input) ? cout << "Found!\n" : cout << "Not found.\n";
    }

    return 0;
}