#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <functional> // for function
using namespace std;

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
    int rowIndex;

    explicit Node(Playlist  playlist) : data(move(playlist)), next(nullptr), rowIndex(0) {}

    // Constructor definition for Node
    Node(const Playlist& playlist, const int index)
        : data(playlist), next(nullptr), rowIndex(index) {}
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
    const string lowerStr = toLower(str);
    const string lowerQuery = toLower(query);
    return lowerStr.find(lowerQuery) != string::npos;
}

string erase(const string& str, const char c) {// Function to remove a character from a string
    string newStr = str;
    newStr.erase(ranges::remove(newStr, c).begin(), newStr.end());
    return newStr;
}

string formatWithCommas(const long long number) {
    string numStr = to_string(number); // Convert the number to a string
    int insertPosition = numStr.length() - 3; // Start inserting commas after the third-last digit

    // Traverse the string backwards and insert commas
    while (insertPosition > 0) {
        numStr.insert(insertPosition, ","); // Insert comma
        insertPosition -= 3; // Move back 3 positions
    }

    return numStr;
}

long long globalSwapCount = 0; // Global counter to track swaps during sorting
long long globalTotalMs = 0; // Global variable to track sorting duration in milliseconds
long long globalStartTimeMs = 0; // Global start time in milliseconds
long long globalEndTimeMs = 0; // Global end time in milliseconds
chrono::system_clock::time_point startTime, endTime;
chrono::duration<double> elapsed;

void startTimer() {
    startTime = chrono::high_resolution_clock::now();
    globalStartTimeMs = 0;
    globalStartTimeMs = chrono::duration_cast<chrono::milliseconds>(startTime.time_since_epoch()).count();
}

void endTimer() {
    endTime = chrono::high_resolution_clock::now();
    globalEndTimeMs = chrono::duration_cast<chrono::milliseconds>(endTime.time_since_epoch()).count();
    elapsed = endTime - startTime;
}

// Function Declarations
void displayPlaylistHeader();
void displayPlaylists(const Node*, int);
void displaySortingResults(const string&, const string&, const string&);
void displaySearchingResults(const string&, const string&);

