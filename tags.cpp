#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

// Load categories from a file
void loadCategories(const string& categoryFile, map<string, set<string>>& categories) {
    ifstream file(categoryFile);
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string category;
        getline(iss, category, ':');

        size_t startPos = 0;
        while ((startPos = line.find('/', startPos)) != string::npos) {
            size_t endPos = line.find('/', startPos + 1);
            if (endPos != string::npos) {
                string tag = line.substr(startPos, (endPos - startPos) + 1);
                categories[category].insert(tag);
                startPos = endPos + 1;
            }
        }
    }
}

// Save categories to a file
void saveCategories(const string& categoryFile, const map<string, set<string>>& categories) {
    ofstream file(categoryFile);
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    for (const auto& category : categories) {
        file << category.first << ':';
        for (const auto& tag : category.second) {
            file << " " << tag;
        }
        file << endl;
    }
}

// Create a new recipe
void createNewRecipe(const string& recipeFile, map<string, set<string>>& categories, const string& categoryFile) {
    ofstream nameFile(recipeFile, ios::app);
    if (!nameFile) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    cout << "Enter New Recipe Name:" << endl;
    string recipeName;
    getline(cin, recipeName);
    nameFile << recipeName << " $ ";

    cout << "Enter tags (type 'end' to finish)" << endl;

    string tag;
    while (true) {
        cout << "Enter New Tag:" << endl;
        getline(cin, tag);
        if (tag == "end") break;

        string fullTag = "/" + tag + "/";
        bool tagFound = false;
        for (const auto& category : categories) {
            if (category.second.find(fullTag) != category.second.end()) {
                tagFound = true;
                break;
            }
        }

        if (!tagFound) {
            cout << fullTag << " not found in existing categories. Please categorize it by selecting the corresponding number." << endl;
            int i = 1;
            for (const auto& category : categories) {
                cout << i++ << ". " << category.first << endl;
            }

            int categorySelect;
            do {
                cout << "Type the corresponding number: ";
                cin >> categorySelect;
            } while (categorySelect < 1 || categorySelect >= categories.size() + 1);
            cin.ignore();

            i = 1;
            for (auto& category : categories) {
                if (i == categorySelect) {
                    category.second.insert(fullTag);
                    saveCategories(categoryFile, categories);
                    break;
                }
                i++;
            }
        }
        nameFile << " " << fullTag;
    }
    nameFile << endl;
}

// Filter recipes by tags
void filterByTag(const string& recipeFile, const vector<string>& searchTerm) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "File Exploded" << endl;
        return;
    }

    string line;
    bool foundFlag = false;
    while (getline(inFile, line)) {
        bool hasAllFilters = true;
        for (const auto& term : searchTerm) {
            if (line.find(term) == string::npos) {
                hasAllFilters = false;
                break;
            }
        }
        if (hasAllFilters) {
            size_t pos = line.find('$');
            if (pos != string::npos) {
                cout << line.substr(0, pos) << endl;
            } else {
                cout << line << endl;
            }
            foundFlag = true;
        }
    }

    if (!foundFlag) {
        cout << "Recipe with these tags not found." << endl;
    }
}

// Display all tags without duplicates
void displayAllTags(const string& recipeFile) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "File is not located" << endl;
        return;
    }

    set<string> uniqueTags;
    string line;

    while (getline(inFile, line)) {
        size_t startPos = 0;
        while ((startPos = line.find('/', startPos)) != string::npos) {
            size_t endPos = line.find('/', startPos + 1);
            if (endPos != string::npos) {
                string tag = line.substr(startPos, (endPos - startPos) + 1);
                uniqueTags.insert(tag);
                startPos = endPos + 1;
            } else {
                break;
            }
        }
    }

    cout << "All Filters:" << endl;
    for (const auto& tag : uniqueTags) {
        cout << tag << endl;
    }
}

// Search by filters
void searchByFilter(const string& recipeFile) {
    vector<string> searchTerms;
    string term;
    cout << "Enter Filters: (Type 'end' to finish)" << endl;

    while (true) {
        cin >> term;
        if (term == "end") break;
        searchTerms.push_back(term);
    }

    filterByTag(recipeFile, searchTerms);
}

