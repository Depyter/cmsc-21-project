#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <set>
#include <sstream>
#include <regex>
#include <cctype>
#include <map>
#include <algorithm>
#include "levenshtein.h"


using namespace std;

// Create a Recipe class that will hold essential information about a recipe.
// This includes Titles, Ingredients, Keywords, Notes, and Steps.

// A lil hack to have less shit to worry about :>
// Declare a global variable 
enum class RecipeField {
    Ingredients,
    Steps,
    Tags
};

list<unique_ptr <class Recipe> > recipe_list;

void append_recipe_to_file(string title);
void delete_line_from_file(string filename , int nth_line);
void printout_field(RecipeField field, const Recipe& recipe);


class Recipe {
    public:

        string title;
        list<string> ingredients;
        list<string> tags;
        list<string> steps;

        // Do this when the user wants to add a new recipe.
        Recipe() {
            cout << "Enter the title of the recipe:\n";
            getline(cin, this->title);
            cout << "Enter the ingredients of the recipe: (type 'end' to finish)\n";

            string ingredient;
            while (cin >> ingredient) {
                if (ingredient == "end") break;
                this->ingredients.push_back(ingredient);
            }

            cout << "Enter the tags of the recipe: (type 'end' to finish)\n";
            string tag;
            while (cin >> tag) {
                if (tag == "end") break;
                this->tags.push_back(tag);
            }

            cout << "Enter the steps of the recipe: (type 'end' to finish)\n";
            string step;
            cin.ignore(); // Ignore any remaining characters in the input buffer
            while (getline(cin, step)) {
                if (step == "end") break;
                this->steps.push_back(step);
            }
        }
        // Do this when we are loading the recipe from the file.
        Recipe(string line) {
            size_t pos = line.find('$'); //finds position $ in the line
            if (pos != string::npos){//prints from 0 to the space before the $ sign;
                string subs = line.substr(0, pos - 1);
                this->title = subs;
            }
        }

        void load_steps(string step) {
            // Load the steps from the file.
            this->steps.push_back(step);
        }

        void load_ingredients(string ingredient) {
            // Load the ingredients from the file.
            this->ingredients.push_back(ingredient);
        }

        void load_tags(string tag) {
            // Load the tags from the file.
            this->tags.push_back(tag);
        }
};

void append_recipe_to_file(string title) {
    // Append the recipe to the file.
    fstream general_information("data/Names.txt", fstream::app);
    if (!general_information) {
        cerr << "Unable to open file";
        return;
    }
    general_information << title << " $";
    for (auto &tag : recipe_list.back()->tags) {
        general_information << " /" << tag << "/";
    }
    general_information << endl;
    general_information.close();

    fstream ingredients("data/Ingredients.txt", fstream::app);
    if (!ingredients) {
        cerr << "Unable to open file";
        return;
    }
    ingredients << title;
    for (auto &ingredient : recipe_list.back()->steps) {
        ingredients << " /" << ingredient << "/";
    }
    ingredients << endl;
    ingredients.close();

    fstream steps("data/Steps.txt", fstream::app);
    if (!steps) {
        cerr << "Unable to open file";
        return;
    }
    steps << title;
    for (auto &step : recipe_list.back()->steps) {
        steps << " /" << step << "/";
    }
    steps << endl;
    steps.close();
}

void init_recipes() {
    // Load the recipe from the file.
    fstream general_information("data/Names.txt", fstream::in | fstream::out);

    if (!general_information) {
    cerr << "Unable to open file. Creating new file..." << endl;
        general_information.open("data/Names.txt", fstream::out);
        if (!general_information) {
            cerr << "Unable to create file" << endl;
            return;
        }
    return;
    }

    string line;
    while(getline(general_information, line)) {
        recipe_list.push_back(make_unique<Recipe>(line));
    }
    general_information.close();
}

