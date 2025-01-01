#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional> // for std::function

using namespace std;

long long globalSwapCount = 0; // Global counter to track swaps during sorting
double globalSortTime = 0.0; // Global variable to track sorting duration

struct Playlist
{
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
};

struct Node
{
    Playlist data;
    Node* next;

    Node(const Playlist& playlist) : data(playlist), next(nullptr)
    {
    }
};

Node* readCsv(const string& filename)
{
    Node* head = nullptr; // Linked list head
    Node* tail = nullptr; // Pointer to track the end

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open the file " << filename << endl;
        return nullptr;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line))
    {
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

        Node* newNode = new Node(playlist);

        if (!head)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
    }

    file.close();
    return head;
}

string truncateText(const string& text, size_t maxLength = 12)
{
    if (text.length() > maxLength)
    {
        return text.substr(0, maxLength) + "..";
    }
    return text;
}

void displayPlaylists(Node* head, int limit = 100)
{
    cout << setw(10) << "Song ID"
        << setw(20) << "Title"
        << setw(20) << "Artist"
        << setw(20) << "Album"
        << setw(15) << "Genre"
        << setw(15) << "Release Date"
        << setw(10) << "Duration"
        << setw(15) << "Popularity"
        << setw(10) << "Stream"
        << setw(10) << "Language" << endl;
    cout << string(145, '-') << endl;

    Node* current = head;
    int count = 0;

    while (current && count < limit)
    {
        const auto& p = current->data;
        cout << setw(10) << p.song_id
            << setw(20) << truncateText(p.song_title)
            << setw(20) << truncateText(p.artist)
            << setw(20) << truncateText(p.album)
            << setw(15) << truncateText(p.genre)
            << setw(15) << p.release_date
            << setw(10) << p.duration
            << setw(15) << p.popularity
            << setw(10) << p.stream
            << setw(10) << p.language << endl;

        current = current->next;
        ++count;
    }
}

// Helper function to display sorting results
void displaySortingResults(const string& algorithmName, const string& sortOrder, const string& sortBy)
{
    cout << "\n========== SORTING RESULTS ==========\n";
    cout << "Algorithm    : " << algorithmName << endl;
    cout << "Order        : " << sortOrder << endl;
    cout << "Sorted By    : " << sortBy << endl;
    cout << "-------------------------------------\n";
    cout << "Total Swaps  : " << globalSwapCount << endl;
    cout << "Elapsed Time : " << fixed << setprecision(6) << globalSortTime << " seconds\n";
    cout << "=====================================\n";
}

// Helper function to extract the numeric part of song_id
int extractSongIdNumber(const string& song_id)
{
    size_t pos = 0;
    while (pos < song_id.size() && !isdigit(song_id[pos]))
    {
        ++pos;
    }
    return (pos < song_id.size()) ? stoi(song_id.substr(pos)) : 0;
}

// Merge Sort Implementation
Node* merge(Node* left, Node* right, auto comparator, long long& swapCount)
{
    if (!left) return right;
    if (!right) return left;

    Node* result = nullptr;

    if (comparator(left->data, right->data))
    {
        result = left;
        result->next = merge(left->next, right, comparator, swapCount);
    }
    else
    {
        result = right;
        result->next = merge(left, right->next, comparator, swapCount);
        swapCount++;
    }

    return result;
}

