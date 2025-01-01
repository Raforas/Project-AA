// Importing libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional> // for std::function


// Defining a struct to store the playlist data
using namespace std;


long long globalSwapCount = 0; // Global counter to track swaps during sorting
double globalSortTime = 0.0;  // Global variable to track sorting duration

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

    ifstream file(filename); // Open CSV file
    if (!file.is_open())
    {
        cerr << "Error: Could not open the file " << filename << endl;
        return nullptr;
    }

    string line;
    // Reading the header (first line)
    getline(file, line);

    // Reading data rows from the CSV
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
        playlist.duration = stoi(value); // Convert string to int
        getline(ss, value, ','); // Popularity
        playlist.popularity = stoi(value); // Convert string to int
        getline(ss, value, ','); // Stream
        playlist.stream = stoi(value); // Convert string to int
        getline(ss, playlist.language, ',');

        // Create a new node for the playlist
        Node* newNode = new Node(playlist);

        // Append node to the linked list
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
    return head; // Return head of the linked list
}

// Function prototypes

void additionalFunctionsMenu();

string truncateText(const string& text, size_t maxLength = 12)
{
    if (text.length() > maxLength)
    {
        return text.substr(0, maxLength) + "..";
    }
    return text;
}


void displayMenu()
{
    cout << "\n========== Spotify Playlist Manager ==========\n";
    cout << "1. Display Playlists (first 100 entries)\n";
    cout << "2. Sort Playlists\n";
    cout << "3. Search Playlist\n"; // New option for Search
    cout << "4. Additional Functions\n";
    cout << "5. Exit\n";
    cout << "Enter your choice: ";
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

// Placeholder for Search Menu
// Man ni aku letak view aku je ni, nanti kalau kau nk ubah pape tukar je kat sini <<<< Rafeeq
void searchMenu()
{
    while (true)
    {
        cout << "\n========== Search Menu ==========\n";
        cout << "1. Search by Song Title\n";
        cout << "2. Search by Artist\n";
        cout << "3. Search by Genre\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
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

Node* merge(Node* left, Node* right, auto comparator, long long& swapCount) {
    if (!left) return right;
    if (!right) return left;

    Node* result = nullptr;

    if (comparator(left->data, right->data)) {
        result = left;
        result->next = merge(left->next, right, comparator, swapCount);
    } else {
        result = right;
        result->next = merge(left, right->next, comparator, swapCount);
        swapCount++; // Increment swap count when nodes are "moved" or re-ordered
    }

    return result;
}

void split(Node* source, Node** front, Node** back)
{
    Node* slow = source;
    Node* fast = source->next;

    // Advance fast two nodes, slow one node
    while (fast)
    {
        fast = fast->next;
        if (fast)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // Split the linked list
    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

Node* mergeSort(Node* head, auto comparator, long long& swapCount) {
    if (!head || !head->next)
        return head;

    Node* front = nullptr;
    Node* back = nullptr;

    // Split the list into two halves
    split(head, &front, &back);

    // Recursively sort the two halves
    front = mergeSort(front, comparator, swapCount);
    back = mergeSort(back, comparator, swapCount);

    // Merge the two sorted halves and count swaps
    return merge(front, back, comparator, swapCount);
}

void sortMenu(Node*& head)
{
    while (true)
    {
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
        if (columnChoice < 1 || columnChoice > 10)
        {
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

        if (algoChoice < 1 || algoChoice > 2)
        {
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
        if (orderChoice < 1 || orderChoice > 2)
        {
            cout << "Invalid sort order choice. Please try again.\n";
            continue;
        }

        // Step 4: Define comparator
        auto comparator = [&](const Playlist& a, const Playlist& b)
        {
            switch (columnChoice)
            {
            case 1:
                return ascending
                           ? extractSongIdNumber(a.song_id) < extractSongIdNumber(b.song_id)
                           : extractSongIdNumber(a.song_id) > extractSongIdNumber(b.song_id);
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

        // Merge Sort
        // Step 4: Sorting process (measure time and swaps)
        long long swapCount = 0;                            // Initialize swap counter
        auto start = chrono::high_resolution_clock::now();  // Start timing

        if (algoChoice == 1)
        {
            head = mergeSort(head, comparator, swapCount); // Perform merge sort
        }
        else if (algoChoice == 2)
        {
            // Placeholder for Quick Sort (if implemented)
            cout << "Quick Sort is currently not implemented.\n";
            continue;
        }

        auto end = chrono::high_resolution_clock::now(); // End timing
        chrono::duration<double> elapsed = end - start;  // Calculate elapsed time

        // Record global variables for later analysis
        globalSwapCount = swapCount;
        globalSortTime = elapsed.count();

        // Step 5: Display sorted results and metrics
        cout << "\nPlaylists sorted successfully:\n";
        displayPlaylists(head);

        cout << "\n========== SORTING RESULTS ==========\n";
        cout << "Total Swaps: " << globalSwapCount << endl;
        cout << "Elapsed Time: " << globalSortTime << " seconds\n";
        cout << "=====================================\n";
    }
}




void additionalFunctionsMenu()
{
    while (true)
    {
        cout << "\n========== Additional Functions ==========\n";
        cout << "1. Additional Function Placeholder 1\n";
        cout << "2. Additional Function Placeholder 2\n";
        cout << "3. Additional Function Placeholder 3\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
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

int main()
{
    string filename = "../spotify_songs_dataset_unsorted.csv"; // Path to dataset
    Node* head = readCsv(filename);

    if (!head)
    {
        cout << "Error: No playlist data loaded. Exiting program.\n";
        return 1;
    }

    while (true)
    {
        displayMenu();
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            displayPlaylists(head);
            break;
        case 2:
            sortMenu(head);
            break;
        case 3:
            cout << "Search functionality placeholder.\n";
            break;
        case 4:
            cout << "Additional functionality placeholder.\n";
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