void load_data(const string &filename, void (Recipe::*load_func)(string)) {
    fstream file(filename, fstream::in);

    if (!file) {
        cerr << "Unable to open file. Creating new file..." << endl;
        file.open(filename, fstream::out);
        if (!file) {
            cerr << "Unable to create file" << endl;
            return;
        }
    }

    string line;
    auto it = recipe_list.begin();

    while (getline(file, line) && it != recipe_list.end()) {
        size_t startPos = 0;

        while ((startPos = line.find('/', startPos)) != string::npos) {
            size_t endPos = line.find('/', startPos + 1);
            if (endPos != string::npos) {
                string data = line.substr(startPos + 1, endPos - startPos - 1);
                ((*(*it)).*load_func)(data);
                startPos = endPos + 1;
            } else {
                break;
            }
        }
        ++it;
    }
}

void load_steps() {
    load_data("data/Steps.txt", &Recipe::load_steps);
}

void load_ingredients() {
    load_data("data/Ingredients.txt", &Recipe::load_ingredients);
}

void load_tags() {
    load_data("data/Names.txt", &Recipe::load_tags);
}

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

void delete_recipe() {
    // Delete the recipe from the list and the file.
    string title;
    cout << "Enter the title of the recipe you want to delete:\n";
    getline(cin, title);

    auto it = recipe_list.begin();
    int line_num = 0;
    bool found = false;

    while (it != recipe_list.end()) {
        if ((*it)->title == title) {
            found = true;
            delete_line_from_file("data/Names.txt", line_num);
            delete_line_from_file("data/Steps.txt", line_num);
            delete_line_from_file("data/Ingredients.txt", line_num);
            it = recipe_list.erase(it); // erase returns the next iterator
            break;
        } else {
            ++it;
            ++line_num;
        }
    }

    if (!found) {
        cout << "Recipe with title " << "'"<< title << "'"<< " not found." << endl;
    }
}

void delete_line_from_file(const string filename , int nth_line) {
    // Delete the recipe from the file.
    fstream file(filename, fstream::in);
    fstream temp("data/temp.txt", fstream::out);

    if (!file) {
        cerr << "Unable to open file" << endl;
        return;
    }

    string line;

    int current_line = 0;
    while(getline(file, line)) {
        if (current_line != nth_line) {
            temp << line << endl;
        }
        ++current_line;
    }

    file.close();
    temp.close();

    remove(filename.c_str());
    rename("data/temp.txt", filename.c_str());
}

Recipe* find_recipe(const string& title) {
    for (auto& recipe : recipe_list) {
        if (recipe->title == title) {
            return recipe.get();
        }
    }
    return nullptr;
}

string search_recipe(string keyword = "") {
    // If no keyword is provided, prompt the user to enter one
    if (keyword.empty()) {
        cout << "Enter the keyword you want to search for:\n";
        getline(cin, keyword);
    }

    string result = "";
    for (auto &recipe : recipe_list) {
        if (levenshteinDist(recipe->title, keyword) <= 3) {
            if (!result.empty()) {
                result += "\n" + recipe->title;
            } else {
                result += recipe->title;
            }
        }
    }

    if (result.empty()) {
        cout << "Recipe not found." << endl;
    }

    return result;
}

void search_filter() {
    vector<string> searchTerms;
    string term;
    cout << "Enter Filters: (Type 'end' to finish)" << endl;

    while (true) {

            cin >> term;
            if (term == "end") break;
            searchTerms.push_back(term);
        }

    string line;
    bool foundFlag = false;
    bool hasAllFilters = true;
    for (auto &recipe: recipe_list) {
        for (const auto& term : searchTerms) {
            if (find(recipe->tags.begin(), recipe->tags.end(), term) == recipe->steps.end()) {
                hasAllFilters = false;
            }
        }
        if (hasAllFilters) {
            foundFlag = true;
            cout << endl << "Recipe: " << recipe->title << endl;
            printout_field(RecipeField::Ingredients, *recipe);
            printout_field(RecipeField::Steps, *recipe);        
        }
    }
    
    if (!foundFlag) {
        cout << "Recipe with these tags not found." << endl;
    }
}

void search_category(){
}

