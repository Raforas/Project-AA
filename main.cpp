// Importing libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>

#include <numeric> // for accumulate
#include <limits>  // for input validation
#include <vector>

// Defining a struct to store the playlist data
using namespace std;

struct Playlist {
    string song_id;
    string song_title;
    string artist;
    string album;
    string genre;
    string release_date; // Use strings for simplicity in parsing dates
    int duration;
    int popularity;
    int stream;
    string language;
    string explicit_content; // "Yes" or "No"
};

// Function to read the CSV file and store the data into a vector of Playlist structures
vector<Playlist> readCsv(const string& filename) {
    vector<Playlist> playlists;
    ifstream file(filename); // Open CSV file
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return playlists;
    }

    string line;
    // Reading the header (first line)
    getline(file, line);

    // Reading each row from the csv
    while (getline(file, line)) {
        stringstream ss(line);
        Playlist playlist;
        string value;

        getline(ss, playlist.song_id, ',');
        getline(ss, playlist.song_title, ',');
        getline(ss, playlist.artist, ',');
        getline(ss, playlist.album, ',');
        getline(ss, playlist.genre, ',');
        getline(ss, playlist.release_date, ',');
        getline(ss, value, ','); // Duration
        playlist.duration = stoi(value); // Convert string to int
        getline(ss, value, ','); // Popularity
        playlist.popularity = stoi(value); // Convert string to int
        getline(ss, value, ','); // Stream
        playlist.stream = stoi(value); // Convert string to int
        getline(ss, playlist.language, ',');
        getline(ss, playlist.explicit_content, ',');

        // Append the data to the vector
        playlists.push_back(playlist);
    }

    file.close();
    return playlists;
}

// Function prototypes
void sortMenu(vector<Playlist>& playlists);
void additionalFunctionsMenu(const vector<Playlist>& playlists);

string truncateText(const string& text, size_t maxLength = 12) {
    if (text.length() > maxLength) {
        return text.substr(0, maxLength) + "..";
    }
    return text;
}


void displayMenu() {
    cout << "\n========== Spotify Playlist Manager ==========\n";
    cout << "1. Display Playlists (first 100 entries)\n";
    cout << "2. Sort Playlists\n";
    cout << "3. Search Playlist\n"; // New option for Search
    cout << "4. Additional Functions\n";
    cout << "5. Exit\n";
    cout << "Enter your choice: ";
}

// Function to display the playlists (re-used from main code)
void displayPlaylists(const vector<Playlist>& playlists, int limit = 100) {
    cout << setw(10) << "Song ID"
        << setw(20) << "Title"
        << setw(20) << "Artist"
        << setw(20) << "Album"
        << setw(15) << "Genre"
        << setw(15) << "Release Date"
        << setw(10) << "Duration"
        << setw(15) << "Popularity"
        << setw(10) << "Stream"
        << setw(10) << "Language"
        << setw(10) << "Explicit" << endl;
    cout << string(180, '-') << endl;

    for (size_t i = 0; i < playlists.size() && i < limit; ++i) {
        const auto& p = playlists[i];
        cout << setw(10) << p.song_id
            << setw(20) << truncateText(p.song_title)
            << setw(20) << truncateText(p.artist)
            << setw(20) << truncateText(p.album)
            << setw(15) << truncateText(p.genre)
            << setw(15) << p.release_date
            << setw(10) << p.duration
            << setw(15) << p.popularity
            << setw(10) << p.stream
            << setw(10) << p.language
            << setw(10) << p.explicit_content << endl;
    }
}