Node* deepCopyList(const Node* head) {
    if (!head) return nullptr;

    auto* newHead = new Node(head->data, head->rowIndex); // Copy rowIndex
    const Node* current = head->next;
    Node* newCurrent = newHead;

    while (current) {
        newCurrent->next = new Node(current->data, current->rowIndex); // Copy rowIndex
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
    int rowIndex = 0;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        rowIndex++;
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

        auto newNode = new Node(playlist, rowIndex);

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
    long long dummySwap = 0;  //Dummy variable for swap count
    return mergeSort(copyList, comparator, dummySwap); // Use existing merge sort
}

// Function to get the middle node
Node* getMid(Node* start, const Node* end) {
    Node* slow = start;
    const Node* fast = start;

    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

// Binary Search Implementation
bool binarySearch(Node* head, const string& query, const function<int(const Playlist&, const string&)>& comparator,
                  const bool limitOutput, const int limit = 5) {
    Node* start = head;
    const Node* end = nullptr;
    bool matchFound = false;
    int count = 0; // Tracks the number of results displayed

    while (start != end) {
        const Node* mid = getMid(start, end);

        if (!mid) break;

        if (const int compResult = comparator(mid->data, query); compResult == 0) {
            // Match found
            cout << "Match found at index [" << mid->rowIndex << "]" << endl;
            displayPlaylistHeader();
            displayPlaylists(mid, 1); // Display the match
            matchFound = true;
            count++;

            // Search for matches on the right side
            const Node* temp = mid->next;
            while (temp && comparator(temp->data, query) == 0) {
                if (limitOutput && count >= limit) break; // Limit the output if required
                displayPlaylists(temp, 1);
                temp = temp->next;
                count++;
            }

            // Search for matches on the left side
            temp = start;
            while (temp && temp != mid) {
                if (comparator(temp->data, query) == 0) {
                    if (limitOutput && count >= limit) break; // Limit the output if required
                    displayPlaylists(temp, 1);
                    count++;
                }
                temp = temp->next;
            }

            break; // Exit the binary search loop after finding all matches
        } else if (compResult < 0) {
            start = mid->next; // Search on the right
        } else {
            end = mid; // Search on the left
        }
    }

    string algorithmName = "Binary Search";

    if (!matchFound) {
        cout << "No match found for the query '" << query << "'.\n";
        return false;
    }

    return true;
}

void ternarySearchRecursive(Node* start, Node* end, const string& query, const function<int(const Playlist&, const string&)>& comparator,
                            int& count, const bool limitOutput, const int limit, bool& headerDisplayed) {
    if (!start || start == end || count >= limit) {
        return; // Base case: stop if the range is invalid or the limit is reached
    }

    // Find midpoints
    Node* mid1 = getMid(start, end);
    Node* mid2 = mid1 ? getMid(mid1->next, end) : nullptr;

    // Check for mid1 match
    if (mid1 && comparator(mid1->data, query) == 0) {
        if (!headerDisplayed) {
            // Display header before the first match
            cout << "Match found at node index [" << mid1->rowIndex << "]:" << endl;
            displayPlaylistHeader();
            headerDisplayed = true;
        }
        displayPlaylists(mid1, 1); // Display only the playlist data
        count++;
        if (limitOutput && count >= limit) return;
    }

    // Check for mid2 match
    if (mid2 && comparator(mid2->data, query) == 0) {
        if (!headerDisplayed) {
            // Display header before the first match
            cout << "Match found at node index [" << mid2->rowIndex << "]:" << endl;
            displayPlaylistHeader();
            headerDisplayed = true;
        }
        displayPlaylists(mid2, 1); // Display only the playlist data
        count++;
        if (limitOutput && count >= limit) return;
    }

    // Recursive narrowing of search space
    if (mid1 && comparator(mid1->data, query) > 0) {
        // Search the left segment
        ternarySearchRecursive(start, mid1, query, comparator, count, limitOutput, limit, headerDisplayed);
    } else if (mid2 && comparator(mid2->data, query) < 0) {
        // Search the right segment
        ternarySearchRecursive(mid2->next, end, query, comparator, count, limitOutput, limit, headerDisplayed);
    } else {
        // Search the middle segment
        ternarySearchRecursive(mid1 ? mid1->next : nullptr, mid2, query, comparator, count, limitOutput, limit, headerDisplayed);
    }
}

bool ternarySearch(Node* head, const string& query, const function<int(const Playlist&, const string&)>& comparator,
                   const int i, const bool limitOutput, const int limit = 5) {
    int count = 0; // Counter to track the number of results displayed
    bool matchFound = false;

    // Use a flag to ensure the header is displayed only once
    bool headerDisplayed = false;

    // Call the recursive helper function
    ternarySearchRecursive(head, nullptr, query, comparator, count, limitOutput, limit, headerDisplayed);

    if (count > 0) {
        matchFound = true;
    }

    if (!matchFound) {
        cout << "No match found for the query '" << query << "'.\n";
        return false;
    }
    return true;
}

void insertIntoArray(string *arr, const string &filename) {//insert data from file into string array
    ifstream fin;
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

void searchSubMenu(const Node* head) {
    while (true) {
        string titleSearchTarget[100];
        string artistSearchTarget[100];
        string albumSearchTarget[100];
        string genreSearchTarget[100];
        string languageSearchTarget[100];
        int searchType;//Binary search or Ternary search
        string algorithmName;

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

        if (choice == 6) {
            return; // Return to the main menu
        }

        cout << "1. Binary Search" << endl;
        cout << "2. Ternary Search" << endl;
        cout << "Select Search Type: ";
        cin >> searchType;
        algorithmName = (searchType == 1) ? "Binary Search" : "Ternary Search";

        Node* sortedHead = nullptr;
        switch (choice) {
            case 1:
                cout << "Search by Title selected.\n\n";
                insertIntoArray(titleSearchTarget, "../target_search_title.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                        return toLower(a.song_title) < toLower(b.song_title);
                });

                startTimer();
                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << i + 1 << ": " << titleSearchTarget[i] << endl;
                    if (algorithmName == "Binary Search") {
                        binarySearch(sortedHead, titleSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.song_title, query) ? 0 : (toLower(p.song_title) < toLower(query) ? -1 : 1);
                        }, false);
                    } else if (algorithmName == "Ternary Search") {
                        ternarySearch(sortedHead, titleSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.song_title, query) ? 0 : (toLower(p.song_title) < toLower(query) ? -1 : 1);
                        }, i, true, 5);
                    }
                    cout << endl;
                }
                endTimer();
                globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
                displaySearchingResults(algorithmName, "Title");
                break;
            case 2:
                cout << "Search by Artist selected.\n";
                insertIntoArray(artistSearchTarget, "../target_search_artist.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.artist) < toLower(b.artist);
                });

                startTimer();
                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << artistSearchTarget[i] << endl;
                    if (algorithmName == "Binary Search") {
                        binarySearch(sortedHead, artistSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.artist, query) ? 0 : (toLower(p.artist) < toLower(query) ? -1 : 1);
                        }, false);
                    } else if (algorithmName == "Ternary Search") {
                        ternarySearch(sortedHead, artistSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.artist, query) ? 0 : (toLower(p.artist) < toLower(query) ? -1 : 1);
                        }, i, true, 5);
                    }
                    cout << endl;
                }
                endTimer();
                globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
                displaySearchingResults(algorithmName, "Artist");
                break;
            case 3:
                cout << "Search by Album selected.\n";
                insertIntoArray(albumSearchTarget, "../target_search_album.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.album) < toLower(b.album);
                });

                startTimer();
                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << albumSearchTarget[i] << endl;
                    if (algorithmName == "Binary Search") {
                        binarySearch(sortedHead, albumSearchTarget[i], [](const Playlist& p, const string& query) {
                            return toLower(erase(p.album, '.')) == toLower(query) ? 0 : (toLower(p.album) < toLower(query) ? -1 : 1);
                        }, false);
                    } else if (algorithmName == "Ternary Search") {
                        ternarySearch(sortedHead, albumSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.album, query) ? 0 : (toLower(p.album) < toLower(query) ? -1 : 1);
                        }, i, true, 5);
                    }


                    cout << endl;
                }
                endTimer();
                globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
                displaySearchingResults(algorithmName, "Album");
                break;
            case 4:
                cout << "Search by Genre selected.\n";
                insertIntoArray(genreSearchTarget, "../target_search_genre.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.genre) < toLower(b.genre);
                });

                startTimer();
                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << genreSearchTarget[i] << endl;
                    if (algorithmName == "Binary Search") {
                        binarySearch(sortedHead, genreSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.genre, query) ? 0 : (toLower(p.genre) < toLower(query) ? -1 : 1);
                        }, true, 5); // Limit output to 5 results
                    } else if (algorithmName == "Ternary Search") {
                        ternarySearch(sortedHead, genreSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.genre, query) ? 0 : (toLower(p.genre) < toLower(query) ? -1 : 1);
                        }, i, true, 5);
                    }


                    cout << endl;
                }
                endTimer();
                globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
                displaySearchingResults(algorithmName, "Genre");
                break;
            case 5:
                cout << "Search by Language selected.\n";
                insertIntoArray(languageSearchTarget, "../target_search_language.txt");

                sortedHead = getSortedHead(head, [](const Playlist& a, const Playlist& b) {
                    return toLower(a.language) < toLower(b.language);
                });

                startTimer();
                for (int i = 0; i < 100; i++) {
                    cout << "Search Target " << (i + 1) << ": " << languageSearchTarget[i] << endl;
                    if (algorithmName == "Binary Search") {
                        binarySearch(sortedHead, languageSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.language, query) ? 0 : (toLower(p.language) < toLower(query) ? -1 : 1);
                        }, true, 5);// limits to 5 results only
                    } else if (algorithmName == "Ternary Search") {
                        ternarySearch(sortedHead, languageSearchTarget[i], [](const Playlist& p, const string& query) {
                            return contains(p.language, query) ? 0 : (toLower(p.language) < toLower(query) ? -1 : 1);
                        }, i, true, 5);
                    }
                    cout << endl;
                }
                endTimer();
                globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
                displaySearchingResults(algorithmName, "Language");
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Function to extract the year from a release date (YYYY-MM-DD)
int extractYear(const string& release_date) {
    if (release_date.length() >= 4) {
        return stoi(release_date.substr(0, 4));  // First 4 chars are the year
    }
    return 0;  // Invalid date format
}