void search_name() {
    string recipes = search_recipe();
    Recipe* recipe = nullptr;
    int count = std::count(recipes.begin(), recipes.end(), '\n');

    if (count >= 1) {
        cout << "Multiple similar recipes are found:" << endl;
        cout << recipes << endl;
        cout << "Enter the exact title of the recipe you want to look up: ";
        string response;
        getline(cin, response);
        recipe = find_recipe(response);
    } else {
        recipe = find_recipe(recipes);
    }
    cout << endl << "Recipe: " << recipes << endl;
    printout_field(RecipeField::Ingredients, *recipe);
    printout_field(RecipeField::Steps, *recipe);
    printout_field(RecipeField::Tags, *recipe);
}
// Function to write recipes to a file
void rewrite_file(const string filename, RecipeField field) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Error opening file for writing.");
    }

    for (auto it = recipe_list.begin(); it != recipe_list.end(); ++it) {
        outFile << (*it)->title;
        switch (field) {
            case RecipeField::Ingredients:
                for (auto &ingredient : (*it)->ingredients) {
                    outFile << " /" << ingredient << "/";
                }
                break;
            case RecipeField::Steps:
                for (auto &step : (*it)->steps) {
                    outFile << " /" << step << "/";
                }
                break;
            case RecipeField::Tags:
                for (auto &tag : (*it)->tags) {
                    outFile << " /" << tag << "/";
                }
                break;
            
            default:
                break;
        }
        outFile << endl;
    }
    outFile.close();
}

