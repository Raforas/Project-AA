// Importing libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>

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

// A simple function to print the playlists data
void displayPlaylists(const std::vector<Playlist>& playlists, int limit = 10)
{
    std::cout << std::setw(10) << "Song ID" << std::setw(40) << "Title" << std::setw(20) << "Artist"
        << std::setw(20) << "Album" << std::setw(15) << "Genre" << std::setw(15) << "Release Date"
        << std::setw(10) << "Duration" << std::setw(15) << "Popularity" << std::setw(10) << "Stream"
        << std::setw(10) << "Language" << std::setw(10) << "Explicit" << std::endl;
    std::cout << std::string(180, '-') << std::endl;

    // Display up to the specified 'limit' number of entries
    for (size_t i = 0; i < playlists.size() && i < limit; ++i)
    {
        const auto& p = playlists[i];
        std::cout << std::setw(10) << p.song_id << std::setw(40) << p.song_title << std::setw(20) << p.artist
            << std::setw(20) << p.album << std::setw(15) << p.genre << std::setw(15) << p.release_date
            << std::setw(10) << p.duration <<std::setw(15) << p.popularity << std::setw(10) << p.stream
            << std::setw(10) << p.language << std::setw(10) << p.explicit_content << std::endl;
    }
}

int main()
{
    // Input the file name containing the playlist data
    std::string filename =
        "D:/Documents/Utem's Works/#Year2Sem1/BITP2113 ALGORITHM ANALYSIS/Project AA/spotify_songs_dataset_latest.csv";
    // Example filename

    // Step 1: Read the CSV file
    std::vector<Playlist> playlists = readCsv(filename);

    // Step 2: Display the playlists (first 10 entries for simplicity)
    if (playlists.empty())
    {
        std::cout << "No data to display." << std::endl;
    }
    else
    {
        std::cout << "Displaying first 10 entries of the playlist:" << std::endl;
        displayPlaylists(playlists, 10);
    }

    // Step 3: Add any additional operations (sorting, filtering, etc.).
    // Example: Sort by song popularity (descending)
    std::sort(playlists.begin(), playlists.end(), [](const Playlist& a, const Playlist& b)
    {
        return a.popularity > b.popularity;
    });

    std::cout << "\nTop 10 popular songs by popularity:" << std::endl;
    displayPlaylists(playlists, 10);
    system("pause");
    return 0;
}
