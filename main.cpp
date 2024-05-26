#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <memory>
#include "levenshtein.h"

using namespace std;

// Create a Recipe class that will hold essential information about a recipe.
// This includes Titles, Ingredients, Keywords, Notes, and Steps.

// A lil hack to have less shit to worry about :>
// Declare a global variable 
list<unique_ptr <class Recipe> > recipe_list;

void append_recipe_to_file(string title);
void delete_line_from_file(string filename , int nth_line);

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
    cerr << "Unable to open file";
    return;
    }

    string line;
    while(getline(general_information, line)) {
        recipe_list.push_back(make_unique<Recipe>(line));
    }
    general_information.close();
}

void load_data(const string& filename, void (Recipe::*load_func)(string)) {
    fstream file(filename, fstream::in);

    if (!file) {
        cerr << "Unable to open file";
        return;
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

void search_recipe() {
    // Search for the recipe based on the keyword.
    string keyword;
    cout << "Enter the keyword you want to search for:\n";
    getline(cin, keyword);

    for (auto &recipe : recipe_list) {
        if (levenshteinDist(recipe->title, keyword) <= 3) {
            cout << recipe->title << endl;
        }
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

int main() {
    // g++ main.cpp levenshtein.cpp -o main --std c++20
    // Read the recipes from the file and store them in the list.
    init_recipes();
    load_tags();
    load_steps();
    load_ingredients();
    print_Recipes();
    search_recipe();
    return 0;
}