// Importing libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <bits/stdc++.h>
#include <map>

using namespace std;

// Defining a struct to store the playlist data
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
    string explicit_content; // "Yes" or "No"
};

// Function to read the CSV file and store the data into a vector of Playlist structures
vector<Playlist> readCsv(const string& filename){
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

// A simple function to print the playlists data
void displayPlaylists(const vector<Playlist>& playlists, const int limit = 10){
    cout << setw(10) << "Song ID" << setw(40) << "Title" << setw(20) << "Artist"
        << setw(20) << "Album" << setw(15) << "Genre" << setw(15) << "Release Date"
        << setw(10) << "Duration" << setw(15) << "Popularity" << setw(10) << "Stream"
        << setw(10) << "Language" << setw(10) << "Explicit" << endl;
    cout << string(180, '-') << endl;

    // Display up to the specified 'limit' number of entries
    for (size_t i = 0; i < playlists.size() && i < limit; ++i) {
        const auto&[song_id, song_title, artist, album, genre, release_date, duration, popularity, stream, language, explicit_content] = playlists[i];
        cout << setw(10) << song_id << setw(40) << song_title << setw(20) << artist
            << setw(20) << album << setw(15) << genre << setw(15) << release_date
            << setw(10) << duration <<setw(15) << popularity << setw(10) << stream
            << setw(10) << language << setw(10) << explicit_content << endl;
    }
}

int main(){
    // Input the file name containing the playlist data
    const string filename =
        "spotify_songs_dataset_latest.csv";
    // Example filename

    // Step 1: Read the CSV file
    vector<Playlist> playlists = readCsv(filename);

    // Step 2: Display the playlists (first 10 entries for simplicity)
    if (playlists.empty())    {
        cout << "No data to display." << endl;
    } else {
        cout << "Displaying first 10 entries of the playlist:" << endl;
        displayPlaylists(playlists, 10);
    }

    // Step 3: Add any additional operations (sorting, filtering, etc.).
    // Example: Sort by song popularity (descending)
    ranges::sort(playlists, [](const Playlist& a, const Playlist& b) {
        return a.popularity > b.popularity;
    });

    cout << "\nTop 10 popular songs by popularity:" << endl;
    displayPlaylists(playlists, 10);
    system("pause");
    return 0;
}