void split(Node* source, Node** front, Node** back)
{
    Node* slow = source;
    Node* fast = source->next;

    while (fast)
    {
        fast = fast->next;
        if (fast)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

Node* mergeSort(Node* head, auto comparator, long long& swapCount)
{
    if (!head || !head->next)
        return head;

    Node* front = nullptr;
    Node* back = nullptr;

    split(head, &front, &back);

    front = mergeSort(front, comparator, swapCount);
    back = mergeSort(back, comparator, swapCount);

    return merge(front, back, comparator, swapCount);
}

// Quick Sort Implementation
Node* getTail(Node* cur)
{
    while (cur != nullptr && cur->next != nullptr)
    {
        cur = cur->next;
    }
    return cur;
}

Node* partition(Node* head, Node* end, Node** newHead, Node** newEnd, auto comparator, long long& swapCount)
{
    Node* pivot = end;
    Node *prev = nullptr, *cur = head, *tail = pivot;

    *newHead = nullptr;

    while (cur != pivot)
    {
        if (comparator(cur->data, pivot->data))
        {
            if (!*newHead)
            {
                *newHead = cur;
            }
            prev = cur;
            cur = cur->next;
        }
        else
        {
            if (prev)
            {
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

    if (!*newHead)
    {
        *newHead = pivot;
    }

    *newEnd = tail;

    return pivot;
}

Node* quickSortRecursive(Node* head, Node* end, auto comparator, long long& swapCount)
{
    if (!head || head == end)
    {
        return head;
    }

    Node* newHead = nullptr;
    Node* newEnd = nullptr;

    Node* pivot = partition(head, end, &newHead, &newEnd, comparator, swapCount);

    if (newHead != pivot)
    {
        Node* temp = newHead;
        while (temp->next != pivot)
        {
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

Node* quickSort(Node* head, auto comparator, long long& swapCount, double& sortTime)
{
    if (!head || !head->next)
    {
        return head;
    }

    Node* end = getTail(head);

    auto start = chrono::high_resolution_clock::now();

    head = quickSortRecursive(head, end, comparator, swapCount);

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endTime - start;

    sortTime = elapsed.count();

    return head;
}

void sortMenu(Node*& head)
{
    while (true)
    {
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

        if (!(columnChoice >= 1 && columnChoice <= 10))
        {
            cout << "Invalid column choice. Please try again.\n";
            continue;
        }

        cout << "\nChoose Sorting Algorithm:\n";
        cout << "1. Merge Sort\n";
        cout << "2. Quick Sort\n";
        cout << "Enter your choice: ";
        int algoChoice;
        cin >> algoChoice;

        if (!(algoChoice == 1 || algoChoice == 2))
        {
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
        auto comparator = [&](const Playlist& a, const Playlist& b)
        {
            switch (columnChoice)
            {
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

        if (algoChoice == 1)
        {
            auto start = chrono::high_resolution_clock::now();

            head = mergeSort(head, comparator, swapCount);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;

            globalSwapCount = swapCount;
            globalSortTime = elapsed.count();

            algorithmName = "Merge Sort";
        }
        else if (algoChoice == 2)
        {
            double sortTime = 0.0;
            head = quickSort(head, comparator, swapCount, sortTime);

            globalSwapCount = swapCount;
            globalSortTime = sortTime;

            algorithmName = "Quick Sort";
        }

        cout << "\n========== SORTED PLAYLIST ==========\n";
        displayPlaylists(head);
        displaySortingResults(algorithmName, sortOrder, columnName);
    }
}


// Main menu function
void displayMenu()
{
    cout << "\n==========  Song Searcher ==========\n";
    cout << "1: Display Playlists\n";
    cout << "2: Sort Playlists\n";
    cout << "3: Search\n";
    cout << "4: Additional Functions\n";
    cout << "5: Exit\n";
    cout << "Enter your choice: ";
}

// Function placeholders for search sub-functions
// MANBILLL PART
void searchSubMenu(Node* head) {
    while (true) {
        cout << "\n========== Search Menu ==========\n";
        cout << "1. Search by Title\n";  // Placeholder for search by title
        cout << "2. Search by Artist\n"; // Placeholder for search by artist
        cout << "3. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Search by Title selected. Functionality to be implemented.\n";
            break;
        case 2:
            cout << "Search by Artist selected. Functionality to be implemented.\n";
            break;
        case 3:
            return; // Return to the main menu
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}


// NAQIIBBB PART
// Function placeholders for additional functions sub-functions
void additionalFunctionsSubMenu(Node* head) {
    while (true) {
        cout << "\n========== Additional Functions Menu ==========\n";
        cout << "1. Additional Function 1\n"; // Placeholder for the first additional function
        cout << "2. Additional Function 2\n"; // Placeholder for the second additional function
        cout << "3. Back to Main Menu\n";
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
            return; // Return to the main menu
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}



// Main function with updated switch cases
int main() {
    string filename = "../spotify_songs_dataset_unsorted.csv"; // Path to dataset
    Node* head = readCsv(filename);

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            displayPlaylists(head); // Call existing display functionality
            break;
        case 2:
            sortMenu(head); // Call existing sort functionality
            break;
        case 3:
            searchSubMenu(head); // Call search submenu
            break;
        case 4:
            additionalFunctionsSubMenu(head); // Call additional functions submenu
            break;
        case 5:
            cout << "Exiting program. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}