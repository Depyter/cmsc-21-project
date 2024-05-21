#include <iostream>
#include <list>
#include <string>
using namespace std;

// Create a Recipe class that will hold essential information about a recipe.
// This includes Titles, Ingredients, Keywords, Notes, and Steps.
class Recipe {

    private:
        string title;
        list<string> ingredients;
        string keywords;
        string notes;
        list<string> steps;

    public:
        // Do this when we are loading the recipe from the file.
        Recipe() {
            printf("Recipe constructor\n");
        }

        // Do this when the user wants to add a new recipe.
        Recipe(string title) {
            printf("Recipe constructor\n");
        }

        // Do this when the user wants to delete a recipe.
        ~Recipe() {
            printf("Recipe destructor\n");
        }
};

// The idea is to have an array of Recipe objects that will hold all the recipes.
// The user will be able to add, delete, and modify recipes.
// The user will also be able to search for recipes based on keywords.
// There should be a standard way to display the recipe information and also store the recipe information in the text files.
// To be written in the Readme file.
// Possibly use multiple files to store the information.
// TODO: Use Levenshtein distance to search for recipes based on similar keywords or recipe names.
// Use maps in c++ to store the recipe keywords.

int main() {

    return 0;
}