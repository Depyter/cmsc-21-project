#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>

using namespace std;

// Function to edit or delete a specific block of information in a recipe
string editRecipeBlock(const string &recipe, const string &blockKey, const string &newContent) {
    if (newContent == "erase") {
        regex pattern("/" + blockKey + "/");
        return regex_replace(recipe, pattern, " ");
    } else if (newContent == "add") {
        return recipe + " /" + blockKey + "/";
    }
    else {
        regex pattern("/" + blockKey + "/");
        return regex_replace(recipe, pattern, "/" + newContent + "/");
    }
}

// Function to read recipes from a file
vector<string> readRecipesFromFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Error opening file.");
    }

    string line;
    vector<string> recipes;

    while (getline(file, line)) {
        recipes.push_back(line);
    }
    file.close();
    return recipes;
}

// Function to write recipes to a file
void writeRecipesToFile(const string &filename, const vector<string> &recipes) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Error opening file for writing.");
    }

    for (const string &recipe : recipes) {
        outFile << recipe << endl;
        if (outFile.fail()) {
            throw runtime_error("Error writing to file.");
        }
    }
    outFile.close();
}

// Function to display the contents of a file
void displayFileContents(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Error opening file.");
    }

    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

int main() {
    const string filename = "data/Ingredients.txt";

    try {
        // Read the file and store each line in a vector
        vector<string> recipes = readRecipesFromFile(filename);

        // Print the original contents of the file
        cout << "Original file contents:" << endl;
        for (const string &recipe : recipes) {
            cout << recipe << endl;
        }

        // Ask the user which recipe and block to edit
        string recipeKey, blockKey, newContent;
        cout << "Enter the recipe key to edit: ";
        getline(cin, recipeKey);
        cout << "Enter the block key to edit (or type 'add' to add a new block): ";
        getline(cin, blockKey);
        cout << "Enter the new content for the block (or type 'erase' to delete the block): ";
        getline(cin, newContent);

        // Edit the selected recipe(s)
        bool foundRecipe = false;
        for (string &recipe : recipes) {
            if (recipe.find(recipeKey) != string::npos) {
                cout << "Editing recipe: " << recipe << endl;
                if (blockKey == "add"){
                     recipe = editRecipeBlock(recipe, newContent, "add");
                } else {
                     recipe = editRecipeBlock(recipe, blockKey, newContent);
                }
               foundRecipe = true;
            }
        }

        if (!foundRecipe) {
            cerr << "Recipe key not found." << endl;
            return 1;
        }

        writeRecipesToFile(filename, recipes);

        cout << "Updated file contents:" << endl;
        displayFileContents(filename);

        cout << "The recipe has been updated and the changes are reflected in the file." << endl;
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
