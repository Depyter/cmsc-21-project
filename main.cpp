#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <set>
#include <sstream>
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
    Tags,
};

list<unique_ptr <class Recipe> > recipe_list;

void append_recipe_to_file(string title);
void delete_line_from_file(string filename , int nth_line);
void printout_field(RecipeField field, const Recipe& recipe);
void saveCategories(const map<string, set<string>>& categories);
void add_tags(map<string, set<string>>& categories, string tag);
bool isTagUsedInRecipes(const string& checktag);
void removeTagFromCategory(map<string, set<string>>& categories, const string& del_tag);
Recipe* find_recipe(const string& title);

string toUpperCase(const string& input) {
    string result = input;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

class Recipe {
    public:
        string title;
        list<string> ingredients;
        list<string> tags;
        list<string> steps;

        // Do this when the user wants to add a new recipe.
        Recipe(map<string, set<string>>& categories) {
            cout << "Enter the title of the recipe:\n";
            getline(cin, this->title);
            cout << "Enter the ingredients of the recipe: (type 'end' to finish)\n";
            string ingredient;
                while (true) {
                getline(cin, ingredient);
                if (ingredient == "end") break;
                this->ingredients.push_back(ingredient);
            }

            cout << "Enter the tags of the recipe (Capitalized): (type 'end' to finish)\n";
            string tag;
            while (true) {
                getline(cin, tag);
                if (tag == "end") break;
                tag = toUpperCase(tag);
                add_tags(categories, tag);
                this->tags.push_back(tag);
            }

            cout << "Enter the steps of the recipe: (type 'end' to finish)\n";
            string step;
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
    for (auto &ingredient : recipe_list.back()->ingredients) {
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

void add_tags(map<string, set<string>>& categories, string tag) {
    bool tagFound = false;
        for (const auto& category : categories) {
            if (category.second.find(tag) != category.second.end()) {
                tagFound = true;
                break;
            }
        }

        if (!tagFound) {
            cout << tag << " not found in existing categories. Please categorize it by selecting the corresponding number." << endl;
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
                    category.second.insert(tag);
                    saveCategories(categories);
                    break;
                }
                i++;
            }
        }
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

        // Initialize the category with an empty set
        categories[category] = set<string>();

        size_t startPos = 0;
        while ((startPos = line.find('/', startPos)) != string::npos) {
            size_t endPos = line.find('/', startPos + 1);
            if (endPos != string::npos) {
                string tag = line.substr(startPos + 1, (endPos - startPos) - 1);
                categories[category].insert(tag);
                startPos = endPos + 1;
            }
        }
    }
}

// Save categories to a file
void saveCategories(const map<string, set<string>>& categories) {
    ofstream file("data/TagCategories.txt");
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    for (const auto& category : categories) {
        file << category.first << ':';
        for (const auto& tag : category.second) {
            file << " /" << tag << "/";
        }
        file << endl;
    }
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

void delete_recipe(map<string, set<string>>& categories) {
    // Delete the recipe from the list and the file.
    string recipeKey, recipes;

    while (true) {
        cout << "Enter the recipe key to delete: ";
        getline(cin, recipeKey);
        if (recipeKey == "back") return;

        recipes = search_recipe(recipeKey);
        if (!recipes.empty()) break;

        cout << "Please input a valid recipe." << endl;
        cout << "Type 'back' to go back to the main menu" << endl;
    }

    // Store it in a vector
    // Parse the string to get the recipe titles
    vector<string> recipeTitles;
    size_t start = 0;
    size_t end = recipes.find('\n');
    while (end != string::npos) {
        recipeTitles.push_back(recipes.substr(start, end - start));
        start = end + 1;
        end = recipes.find('\n', start);
    }
    recipeTitles.push_back(recipes.substr(start)); // Add the last title

    Recipe* recipe = nullptr;
    if (recipeTitles.size() > 1) {
        cout << "Multiple similar recipes are found:" << endl;
        for (size_t i = 0; i < recipeTitles.size(); ++i) {
            cout << i + 1 << ". " << recipeTitles[i] << endl;
        }
        cout << "Enter the number of the recipe you want to look up: ";
        int choice;
        cin >> choice;
        while (choice < 1 || choice > recipeTitles.size()) {
            cout << "Invalid choice. Please enter a number between 1 and " << recipeTitles.size() << ": ";
            cin >> choice;
        }
        string selectedTitle = recipeTitles[choice - 1];
        recipe = find_recipe(selectedTitle);
    } else if (levenshteinDist(recipeTitles[0], recipeKey) != 0){
        cout << "Did you mean " << recipeTitles[0] << "? Yes(y) or No(n): ";
        string response;
        getline(cin, response);
        if (response == "y" || response == "Y") {
            recipe = find_recipe(recipeTitles[0]);
        } else {
            cout << "Returning to main menu." << endl;
            return;
        }
    } else {
        recipe = find_recipe(recipeTitles[0]);
    }

    auto it = recipe_list.begin();
    int line_num = 0;
    bool found = false;

    while (it != recipe_list.end()) {
        if ((*it)->title == recipe->title) {
            found = true;
            delete_line_from_file("data/Names.txt", line_num);
            delete_line_from_file("data/Steps.txt", line_num);
            delete_line_from_file("data/Ingredients.txt", line_num);

            list<string> deleted_tags =(*it)->tags;
            it = recipe_list.erase(it); // erase returns the next iterator
            for (auto& tag: deleted_tags) {
                if (!isTagUsedInRecipes(tag)) {
                    removeTagFromCategory(categories, tag);
                    saveCategories(categories);
                }
            }
            break;
        } else {
            ++it;
            ++line_num;
        }
    }

    if (!found) {
        cout << "Recipe with title " << "'"<< recipe->title << "'"<< " not found." << endl;
    } else {
        cout << "Recipe deleted." << endl;
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

void removeTagFromCategory(map<string, set<string>>& categories, const string& del_tag) {
    // Iterate over each category in the map
    for (auto& category_pair : categories) {
        auto& tags = category_pair.second; // Get the reference to the set of tags for the current category
        
        // Find the tag to remove
        auto it = tags.find(del_tag);
        
        // Check if the tag was found before erasing
        if (it != tags.end()) {
            tags.erase(it); // Erase the tag
            
            // If the set of tags becomes empty after erasing del_tag, erase the entire category from the map
            if (tags.empty()) {
                categories.erase(category_pair.first);
                break;
            }
        }
    }
}

Recipe* find_recipe(const string& title) {
    for (auto& recipe : recipe_list) {
        if (recipe->title == title) {
            return recipe.get();
        }
    }
    return nullptr;
}

void search_filter(const vector<string>& searchTerms = {}) {
    vector<string> terms;
    if (searchTerms.empty()) {
        string term;
        cout << "Enter Filters: (Type 'end' to finish)" << endl;
        while (true) {
            getline(cin, term);
            if (term == "end") break;
            term = toUpperCase(term);
            terms.push_back(term);
        }
    } else {
        terms = searchTerms;
    }

    vector<Recipe*> matchingRecipes;

    for (auto& recipe : recipe_list) {
        bool hasAllFilters = true; // Reset for each recipe

        for (const auto& term : terms) {
            if (find(recipe->tags.begin(), recipe->tags.end(), term) == recipe->tags.end()) {
                hasAllFilters = false;
                break; // No need to check further if one filter is missing
            }
        }

        if (hasAllFilters) {
            matchingRecipes.push_back(recipe.get());
        }
    }

    if (matchingRecipes.empty()) {
        cout << "Recipe with these tags not found." << endl;
    } else if (matchingRecipes.size() == 1) {
        Recipe* recipe = matchingRecipes[0];
        cout << endl << "Recipe: " << recipe->title << endl;
        printout_field(RecipeField::Ingredients, *recipe);
        printout_field(RecipeField::Steps, *recipe);
    } else {
        cout << "Multiple recipes found. Please choose one:" << endl;
        for (size_t i = 0; i < matchingRecipes.size(); ++i) {
            cout << i + 1 << ". " << matchingRecipes[i]->title << endl;
        }
        int choice;
        cout << "Enter the number of the recipe you want to view: ";
        cin >> choice;
        while (choice < 1 || choice > matchingRecipes.size()) {
            cout << "Invalid choice. Please enter a number between 1 and " << matchingRecipes.size() << ": ";
            cin >> choice;
        }
        Recipe* selectedRecipe = matchingRecipes[choice - 1];
        cout << endl << "Recipe: " << selectedRecipe->title << endl;
        printout_field(RecipeField::Ingredients, *selectedRecipe);
        printout_field(RecipeField::Steps, *selectedRecipe);
    }
}

void displayTagsByCategory(const map<string, set<string>>& categories) {
    int categorySelect;
    do {
        cout << "Type the corresponding number: ";
        cin >> categorySelect;
        if (categorySelect == categories.size() + 1) return;
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
        search_filter(searchTerm);
    }
}

void displayAllCategories(const map<string, set<string>>& categories) {
    cout << "Select the corresponding category to display tags of." << endl;
    int i = 1;
    for (const auto& category : categories) {
        cout << i++ << ". " << category.first << endl;
    }
    cout << i << ". Back" << endl;
    displayTagsByCategory(categories);
}

void search_name() {
    string recipeKey, recipes;

    while (true) {
        cout << "Enter the recipe key to search: ";
        getline(cin, recipeKey);
        if (recipeKey == "back") return;

        recipes = search_recipe(recipeKey);
        if (!recipes.empty()) break;

        cout << "Please input a valid recipe." << endl;
        cout << "Type 'back' to go back to the main menu" << endl;
    }

    // Store it in a vector
    // Parse the string to get the recipe titles
    vector<string> recipeTitles;
    size_t start = 0;
    size_t end = recipes.find('\n');
    while (end != string::npos) {
        recipeTitles.push_back(recipes.substr(start, end - start));
        start = end + 1;
        end = recipes.find('\n', start);
    }
    recipeTitles.push_back(recipes.substr(start)); // Add the last title

    Recipe* recipe = nullptr;
    if (recipeTitles.size() > 1) {
        cout << "Multiple similar recipes are found:" << endl;
        for (size_t i = 0; i < recipeTitles.size(); ++i) {
            cout << i + 1 << ". " << recipeTitles[i] << endl;
        }
        cout << "Enter the number of the recipe you want to look up: ";
        int choice;
        cin >> choice;
        while (choice < 1 || choice > recipeTitles.size()) {
            cout << "Invalid choice. Please enter a number between 1 and " << recipeTitles.size() << ": ";
            cin >> choice;
        }
        string selectedTitle = recipeTitles[choice - 1];
        recipe = find_recipe(selectedTitle);
    } else if (levenshteinDist(recipeTitles[0], recipeKey) != 0){
        cout << "Did you mean " << recipeTitles[0] << "? Yes(y) or No(n): ";
        string response;
        getline(cin, response);
        if (response == "y" || response == "Y") {
            recipe = find_recipe(recipeTitles[0]);
        } else {
            cout << "Returning to main menu." << endl;
            return;
        }
    } else {
        recipe = find_recipe(recipeTitles[0]);
    }

    if (recipe) {
        cout << endl << "Recipe: " << recipe->title << endl;
        printout_field(RecipeField::Ingredients, *recipe);
        printout_field(RecipeField::Steps, *recipe);
        printout_field(RecipeField::Tags, *recipe);
    } else {
        cout << "Recipe not found." << endl;
    }
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
                outFile << " $";
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

// Check if a tag is used in any recipe
bool isTagUsedInRecipes(const string& checktag) {
    for (auto &recipe: recipe_list) {
        for (auto &tag: recipe->tags) {
            if (tag == checktag) {
                return true;
            }
        }
    }
    return false;
}

// Function to edit or delete a specific block of information in a recipe
void editRecipeBlock(string title, string blockKey, string newContent, RecipeField field, map<string, set<string>>& categories) {
    // Check if blockKey
    int num = (blockKey != "add") ? stoi(blockKey) - 1 : -1;
    bool updated = false;

    for (auto &recipe : recipe_list) {
        if (recipe->title == title) {
            auto& fieldContainer = (field == RecipeField::Ingredients) ? recipe->ingredients :(field == RecipeField::Steps) ? recipe->steps : recipe->tags;

            if (blockKey == "add") {
                if (field == RecipeField::Tags) {
                    add_tags(categories,newContent);
                }
                fieldContainer.push_back(newContent);
                updated = true;
            } else if (num >= 0 && num < static_cast<int>(fieldContainer.size())) {
                auto it = fieldContainer.begin();
                std::advance(it, num);
                if (newContent == "erase") {
                    string tag;
                    if (field == RecipeField::Tags) tag = *it;
                    fieldContainer.erase(it);

                    if (field == RecipeField::Tags) {
                        if (!isTagUsedInRecipes(tag)) {
                            removeTagFromCategory(categories, tag);
                            saveCategories(categories);
                        }
                    }

                } else {
                    *it = newContent;
                }
                updated = true;
            }
            break;
        }
    }

    if (!updated) {
        cout << "Unable to update recipe contents." << endl;
    } else {
        cout << "Updated recipe contents." << endl;
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
    int count = 0;
    for (const auto& tag : uniqueTags) {
        cout << tag << " ";
        count++;
        if (count % 5 == 0) {
            cout << endl;
        }
    }
    cout << endl;
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
                break;

            default:
                cout << "Invalid field." << endl;
                break;
            }
    cout << endl;
}

void change_recipe_name(string new_name, string recipeKey){
    Recipe* recipe = find_recipe(recipeKey);
    recipe->title = new_name;
    rewrite_file("data/Names.txt", RecipeField::Tags);
    rewrite_file("data/Ingredients.txt", RecipeField::Ingredients);
    rewrite_file("data/Steps.txt", RecipeField::Steps);
}

void edit_recipe(map<string, set<string>>& categories) {
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
        cout << "Which field to edit: (Ingredients || Steps || Tags || Name)" << endl;
        getline(cin, field);
        if (field == "Ingredients" || field == "Steps" || field == "Tags" || field == "Name") break;
        cout << "Invalid field. Please enter 'Ingredients' or 'Steps'." << endl;
    }

    if (field == "Name") {
        cout << "Enter new recipe name:" << endl;
        getline(cin, newContent);
        change_recipe_name(newContent, recipeKey);
        return;
    } else if (field == "Ingredients") {
        recipefield = RecipeField::Ingredients;
    } else if (field == "Steps") {
        recipefield = RecipeField::Steps;
    } else {
        recipefield = RecipeField::Tags;
    }

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
    editRecipeBlock(recipeKey, blockKey, newContent, recipefield, categories);

    if (recipefield == RecipeField::Tags) {
    rewrite_file("data/Names.txt", recipefield);        
    } else {
    rewrite_file("data/" + field + ".txt", recipefield);
    }
}

void add_recipe(map<string, set<string>>& categories) {
    recipe_list.push_back(make_unique<Recipe>(categories));
    append_recipe_to_file(recipe_list.back()->title);
}

void update_categories(map<string, set<string>>& categories) {
    for (auto& recipe : recipe_list) {
        for (auto& tag : recipe->tags) {
            add_tags(categories, tag);
        }
    }
}

void display_all_recipes(){
    for(auto& recipe: recipe_list) {
        cout << "Recipe: " << recipe->title << endl;
        printout_field(RecipeField::Ingredients, *recipe);
        printout_field(RecipeField::Steps, *recipe);
        printout_field(RecipeField::Tags, *recipe);
    }
}

// Function to encrypt the string
std::string encrypt(const string& text, int shift) {
    std::string result = text;
    for (int i = 0; i < text.length(); ++i) {
        if (isupper(text[i])) {
            result[i] = char(int((text[i] - 'A' + shift) % 26 + 'A'));
        } else if (islower(text[i])) {
            result[i] = char(int((text[i] - 'a' + shift) % 26 + 'a'));
        }
    }
    return result;
}

// Function to decrypt the string
std::string decrypt(const string& text, int shift) {
    std::string result = text;
    for (int i = 0; i < text.length(); ++i) {
        if (isupper(text[i])) {
            result[i] = char(int((text[i] - 'A' - shift + 26) % 26 + 'A'));
        } else if (islower(text[i])) {
            result[i] = char(int((text[i] - 'a' - shift + 26) % 26 + 'a'));
        }
    }
    return result;
}

string load_password() {
    ifstream file("data/password.txt");
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return "";
    }

    string line;
    getline(file, line);
    string decrypted = decrypt(line, 3);
    file.close();
    return decrypted;
}

void save_password(const string& password) {
    ofstream file("data/password.txt");
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    string encrypted = encrypt(password, 3);
    file << encrypted;
    file.close();
}

int main() {
    // g++ main.cpp levenshtein.cpp -o main --std c++20
    // Read the recipes from the file and store them in the list.
    //freakyjason
    init_recipes();
    load_tags();
    load_steps();
    load_ingredients();

    string categoryFile = "data/TagCategories.txt";
    map<string, set<string>> categories;
    loadCategories(categoryFile, categories);
    bool run = true;
    bool password = false;

    string correct_password = load_password();
    string entered_password;

    cout << "Enter the password to access the program: ";
    getline(cin, entered_password);

    if (entered_password != correct_password) {
        cout << "Incorrect password. Running program on read only mode." << endl;
    } else {
        cout << "Correct password. Running program on admin mode." << endl;
        password = true;
    }

    if (password) {
        while (run) {
            cout << "1. Create New Recipe" << endl;
            cout << "2. Search" << endl;
            cout << "3. Remove a recipe" << endl;
            cout << "4. Edit recipe" << endl;
            cout << "5. Display All TAGS" << endl;
            cout << "6. Display All Recipes" << endl;
            cout << "7. Change password" << endl;
            cout << "8. Exit" << endl;

            int choice;
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                add_recipe(categories);
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
                        displayAllCategories(categories);
                        break;

                    default:
                        break;
                    }
                }
                break;
            case 3:
                delete_recipe(categories);
                break;
            case 4:
                edit_recipe(categories);
                break;
            case 5:
                display_all_tags();
                break;
            case 6:
                display_all_recipes();
                break;
            case 7:
                cout << "Enter the new password: ";
                getline(cin, entered_password);
                save_password(entered_password);
                break;
            case 8:
                run = false;
                break;
            default:
                break;
            }
        }
    } else {
        while (run) {
            cout << "1. Search" << endl;
            cout << "2. Display All TAGS" << endl;
            cout << "3. Displat All Recipes" << endl;
            cout << "4. Exit" << endl;

            int choice;
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: {
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
                        displayAllCategories(categories);
                        break;

                    default:
                        break;
                    }
                }
                break;
            case 2:
                display_all_tags();
                break;
            case 3:
                display_all_recipes();
                break;
            case 4:
                run = false;
                break;
            default:
                break;
            }
        }
    }
    return 0;
}