// Placeholder for Search Menu
// Man ni aku letak view aku je ni, nanti kalau kau nk ubah pape tukar je kat sini <<<< Rafeeq
void searchMenu(const vector<Playlist>& playlists) {
    while (true) {
        cout << "\n========== Search Menu ==========\n";
        cout << "1. Search by Song Title\n";
        cout << "2. Search by Artist\n";
        cout << "3. Search by Genre\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
        case 2:
        case 3:
            cout << "\nThis feature is a placeholder for later functionality.\n";
            break;
        case 4:
            return; // Exit Search Menu
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}


void sortMenu(vector<Playlist>& playlists) {
    while (true) {
        cout << "\n========== Sorting Menu ==========\n";

        // Step 1: Choose Column
        cout << "Choose Column to Sort By:\n";
        cout << "1. Song ID\n";
        cout << "2. Title\n";
        cout << "3. Artist\n";
        cout << "4. Album\n";
        cout << "5. Genre\n";
        cout << "6. Release Date\n";
        cout << "7. Duration\n";
        cout << "8. Popularity\n";
        cout << "9. Streams\n";
        cout << "10. Language\n";
        cout << "11. Back to Main Menu\n"; // Back option
        cout << "Enter your choice: ";
        int columnChoice;
        cin >> columnChoice;

        if (columnChoice == 11) break; // Exit sorting menu if "Back" is chosen.

        // Validate columnChoice
        if (columnChoice < 1 || columnChoice > 10){
            cout << "Invalid column choice. Please try again.\n";
            continue;
        }

        // Step 2: Choose Sorting Algorithm
        cout << "\nChoose Sorting Algorithm:\n";
        cout << "1. Merge Sort\n";
        cout << "2. Quick Sort\n";
        cout << "Enter your choice: ";
        int algoChoice;
        cin >> algoChoice;

        if (algoChoice < 1 || algoChoice > 2){
            cout << "Invalid algorithm choice. Please try again.\n";
            continue;
        }

        // Step 3: Choose Sorting Order
        cout << "\nChoose Sorting Order:\n";
        cout << "1. Ascending\n";
        cout << "2. Descending\n";
        cout << "Enter your choice: ";
        int orderChoice;
        cin >> orderChoice;

        bool ascending = (orderChoice == 1);
        if (orderChoice < 1 || orderChoice > 2){
            cout << "Invalid sort order choice. Please try again.\n";
            continue;
        }

        // Step 4: Define comparator
        auto comparator = [&](const Playlist& a, const Playlist& b) {
            switch (columnChoice){
                case 1: return ascending ? a.song_id < b.song_id : a.song_id > b.song_id;
                case 2: return ascending ? a.song_title < b.song_title : a.song_title > b.song_title;
                case 3: return ascending ? a.artist < b.artist : a.artist > b.artist;
                case 4: return ascending ? a.album < b.album : a.album > b.album;
                case 5: return ascending ? a.genre < b.genre : a.genre > b.genre;
                case 6: return ascending ? a.release_date < b.release_date : a.release_date > b.release_date;
                case 7: return ascending ? a.duration < b.duration : a.duration > b.duration;
                case 8: return ascending ? a.popularity < b.popularity : a.popularity > b.popularity;
                case 9: return ascending ? a.stream < b.stream : a.stream > b.stream;
                case 10: return ascending ? a.language < b.language : a.language > b.language;
                default: return true;
            }
        };

        // Step 5: Execute sorting algorithm
        if (algoChoice == 1){
            // Merge Sort
        } else if (algoChoice == 2) {
            // Quick Sort
        }

        // Step 6: Display sorted results
        cout << "\nPlaylists sorted successfully:\n";
        displayPlaylists(playlists);
    }
}

void additionalFunctionsMenu(const vector<Playlist>& playlists){
    while (true) {
        cout << "\n========== Additional Functions ==========\n";
        cout << "1. Additional Function Placeholder 1\n";
        cout << "2. Additional Function Placeholder 2\n";
        cout << "3. Additional Function Placeholder 3\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
        case 2:
        case 3:
            cout << "\nThis feature is a placeholder for later functionality.\n";
            break;
        case 4:
            return; // Exit Additional Functions Menu
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}

int main() {
    string filename =
        "../spotify_songs_dataset_unsorted.csv";
    // Path to your playlist dataset
    vector<Playlist> playlists = readCsv(filename);

    if (playlists.empty()) {
        cout << "Error: No playlist data loaded. Exiting program.\n";
        return 1;
    }

    while (true){
        displayMenu();
        int choice;
        cin >> choice;

        if (!cin){
            // Check for invalid input
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice){
        case 1:
            displayPlaylists(playlists);
            break;
        case 2:
            sortMenu(playlists);
            break;
        case 3:
            searchMenu(playlists); // New Search Menu
            break;
        case 4:
            additionalFunctionsMenu(playlists);
            break;
        case 5:
            cout << "Exiting program. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}