// Function to calculate total playlist duration and display results
void calculateTotalPlaylistDurationForYears(const Node* head) {
    constexpr int MAX_YEARS = 100;
    int years[MAX_YEARS];           // Array to store unique years
    int durations[MAX_YEARS] = {0}; // Array to store total durations for each year
    int yearCount = 0;              // Track the number of unique years
    int totalDuration = 0;          // Total duration across all songs in the playlist dataset

    const Node* current = head;

    // Step 1: Traverse the linked list
    while (current) {
        const int year = extractYear(current->data.release_date); // Extract year from release_date
        totalDuration += current->data.duration;            // Add song duration to total

        // Check if the current year already exists in `years` array
        bool yearFound = false;
        for (int i = 0; i < yearCount; i++) {
            if (years[i] == year) {
                durations[i] += current->data.duration; // Add duration to the corresponding year
                yearFound = true;
                break;
            }
        }

        // If the year is not found, add it to the `years` array
        if (!yearFound) {
            years[yearCount] = year;
            durations[yearCount] = current->data.duration;
            yearCount++;
        }

        current = current->next; // Move to the next node
    }

    // Step 2: Sort years and durations in descending order using Bubble Sort
    for (int i = 0; i < yearCount - 1; i++) {
        for (int j = 0; j < yearCount - i - 1; j++) {
            if (years[j] < years[j + 1]) {
                // Swap years
                const int tempYear = years[j];
                years[j] = years[j + 1];
                years[j + 1] = tempYear;

                // Swap durations
                const int tempDuration = durations[j];
                durations[j] = durations[j + 1];
                durations[j + 1] = tempDuration;
            }
        }
    }

    // Step 3: Display Results
    cout << "\nYearly Total Playlist Duration:\n";
    cout << left << setw(10) << "Year"
         << setw(15) << "Duration (H:M)"
         << "Percentage (%)\n";
    cout << string(40, '-') << endl;

    for (int i = 0; i < yearCount; i++) {
        // Hours and minutes for the year
        const int hours = durations[i] / 3600;      // Convert seconds to hours
        const int minutes = (durations[i] % 3600) / 60;   // Remaining seconds converted to minutes
        const double percentage = static_cast<double>(durations[i]) / totalDuration * 100;

        // Display year, duration in 'H:M' format, and percentage
        cout << setw(10) << years[i]
             << setw(15) << (to_string(hours) + "h " + to_string(minutes) + "m")
             << fixed << setprecision(2) << percentage << "%\n";
    }

    // Step 4: Display Total Playlist Duration
    const int totalHours = totalDuration / 3600;         // Total seconds to hours
    const int totalMinutes = (totalDuration % 3600) / 60; // Remaining seconds to minutes
    cout << "\nTotal Playlist Duration: " << totalHours << "h " << totalMinutes << "m\n";
}