// Display tags by category
void displayTagsByCategory(const map<string, set<string>>& categories, const string& recipeFile) {
    int categorySelect;
    do {
        cout << "Type the corresponding number: ";
        cin >> categorySelect;
    } while (categorySelect < 1 || categorySelect > categories.size());
    cin.ignore();

    int i = 1;
    vector<string> tagsList;
    for (const auto& category : categories) {
        if (i == categorySelect) {
            cout << category.first << ":" << endl;
            int j = 1;
            for (const auto& tag : category.second) {
                cout << j++ << ". " << tag << endl;
                tagsList.push_back(tag);
            }
            cout << j << ". Back" << endl;
            break;
        }
        i++;
    }

    int tagSelect;
    do {
        cout << "Type the corresponding number of the tag you want to filter by: ";
        cin >> tagSelect;
    } while (tagSelect < 1 || tagSelect > tagsList.size() + 1);
    cin.ignore();

    if (tagSelect != tagsList.size() + 1) {
        vector<string> searchTerm = {tagsList[tagSelect - 1]};
        filterByTag(recipeFile, searchTerm);
    }
}

// Display all categories
void displayAllCategories(const map<string, set<string>>& categories, const string& recipeFile) {
    cout << "Select the corresponding category to display tags of." << endl;
    int i = 1;
    for (const auto& category : categories) {
        cout << i++ << ". " << category.first << endl;
    }
    cout << i << ". Back" << endl;

    displayTagsByCategory(categories, recipeFile);
}

// Check if a tag is used in any recipe
bool isTagUsedInRecipes(const string& recipeFile, const string& tag) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "File exploded" << endl;
        return false;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.find(tag) != string::npos) {
            return true;
        }
    }

    return false;
}

// Remove a tag from all recipes
void removeTagFromRecipes(const string& recipeFile, const string& tag) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "Failed to open the recipe file" << endl;
        return;
    }

    ofstream outFile("temp.txt");
    if (!outFile) {
        cerr << "Failed to create a temporary file" << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) {
        size_t pos;
        while ((pos = line.find(tag)) != string::npos) {
            line.erase(pos, tag.length());
        }
        outFile << line << endl;
    }

    remove(recipeFile.c_str());
    rename("temp.txt", recipeFile.c_str());
}

// Remove a tag from a category
void removeTagFromCategory(map<string, set<string>>& categories, const string& category, const string& tag) {
    categories[category].erase(tag);
    if (categories[category].empty()) {
        categories.erase(category);
    }
}

// Remove a tag completely
void removeTag(map<string, set<string>>& categories, const string& categoryFile, const string& recipeFile) {
    displayAllCategories(categories, recipeFile);

    cout << "Enter the number of the tag to remove: ";
    int tagSelect;
    cin >> tagSelect;
    cin.ignore();

    string tag;
    auto it = categories.begin();
    for (int i = 1; i < tagSelect; ++i) {
        ++it;
    }
    if (it != categories.end()) {
        cout << "Removing tag: " << it->first << endl;
        tag = it->first;

        if (!isTagUsedInRecipes(recipeFile, tag)) {
            removeTagFromCategory(categories, it->first, tag);
            saveCategories(categoryFile, categories);
            removeTagFromRecipes(recipeFile, tag);
        } else {
            cout << "Cannot remove tag because it is used in recipes." << endl;
        }
    } else {
        cout << "Invalid tag selection." << endl;
    }
}

// Main menu
int main() {
    string recipeFile = "data/Names.txt";
    string categoryFile = "data/TagCategories.txt";

    map<string, set<string>> categories;
    loadCategories(categoryFile, categories);

    while (true) {
        cout << "1. Create New Recipe" << endl;
        cout << "2. Search by filter" << endl;
        cout << "3. Display all filters" << endl;
        cout << "4. Display filters by category" << endl;
        cout << "5. Remove Tag" << endl;
        cout << "6. Exit" << endl;

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                createNewRecipe(recipeFile, categories, categoryFile);
                break;
            case 2:
                searchByFilter(recipeFile);
                break;
            case 3:
                displayAllTags(recipeFile);
                break;
            case 4:
                displayAllCategories(categories, recipeFile);
                break;
            case 5:
                removeTag(categories, categoryFile, recipeFile);
                break;
            case 6:
                return 0;
            default:
                cout << "Invalid choice" << endl;
        }
    }

    return 0;
}
