// Importing libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>

#include <numeric> // for std::accumulate
#include <limits>  // for input validation
#include <vector>
// Defining a struct to store the playlist data

struct Playlist
{
    std::string song_id;
    std::string song_title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string release_date; // Use strings for simplicity in parsing dates
    int duration;
    int popularity;
    int stream;
    std::string language;
    std::string explicit_content; // "Yes" or "No"
};

// Function to read the CSV file and store the data into a vector of Playlist structures
std::vector<Playlist> readCsv(const std::string& filename)
{
    std::vector<Playlist> playlists;
    std::ifstream file(filename); // Open CSV file
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return playlists;
    }

    std::string line;
    // Reading the header (first line)
    std::getline(file, line);

    // Reading each row from the csv
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Playlist playlist;
        std::string value;

        std::getline(ss, playlist.song_id, ',');
        std::getline(ss, playlist.song_title, ',');
        std::getline(ss, playlist.artist, ',');
        std::getline(ss, playlist.album, ',');
        std::getline(ss, playlist.genre, ',');
        std::getline(ss, playlist.release_date, ',');
        std::getline(ss, value, ','); // Duration
        playlist.duration = std::stoi(value); // Convert string to int
        std::getline(ss, value, ','); // Popularity
        playlist.popularity = std::stoi(value); // Convert string to int
        std::getline(ss, value, ','); // Stream
        playlist.stream = std::stoi(value); // Convert string to int
        std::getline(ss, playlist.language, ',');
        std::getline(ss, playlist.explicit_content, ',');

        // Append the data to the vector
        playlists.push_back(playlist);
    }

    file.close();
    return playlists;
}

// Function prototypes
void sortMenu(std::vector<Playlist>& playlists);
void additionalFunctionsMenu(const std::vector<Playlist>& playlists);

std::string truncateText(const std::string& text, size_t maxLength = 12)
{
    if (text.length() > maxLength)
    {
        return text.substr(0, maxLength) + "..";
    }
    return text;
}


void displayMenu()
{
    std::cout << "\n========== Spotify Playlist Manager ==========\n";
    std::cout << "1. Display Playlists (first 100 entries)\n";
    std::cout << "2. Sort Playlists\n";
    std::cout << "3. Search Playlist\n"; // New option for Search
    std::cout << "4. Additional Functions\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter your choice: ";
}

// Function to display the playlists (re-used from main code)
void displayPlaylists(const std::vector<Playlist>& playlists, int limit = 100)
{
    std::cout << std::setw(10) << "Song ID"
        << std::setw(20) << "Title"
        << std::setw(20) << "Artist"
        << std::setw(20) << "Album"
        << std::setw(15) << "Genre"
        << std::setw(15) << "Release Date"
        << std::setw(10) << "Duration"
        << std::setw(15) << "Popularity"
        << std::setw(10) << "Stream"
        << std::setw(10) << "Language"
        << std::setw(10) << "Explicit" << std::endl;
    std::cout << std::string(180, '-') << std::endl;

    for (size_t i = 0; i < playlists.size() && i < limit; ++i)
    {
        const auto& p = playlists[i];
        std::cout << std::setw(10) << p.song_id
            << std::setw(20) << truncateText(p.song_title)
            << std::setw(20) << truncateText(p.artist)
            << std::setw(20) << truncateText(p.album)
            << std::setw(15) << truncateText(p.genre)
            << std::setw(15) << p.release_date
            << std::setw(10) << p.duration
            << std::setw(15) << p.popularity
            << std::setw(10) << p.stream
            << std::setw(10) << p.language
            << std::setw(10) << p.explicit_content << std::endl;
    }
}

