#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional> // for function
#include <utility>
using namespace std;

long long globalSwapCount = 0; // Global counter to track swaps during sorting
long long globalSortTimeMs = 0; // Global variable to track sorting duration in milliseconds
long long globalSortStartTimeMs = 0; // Global start time in milliseconds
long long globalSortEndTimeMs = 0; // Global end time in milliseconds

struct Playlist {
    string song_id;
    string song_title;
    string artist;
    string album;
    string genre;
    string release_date; // Use strings for simplicity in parsing dates
    int duration{};
    int popularity{};
    int stream{};
    string language;
};

struct Node {
    Playlist data;
    Node* next;

    explicit Node(Playlist  playlist) : data(move(playlist)), next(nullptr){}
};

//Utility or global use functions
string truncateText(const string& text, const size_t maxLength = 37){// Function to truncate text
    if (text.length() > maxLength){
        return text.substr(0, maxLength) + "..";
    }
    return text;
}

string toLower(const string& str) {// Function to convert a string to lowercase
    string lowerStr = str;
    ranges::transform(lowerStr, lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool contains(const string& str, const string& query) {// Comparator to check if a string contains the query (case-insensitive)
    string lowerStr = toLower(str);
    string lowerQuery = toLower(query);
    return lowerStr.find(lowerQuery) != string::npos;
}

// Function Declarations
void displayPlaylistHeader();
void displayPlaylists(const Node*, int);
void displaySortingResults(const string&, const string&, const string&);

Node* deepCopyList(const Node* head) {
    if (!head) return nullptr;

    const auto newHead = new Node(head->data);
    const Node* current = head->next;
    Node* newCurrent = newHead;

    while (current) {
        newCurrent->next = new Node(current->data);
        newCurrent = newCurrent->next;
        current = current->next;
    }

    return newHead;
}

Node* readCsv(const string& filename, Node*& originalHead) {
    Node* head = nullptr; // Linked list head
    Node* tail = nullptr; // Pointer to track the end

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return nullptr;
    }

    string line;
    getline(file, line); // Skip header

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
        playlist.duration = stoi(value);
        getline(ss, value, ','); // Popularity
        playlist.popularity = stoi(value);
        getline(ss, value, ','); // Stream
        playlist.stream = stoi(value);
        getline(ss, playlist.language, ',');

        auto newNode = new Node(playlist);

        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    file.close();

    // Make a deep copy of the original data
    originalHead = deepCopyList(head);

    return head;
}

// Helper function to extract the numeric part of song_id
int extractSongIdNumber(const string& song_id){
    size_t pos = 0;
    while (pos < song_id.size() && !isdigit(song_id[pos])){
        ++pos;
    }
    return (pos < song_id.size()) ? stoi(song_id.substr(pos)) : 0;
}

// Merge Sort Implementation
Node* merge(Node* left, Node* right, auto comparator, long long& swapCount){
    if (!left) return right;
    if (!right) return left;

    Node* result = nullptr;

    if (comparator(left->data, right->data)){
        result = left;
        result->next = merge(left->next, right, comparator, swapCount);
    } else {
        result = right;
        result->next = merge(left, right->next, comparator, swapCount);
        swapCount++;
    }

    return result;
}

void split(Node* source, Node** front, Node** back){
    Node* slow = source;
    const Node* fast = source->next;

    while (fast){
        fast = fast->next;
        if (fast){
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

Node* mergeSort(Node* head, auto comparator, long long& swapCount){
    if (!head || !head->next)
        return head;

    Node* front = nullptr;
    Node* back = nullptr;

    split(head, &front, &back);

    front = mergeSort(front, comparator, swapCount);
    back = mergeSort(back, comparator, swapCount);

    return merge(front, back, comparator, swapCount);
}
// Merge Sort Section Ends

// Quick Sort Implementation
Node* getTail(Node* cur){
    while (cur != nullptr && cur->next != nullptr)
    {
        cur = cur->next;
    }
    return cur;
}

Node* partition(Node* head, Node* end, Node** newHead, Node** newEnd, auto comparator, long long& swapCount){
    Node* pivot = end;
    Node *prev = nullptr, *cur = head, *tail = pivot;

    *newHead = nullptr;

    while (cur != pivot){
        if (comparator(cur->data, pivot->data)){
            if (!*newHead){
                *newHead = cur;
            }
            prev = cur;
            cur = cur->next;
        } else {
            if (prev){
                prev->next = cur->next;
            }

            Node* temp = cur->next;
            cur->next = nullptr;
            tail->next = cur;
            tail = cur;

            cur = temp;

            swapCount++;
        }
    }

    if (!*newHead) {
        *newHead = pivot;
    }

    *newEnd = tail;

    return pivot;
}

Node* quickSortRecursive(Node* head, Node* end, auto comparator, long long& swapCount){
    if (!head || head == end){
        return head;
    }

    Node* newHead = nullptr;
    Node* newEnd = nullptr;

    Node* pivot = partition(head, end, &newHead, &newEnd, comparator, swapCount);

    if (newHead != pivot) {
        Node* temp = newHead;
        while (temp->next != pivot){
            temp = temp->next;
        }
        temp->next = nullptr;

        newHead = quickSortRecursive(newHead, temp, comparator, swapCount);

        temp = getTail(newHead);
        temp->next = pivot;
    }

    pivot->next = quickSortRecursive(pivot->next, newEnd, comparator, swapCount);

    return newHead;
}

Node* quickSort(Node* head, auto comparator, long long& swapCount){
    if (!head || !head->next){
        return head;
    }

    Node* end = getTail(head);

    head = quickSortRecursive(head, end, comparator, swapCount);


    return head;
}
// Quick Sort Section Ends

// MANBIL
// Binary Search Implementation
Node* getSortedHead(const Node* head, const function<bool(const Playlist&, const Playlist&)> &comparator) {
    Node* copyList = deepCopyList(head);
    long long dummySwap = 0;  // Dummy variable for swap count
    return mergeSort(copyList, comparator, dummySwap); // Use existing merge sort
}

// Function to get the middle node
Node* getMid(Node* start, Node* end) {
    Node* slow = start;
    Node* fast = start;

    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

// Binary Search Implementation
void binarySearch(Node* head, const string& query, const function<int(const Playlist&, const string&)>& comparator, int i) {
    Node* start = head;
    Node* end = nullptr;
    bool matchFound = false;

    while (start != end) {
        Node* mid = getMid(start, end);

        if (!mid) break;

        int compResult = comparator(mid->data, query);

        if (compResult == 0) {
            // Print the matched result
            cout << "Target found at index [" << i << "]" << endl;
            displayPlaylistHeader();
            displayPlaylists(mid, 1);
            matchFound = true;

            // Search for additional matches on both sides
            Node* temp = mid->next;
            while (temp && comparator(temp->data, query) == 0) {
                displayPlaylists(temp, 1);
                temp = temp->next;
            }

            cout << endl;

            temp = start;
            while (temp && temp != mid) {
                if (comparator(temp->data, query) == 0) {
                    displayPlaylists(temp, 1);
                }
                temp = temp->next;
            }

            break; // Exit the binary search once all matches are found
        } else if (compResult < 0) {
            start = mid->next; // Search right half
        } else {
            end = mid; // Search left half
        }
    }

    if (!matchFound) {
        cout << "No match found for the query '" << query << "'.\n";
    }
}

void insertIntoArray(string *arr, ifstream& fin, const string &filename) {//insert data from file into string array
    fin.open(filename);

    if (fin.is_open()) {
        int i = 0;
        string line; // Temporary variable to store each line
        while (getline(fin, line)) { // Use getline to read up to the newline character
            arr[i] = line;  // Store the line into the array
            i++;
        }
    }
    fin.close();
}

void searchSubMenu(Node* head) {
    while (true) {
        ifstream fin;
        string titleSearchTarget[100];
        string artistSearchTarget[100];
        string albumSearchTarget[100];
        string genreSearchTarget[100];
        string languageSearchTarget[100];
        int searchType;//Binary search or Jump search

        cout << "\n========== Search Menu ==========\n";
        cout << "1. Search by Title\n";
        cout << "2. Search by Artist\n";
        cout << "3. Search by Album\n";
        cout << "4. Search by Genre\n";
        cout << "5. Search by Language\n";
        cout << "6. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        Node* sortedHead = nullptr;
        switch (choice) {
            case 1:
                cout << "Search by Title selected.\n\n";
                insertIntoArray(titleSearchTarget, fin, "../target_search_title.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                        return toLower(a.song_title) < toLower(b.song_title);
                });

                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << titleSearchTarget[i] << endl;
                    binarySearch(sortedHead, titleSearchTarget[i], [](const Playlist& p, const string& query) {
                        return contains(p.song_title, query) ? 0 : (toLower(p.song_title) < toLower(query) ? -1 : 1);
                    }, i);

                    cout << endl;
                }
                break;
            case 2:
                cout << "Search by Artist selected.\n";
                insertIntoArray(artistSearchTarget, fin, "../target_search_artist.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.artist) < toLower(b.artist);
                });

                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << artistSearchTarget[i] << endl;
                    binarySearch(sortedHead, artistSearchTarget[i], [](const Playlist& p, const string& query) {
                        return contains(p.artist, query) ? 0 : (toLower(p.artist) < toLower(query) ? -1 : 1);
                    }, i);

                    cout << endl;
                }
                break;
            case 3:
                cout << "Search by Album selected.\n";
                insertIntoArray(albumSearchTarget, fin, "../target_search_album.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.album) < toLower(b.album);
                });

                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << albumSearchTarget[i] << endl;
                    binarySearch(sortedHead, albumSearchTarget[i], [](const Playlist& p, const string& query) {
                        return contains(p.album, query) ? 0 : (toLower(p.album) < toLower(query) ? -1 : 1);
                    }, i);

                    cout << endl;
                }
                break;
            case 4:
                cout << "Search by Genre selected.\n";
                insertIntoArray(genreSearchTarget, fin, "../target_search_genre.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.genre) < toLower(b.genre);
                });

                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << genreSearchTarget[i] << endl;
                    binarySearch(sortedHead, genreSearchTarget[i], [](const Playlist& p, const string& query) {
                        return contains(p.genre, query) ? 0 : (toLower(p.genre) < toLower(query) ? -1 : 1);
                    }, i);

                    cout << endl;
                }
                break;
            case 5:
                cout << "Search by Language selected.\n";
                insertIntoArray(languageSearchTarget, fin, "../target_search_language.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.language) < toLower(b.language);
                });

                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << languageSearchTarget[i] << endl;
                    binarySearch(sortedHead, languageSearchTarget[i], [](const Playlist& p, const string& query) {
                        return contains(p.language, query) ? 0 : (toLower(p.language) < toLower(query) ? -1 : 1);
                    }, i);

                    cout << endl;
                }
                break;
            case 6:
                return; // Return to the main menu
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        fin.close();
    }
}