void totalStreamsByLanguage(const Node* head) {
    if (!head) {
        cout << "No data found.\n";
        return;
    }

    // Arrays to keep track of languages and their respective stream counts
    string languages[100];       // Assuming at most 100 unique languages
    long long streams[100] = {0};
    int languageCount = 0;

    const Node* current = head;
    while (current) {
        const string& lang = current->data.language;
        const long long stream = current->data.stream;

        // Check if the language already exists in the array
        bool found = false;
        for (int i = 0; i < languageCount; i++) {
            if (languages[i] == lang) {
                streams[i] += stream; // Add streams to the existing language
                found = true;
                break;
            }
        }

        // If the language is not found, add it to the array
        if (!found) {
            languages[languageCount] = lang;
            streams[languageCount] = stream;
            languageCount++;
        }

        current = current->next;
    }

    // Display the aggregated results
    cout << "\nTotal Streams by Language:\n";
    cout << left << setw(20) << "Language" << "Total Streams\n";
    cout << string(40, '-') << endl;

    for (int i = 0; i < languageCount; i++) {
        cout << left << setw(20) << languages[i] << right << formatWithCommas(streams[i]) << endl;
    }
}

void calculateTotalSongByArtist(const Node* head) {
    const string artistNames[] = {
        "Joseph Gibson", "Preston Watkins", "Elizabeth Ford", "Aaron Anderson", "Joe Melendez MD"
    };
    constexpr int arraySize = size(artistNames);

    // Traverse the linked list for each artist
    for (int i = 0; i < arraySize; i++) {
        const Node* current = head; // Reset current to the head of the list for each artist
        int totalSongs = 0;         // Counter for the total number of rows with matching artist name
        bool headerDisplayed = false;

        while (current) {
            // Check if the artist name matches
            if (contains(artistNames[i], current->data.artist)) {
                if (!headerDisplayed) {
                    // Display header before the first match
                    cout << "\nMatching Rows for Artist: " << artistNames[i] << endl;
                    displayPlaylistHeader(); // Function to display column headers (if applicable)
                    headerDisplayed = true;
                }
                displayPlaylists(current, 1); // Function to display data of a single node
                totalSongs++; // Increment the counter for each match
            }
            current = current->next; // Move to the next node
        }

        // Display summary
        if (totalSongs > 0) {
            cout << "\nTotal Rows for Artist '" << artistNames[i] << "': " << totalSongs << endl;
        } else {
            cout << "\nNo rows found for artist: " << artistNames[i] << endl;
        }
    }
}

