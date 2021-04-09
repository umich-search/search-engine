#include "BloomFilter.h"
#include "mString.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    cout << "Running Index tests" << endl;
    if (argc != 1) {
        cout << "Usage: " << argv[0] << endl;
        return 1; 
    }

    String size, error, filename, input;
    cout << "Running bloom filter...\n";
    cout << "Enter 'd' for default init, or 'c' for custom: ";
    cin >> input;
    if (input != "d") {
        cout << "Enter estimated number of objects: ";
        cin >> size;
        cout << "Enter false positive error rate: ";
        cin >> error;
        cout << "Enter filename to initialize bloom filter: ";
        cin >> filename;
    }
    else {
        size = "1000000";
        error = ".0001";
        filename = "seen.txt";
    }
    cout << "Initializing bloom filter...\n\n";

    Bloomfilter bf(stoi(size.cstr( )), stod(error.cstr( )));
    ifstream fin(filename.cstr( ));
    if (!fin.is_open()) {
        cout << "Error opening: " << filename << endl;
        return 1;
    }

    uint64_t counter = 0;
    while(fin >> input) {
        bf.insert(input);
        counter++;
    }

    cout << "Bloom filter initialized:\n\tItems inserted: " << counter << "\n\tEstimated Total Size: " << size << "\n\tFalse positive error rate: " << strerror( errno ) << "\n\tFilename: " << filename << "\n\tBloom filter size: " << bf.size( ) << " bytes" << 
        "\nEnter a string to check if the bloom filter contains or enter 'q' to quit.\n";

    while (true) {
        cin >> input;
        if (input == "q") return 0;
        bf.contains(input) ? cout << "Found!\n" : cout << "Not found.\n";
    }

    return 0;
}