// NAQIB PART
// Function placeholders for additional functions sub-functions
void additionalFunctionsSubMenu(Node* head) {
    while (true) {
        cout << "\n========== Additional Functions Menu ==========\n";
        cout << "1. Additional Function 1\n"; // Placeholder for the first additional function
        cout << "2. Additional Function 2\n"; // Placeholder for the second additional function
        cout << "3. Additional Function 2\n"; // Placeholder for the second additional function
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Additional Function 1 selected. Functionality to be implemented.\n";
                break;
            case 2:
                cout << "Additional Function 2 selected. Functionality to be implemented.\n";
                break;
            case 3:
                cout << "Additional Function 3 selected. Functionality to be implemented.\n";
                break;
            case 4:
                return; // Return to the main menu
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

void displayPlaylistHeader() {
    cout << left
        << setw(10) << "Song ID"
        << setw(40) << "Title"
        << setw(30) << "Artist"
        << setw(30) << "Album"
        << setw(15) << "Genre"
        << setw(15) << "Release Date"
        << setw(10) << "Duration"
        << setw(15) << "Popularity"
        << setw(10) << "Stream"
        << setw(10) << "Language" << endl;
    cout << string(185, '-') << endl;
}

void displayPlaylists(const Node* head, const int limit = 100){
    const Node* current = head;
    int count = 0;

    while (current && count < limit){
        const auto&[song_id, song_title, artist, album, genre,
            release_date, duration, popularity, stream, language] = current->data;
        cout << setw(10) << song_id
            << setw(40) << truncateText(song_title)
            << setw(30) << truncateText(artist)
            << setw(30) << truncateText(album)
            << setw(15) << truncateText(genre)
            << setw(15) << release_date
            << setw(10) << duration
            << setw(15) << popularity
            << setw(10) << stream
            << setw(10) << language << endl;

        current = current->next;
        ++count;
    }
}

void sortMenu(Node*& head, const Node* originalHead) {
    while (true) {
        cout << "\n========== Sorting Menu ==========\n";

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
        cout << "11. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int columnChoice;
        cin >> columnChoice;

        if (columnChoice == 11) break;

        if (!(columnChoice >= 1 && columnChoice <= 10)) {
            cout << "Invalid column choice. Please try again.\n";
            continue;
        }

        cout << "\nChoose Sorting Algorithm:\n";
        cout << "1. Merge Sort\n";
        cout << "2. Quick Sort\n";
        cout << "Enter your choice: ";
        int algoChoice;
        cin >> algoChoice;

        if (!(algoChoice == 1 || algoChoice == 2)) {
            cout << "Invalid algorithm choice. Please try again.\n";
            continue;
        }

        cout << "\nChoose Sorting Order:\n";
        cout << "1. Ascending\n";
        cout << "2. Descending\n";
        cout << "Enter your choice: ";
        int orderChoice;
        cin >> orderChoice;

        bool ascending = (orderChoice == 1);

        string columnName;
        auto comparator = [&](const Playlist& a, const Playlist& b) {
            switch (columnChoice) {
                case 1: columnName = "Song ID";
                    return ascending
                               ? extractSongIdNumber(a.song_id) < extractSongIdNumber(b.song_id)
                               : extractSongIdNumber(a.song_id) > extractSongIdNumber(b.song_id);
                case 2: columnName = "Title";
                    return ascending ? a.song_title < b.song_title : a.song_title > b.song_title;
                case 3: columnName = "Artist";
                    return ascending ? a.artist < b.artist : a.artist > b.artist;
                case 4: columnName = "Album";
                    return ascending ? a.album < b.album : a.album > b.album;
                case 5: columnName = "Genre";
                    return ascending ? a.genre < b.genre : a.genre > b.genre;
                case 6: columnName = "Release Date";
                    return ascending ? a.release_date < b.release_date : a.release_date > b.release_date;
                case 7: columnName = "Duration";
                    return ascending ? a.duration < b.duration : a.duration > b.duration;
                case 8: columnName = "Popularity";
                    return ascending ? a.popularity < b.popularity : a.popularity > b.popularity;
                case 9: columnName = "Streams";
                    return ascending ? a.stream < b.stream : a.stream > b.stream;
                case 10: columnName = "Language";
                    return ascending ? a.language < b.language : a.language > b.language;
                default: return true;
            }
        };

        long long swapCount = 0;
        string algorithmName;
        string sortOrder = ascending ? "Ascending" : "Descending";

        // Always start with a fresh copy of the original data
        head = deepCopyList(originalHead);

        if (algoChoice == 1) {
            auto start = chrono::high_resolution_clock::now();
            globalSortStartTimeMs = chrono::duration_cast<chrono::milliseconds>(start.time_since_epoch()).count();

            head = mergeSort(head, comparator, swapCount);

            auto end = chrono::high_resolution_clock::now();
            globalSortEndTimeMs = chrono::duration_cast<chrono::milliseconds>(end.time_since_epoch()).count();
            chrono::duration<double> elapsed = end - start;

            globalSwapCount = swapCount;
            globalSortTimeMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

            algorithmName = "Merge Sort";
        } else if (algoChoice == 2) {

            auto start = chrono::high_resolution_clock::now();
            globalSortStartTimeMs = chrono::duration_cast<chrono::milliseconds>(start.time_since_epoch()).count();

            head = quickSort(head, comparator, swapCount);

            auto end = chrono::high_resolution_clock::now();
            globalSortEndTimeMs = chrono::duration_cast<chrono::milliseconds>(end.time_since_epoch()).count();
            chrono::duration<double> elapsed = end - start;

            globalSwapCount = swapCount;
            globalSortTimeMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

            algorithmName = "Quick Sort";
        }

        cout << "\n========== SORTED PLAYLIST ==========\n";
        displayPlaylistHeader();
        displayPlaylists(head);
        displaySortingResults(algorithmName, sortOrder, columnName);
    }
}

void displaySortingResults(const string& algorithmName, const string& sortOrder, const string& sortBy){// function to display sorting results
    cout << "\n========== SORTING RESULTS ==========\n";
    cout << "Algorithm    : " << algorithmName << endl;
    cout << "Order        : " << sortOrder << endl;
    cout << "Sorted By    : " << sortBy << endl;
    cout << "-------------------------------------\n";
    cout << "Total Swaps  : " << globalSwapCount << endl;
    cout << "Start Time   : " << globalSortStartTimeMs << " milliseconds\n";
    cout << "End Time     : " << globalSortEndTimeMs << " milliseconds\n";
    cout << "Elapsed Time : " << globalSortTimeMs << " milliseconds\n";
    cout << "=====================================\n";
}

// Main function with updated switch cases
int main() {
    const string filename = "../spotify_songs_dataset_unsorted.csv"; // Path to dataset
    Node* originalHead = nullptr; // Original data storage
    Node* head = readCsv(filename, originalHead);

    while (true) {
        cout << "\n==========  Song Searcher ==========\n";
        cout << "1: Display Playlists\n";
        cout << "2: Sort Playlists\n";
        cout << "3: Search\n";
        cout << "4: Additional Functions\n";
        cout << "5: Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                displayPlaylistHeader();
                displayPlaylists(head);
                break;
            case 2:
                sortMenu(head, originalHead); // Updated function
                break;
            case 3:
                searchSubMenu(head);
                break;
            case 4:
                additionalFunctionsSubMenu(head);
                break;
            case 5:
                cout << "Exiting program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}