// Function placeholders for additional functions sub-functions
void additionalFunctionsSubMenu(const Node* head) {
    while (true) {
        cout << "\n========== Additional Functions Menu ==========\n";
        cout << "1. Calculate Total Playlist Duration Per Year\n"; // Placeholder for the first additional function
        cout << "2. Aggregate Total Streams by Language\n"; // Placeholder for the second additional function
        cout << "3. Calculate Total Songs Sang by Artist\n"; // Placeholder for the second additional function
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                calculateTotalPlaylistDurationForYears(head);
            break;
            case 2:
                totalStreamsByLanguage(head);
                break;
            case 3:
                calculateTotalSongByArtist(head);
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
        << setw(15) << "Stream"
        << setw(10) << "Language" << endl;
    cout << string(189, '-') << endl;
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
            << setw(15) << formatWithCommas(stream)
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
            startTimer();

            head = mergeSort(head, comparator, swapCount);

            endTimer();
            globalSwapCount = swapCount;
            globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

            algorithmName = "Merge Sort";
        } else if (algoChoice == 2) {
            startTimer();

            head = quickSort(head, comparator, swapCount);

            endTimer();
            globalSwapCount = swapCount;
            globalTotalMs = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

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
    cout << "Start Time   : " << globalStartTimeMs << " milliseconds\n";
    cout << "End Time     : " << globalEndTimeMs << " milliseconds\n";
    cout << "Elapsed Time : " << globalTotalMs << " milliseconds\n";
    cout << "=====================================\n";
}

void displaySearchingResults(const string& algorithmName, const string& searchBy){// function to display searching results
    cout << "\n========== SEARCHING RESULTS ==========\n";
    cout << "Algorithm    : " << algorithmName << endl;
    cout << "Search by    : " << searchBy << endl;
    cout << "-------------------------------------\n";
    cout << "Start Time   : " << globalStartTimeMs << " milliseconds\n";
    cout << "End Time     : " << globalEndTimeMs << " milliseconds\n";
    cout << "Elapsed Time : " << globalTotalMs << " milliseconds\n";
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
                delete(head);
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}