bool isDigits(const std::string &str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Function to edit or delete a specific block of information in a recipe
void editRecipeBlock(string title, string blockKey, string newContent, RecipeField field) {
    // Check if blockKey
    int num = (blockKey != "add") ? stoi(blockKey) - 1 : -1;
    bool updated = false;

    for (auto &recipe : recipe_list) {
        if (recipe->title == title) {
            auto& fieldContainer = (field == RecipeField::Ingredients) ? recipe->ingredients : recipe->steps;

            if (blockKey == "add") {
                fieldContainer.push_back(newContent);
                updated = true;
            } else if (num >= 0 && num < static_cast<int>(fieldContainer.size())) {
                auto it = fieldContainer.begin();
                std::advance(it, num);
                if (newContent == "erase") {
                    fieldContainer.erase(it);
                } else {
                    *it = newContent;
                }
                updated = true;
            }
            break;
        }
    }

    if (!updated) {
        cout << "Unable to update file contents." << endl;
    } else {
        cout << "Updated file contents." << endl;
        cout << "The recipe has been updated and the changes are reflected in the file." << endl;
    }
}

void display_all_tags(){
//Go through each recipe object and 
    set<string> uniqueTags;

    for (auto &recipe: recipe_list) {
        for (auto &tag: recipe->tags) {
            uniqueTags.insert(tag);
        }
    }

    cout << "All Filters:" << endl;
    for (const auto& tag : uniqueTags) {
        cout << tag << endl;
    }
}

void printout_field(RecipeField field, const Recipe& recipe) {
    switch (field) {
            case RecipeField::Ingredients:
                cout << "Ingredients:" << endl;
                for (int count = 1; auto& ingredient : recipe.ingredients) {
                    cout << count++ << ". " << ingredient << endl;    
                }
                break;

            case RecipeField::Steps:
                cout << "Steps:" << endl;
                for (int count = 1; auto& step : recipe.steps) {
                    cout << count++ << ". " << step << endl;
                }
                break;
            
            case RecipeField::Tags:
                cout << "Tags:" << endl;
                for (int count = 1; auto& tag : recipe.tags) {
                    cout << count++ << ". " << tag << endl;
                }

            default:
                cout << "Invalid field." << endl;
                break;
            }
    cout << endl;
}

void edit_recipe() {
    string recipeKey, blockKey, newContent, field, recipes;

    while (true) {
        cout << "Enter the recipe key to edit: ";
        getline(cin, recipeKey);
        if (recipeKey == "back") return;

        recipes = search_recipe(recipeKey);
        if (!recipes.empty()) break;

        cout << "Please input a valid recipe." << endl;
        cout << "Type 'back' to go back to the main menu" << endl;
    }

    int count = std::count(recipes.begin(), recipes.end(), '\n');

    if (count >= 1) {
        cout << "Multiple similar recipes are found:" << endl;
        cout << recipes << endl;
        cout << "Enter the exact title of the recipe you want to edit: ";
        getline(cin, recipeKey);
    } else if (levenshteinDist(recipeKey, recipes) != 0) {
        cout << "Did you mean " << recipes << "? Yes(y) or No(n): ";
        string response;
        getline(cin, response);
        if (response == "y" || response == "Y") {
            recipeKey = recipes;
        } else {
            cout << "Returning to main menu." << endl;
            return;
        }
    } else {
        recipeKey = recipes;
    }

    Recipe* recipe = find_recipe(recipeKey);

    RecipeField recipefield;
    while (true) {
        cout << "Which field to edit: (Ingredients or Steps)" << endl;
        getline(cin, field);
        if (field == "Ingredients" || field == "Steps") break;
        cout << "Invalid field. Please enter 'Ingredients' or 'Steps'." << endl;
    }

    recipefield = (field == "Ingredients") ? RecipeField::Ingredients : RecipeField::Steps;
    recipefield = (field == "Steps") ? RecipeField::Tags : RecipeField::Steps;

    cout << endl << "Recipe: " << recipeKey << endl;
    printout_field(recipefield, *recipe);

    while (true) {
        cout << "Type the number you want to edit or type 'add' to add a new " << field << ":" << endl;
        getline(cin, blockKey);
        if (blockKey == "add" || isDigits(blockKey)) break;
        cout << "Invalid input. Please enter a number or 'add'." << endl;
    }

    cout << "Enter the new content for the block (or type 'erase' to delete the block):" << endl;
    getline(cin, newContent);

    cout << "Editing recipe: " << recipeKey << endl;
    editRecipeBlock(recipeKey, blockKey, newContent, recipefield);

    if (recipefield == RecipeField::Tags) {
    rewrite_file("data/Names.txt", recipefield);        
    } else {
    rewrite_file("data/" + field + ".txt", recipefield);
    }
}

void print_Recipes() {
    // Print the recipes.
    for (auto &recipe : recipe_list) {
        cout << recipe->title << endl;
        cout << "Ingredients:\n";
        for (auto &ingredient : recipe->ingredients) {
            cout << ingredient << endl;
        }
        cout << "BREAKER" << endl;

        cout << "Steps:\n";
        for (auto &step : recipe->steps) {
            cout << step << endl;
        }
        cout << "BREAKER" << endl;

        cout << "Tags:\n";
        for (auto &tag : recipe->tags) {
            cout << tag << endl;
        }
        cout << "BREAKER" << endl;
    }
}

void add_recipe() {
    recipe_list.push_back(make_unique<Recipe>());
    append_recipe_to_file(recipe_list.back()->title);
}

int main() {
    // g++ main.cpp levenshtein.cpp -o main --std c++20
    // Read the recipes from the file and store them in the list.
    init_recipes();
    load_tags();
    load_steps();
    load_ingredients();

    bool run = true;

    while (run) {
        cout << "1. Create New Recipe" << endl;
        cout << "2. Search" << endl;
        cout << "3. Remove a recipe" << endl;
        cout << "4. Edit recipe" << endl;
        cout << "5. Display All TAGS" << endl;
        cout << "6. Exit" << endl;

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            add_recipe();
            break;
        case 2: {
                cout << "1. Search by filter" << endl;
                cout << "2. Search by Name" << endl;
                cout << "3. Search by Category" << endl;

                int inner_choice;
                cin >> inner_choice;
                cin.ignore();

                switch (inner_choice) {
                case 1:
                    search_filter();
                    break;
                
                case 2:
                    search_name();
                    break;
                
                case 3:
                    search_category();
                    break;

                default:
                    break;
                }
            }

            break;
        case 3:
            delete_recipe();
            break;
        case 4:
            edit_recipe();
            break;
        case 5:
            display_all_tags();
            break;
        case 6:
            run = false;
            break;

        default:
            break;
        }
    }
    
    return 0;
}