#include <iostream>
using namespace std;

// Create a Recipe class that will hold essential information about a recipe.
// This includes Titles, Ingredients, Keywords, Notes, and Steps.
class Recipe {
    public:
        Recipe() {
            printf("Recipe constructor\n");
        }
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