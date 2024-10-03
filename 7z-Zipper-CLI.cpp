#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

string getFileOrFolder(bool isFile) {
    string command;
    if (isFile) {
        // AppleScript command to choose a file
        command = "osascript -e 'POSIX path of (choose file)'";
    } else {
        // AppleScript command to choose a folder
        command = "osascript -e 'POSIX path of (choose folder)'";
    }

    // Create a pipe to read the result of the command
    char buffer[128];
    string result = "";
    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe) {
        cerr << "Failed to run osascript command" << endl;
        return "";
    }

    // Read the output of the command
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    // Remove the newline character from the result
    if (!result.empty() && result[result.length() - 1] == '\n') {
        result.erase(result.length() - 1);
    }

    return result;
}

string getDirectoryPath(const string &filePath) {
    size_t pos = filePath.find_last_of("/");
    if (pos != string::npos) {
        return filePath.substr(0, pos + 1); // Include the trailing slash
    }
    return ""; // Return empty if not found (unlikely for a valid path)
}

int main() {
    string source;
    string target;
    string splitSize;

    cout << "Do you want to select a file or folder? (f for file, d for "
            "directory): ";
    char choice;
    cin >> choice;
    cin.ignore(); // Clear the newline from the input buffer

    string selectedPath;
    if (choice == 'f') {
        selectedPath = getFileOrFolder(true); // Select file
        cout << selectedPath << endl;
    } else if (choice == 'd') {
        selectedPath = getFileOrFolder(false); // Select folder
        cout << selectedPath << endl;
    } else {
        cerr << "Invalid choice!" << endl;
        return 0;
    }

    if (!selectedPath.empty()) {
        source = selectedPath;
    } else {
        cerr << "No file or folder selected." << endl;
        return 0;
    }

    string sourceDirectory = getDirectoryPath(source);

    cout << "Enter the name of the Archive (without extension): ";
    getline(cin, target);

    if (target.empty()) {
        cerr << "Target archive name cannot be empty." << endl;
        return 0;
    }

    cout << "Target archive name: " + target << endl;

    cout << "Enter the split size (in MB) if you want to split the archive "
            "(leave empty for no split): ";
    getline(cin, splitSize);

    string zipper;
    if (!splitSize.empty()) {
        zipper = "7z a -v" + splitSize + "m " + sourceDirectory + target +
                 ".7z " + source;
    } else {
        zipper = "7z a " + sourceDirectory + target + ".7z " + source;
    }

    cout << "Executing command: " << zipper << endl;

    int result = system(zipper.c_str());
    if (result != 0) {
        cerr << "Error: Failed to create the archive." << endl;
    }

    return 0;
}