// Placeholder for Search Menu
// Man ni aku letak view aku je ni, nanti kalau kau nk ubah pape tukar je kat sini <<<< Rafeeq
void searchMenu(const std::vector<Playlist>& playlists)
{
    while (true)
    {
        std::cout << "\n========== Search Menu ==========\n";
        std::cout << "1. Search by Song Title\n";
        std::cout << "2. Search by Artist\n";
        std::cout << "3. Search by Genre\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        case 2:
        case 3:
            std::cout << "\nThis feature is a placeholder for later functionality.\n";
            break;
        case 4:
            return; // Exit Search Menu
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}


void sortMenu(std::vector<Playlist>& playlists)
{
    while (true)
    {
        std::cout << "\n========== Sorting Menu ==========\n";

        // Step 1: Choose Column
        std::cout << "Choose Column to Sort By:\n";
        std::cout << "1. Song ID\n";
        std::cout << "2. Title\n";
        std::cout << "3. Artist\n";
        std::cout << "4. Album\n";
        std::cout << "5. Genre\n";
        std::cout << "6. Release Date\n";
        std::cout << "7. Duration\n";
        std::cout << "8. Popularity\n";
        std::cout << "9. Streams\n";
        std::cout << "10. Language\n";
        std::cout << "11. Back to Main Menu\n"; // Back option
        std::cout << "Enter your choice: ";
        int columnChoice;
        std::cin >> columnChoice;

        if (columnChoice == 11) break; // Exit sorting menu if "Back" is chosen.

        // Validate columnChoice
        if (columnChoice < 1 || columnChoice > 10)
        {
            std::cout << "Invalid column choice. Please try again.\n";
            continue;
        }

        // Step 2: Choose Sorting Algorithm
        std::cout << "\nChoose Sorting Algorithm:\n";
        std::cout << "1. Merge Sort\n";
        std::cout << "2. Quick Sort\n";
        std::cout << "Enter your choice: ";
        int algoChoice;
        std::cin >> algoChoice;

        if (algoChoice < 1 || algoChoice > 2)
        {
            std::cout << "Invalid algorithm choice. Please try again.\n";
            continue;
        }

        // Step 3: Choose Sorting Order
        std::cout << "\nChoose Sorting Order:\n";
        std::cout << "1. Ascending\n";
        std::cout << "2. Descending\n";
        std::cout << "Enter your choice: ";
        int orderChoice;
        std::cin >> orderChoice;

        bool ascending = (orderChoice == 1);
        if (orderChoice < 1 || orderChoice > 2)
        {
            std::cout << "Invalid sort order choice. Please try again.\n";
            continue;
        }

        // Step 4: Define comparator
        auto comparator = [&](const Playlist& a, const Playlist& b) {
            switch (columnChoice)
            {
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
        if (algoChoice == 1)
        {
            // Merge Sort
        }
        else if (algoChoice == 2)
        {
            // Quick Sort
        }

        // Step 6: Display sorted results
        std::cout << "\nPlaylists sorted successfully:\n";
        displayPlaylists(playlists);
    }
}

void additionalFunctionsMenu(const std::vector<Playlist>& playlists)
{
    while (true)
    {
        std::cout << "\n========== Additional Functions ==========\n";
        std::cout << "1. Additional Function Placeholder 1\n";
        std::cout << "2. Additional Function Placeholder 2\n";
        std::cout << "3. Additional Function Placeholder 3\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        case 2:
        case 3:
            std::cout << "\nThis feature is a placeholder for later functionality.\n";
            break;
        case 4:
            return; // Exit Additional Functions Menu
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}
int main()
{
    std::string filename =
        "../spotify_songs_dataset_unsorted.csv";
    // Path to your playlist dataset
    std::vector<Playlist> playlists = readCsv(filename);

    if (playlists.empty())
    {
        std::cout << "Error: No playlist data loaded. Exiting program.\n";
        return 1;
    }

    while (true)
    {
        displayMenu();
        int choice;
        std::cin >> choice;

        if (!std::cin)
        {
            // Check for invalid input
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
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
            std::cout << "Exiting program. Goodbye!\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}