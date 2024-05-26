#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <map>

/*README
don't let the program be initialized with blank categories. have at least 1 dummy tag when the user starts using the program. the program breaks otherwise
and im too tired to know why
*/

using namespace std;

//loads the categories on startup, INITIALIZER
void loadCategories(const string& categoryFile, map<string, set<string>>& categories){ //in map<string, set<string>>, the string is the category, the set of strings are the tags
    ifstream file(categoryFile); //put in the file for tag categories    
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    string line; //buffer
    while (getline(file, line)){
        istringstream iss(line); //allows you to treat the string as a stream
        string category;
        getline(iss, category, ':'); //reads category name till it finds the colon (it makes everything till the colon the category name)

        string tag; //reads each tag in the category
        size_t startPos = 0;
        while ((startPos = line.find('/', startPos)) != string::npos) { //while it detects a tag it adds it to the categories map.
            size_t endPos = line.find('/', startPos + 1);
            if (endPos != string::npos){
                tag = line.substr(startPos, (endPos - startPos) + 1);
                categories[category].insert(tag);
                startPos = endPos + 1;
            }
        }
    }

    file.close();
}

//rewrites the entire categories tab everytime a new change is created (real)
void saveCategories(const string& categoryFile, const map<string, set<string>>& categories){
    ofstream file(categoryFile);
    if (!file) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    for (const auto& category: categories) { //for every category in categories
        file << category.first << ':'; //Write category name followed by colon (string)
        for (const auto& tag: category.second){ //for every tag in a category (set<string>)
            file << " " << tag; //space before every tag.
        }
        file << endl;
    }

    file.close();
}

//MENU OPTION, WRITES A NEW RECIPE TO BOTH THE CATEGORY AND NAME FILE 
void createNewRecipe(const string& RecipeFile, map<string, set<string>>& categories, const string& categoryFile){
    ofstream nameFile(RecipeFile, ios::app); //opens the file
    if (!nameFile) {
        cerr << "Failed to open the file" << endl;
        return;
    }

    cout << "Enter New Recipe Name:" << endl;
    string recipeName;
    getline(cin, recipeName);

    nameFile << recipeName << " $ "; //appends $ to signify that it will now read tags in the file


    cout << "Enter tags (type 'end' to finish)" << endl;

    string tag;
    while (true) {
        cout << "Enter New Tag:" << endl;
        getline(cin, tag); //reads tag input from user
        if (tag == "end"){
            break; //if it's end the loop breaks
        }

        string fullTag = "/" + tag + "/";
        //finds if the tag already exists in a category
        bool tagFound = false; 
        for (const auto& category: categories){ //checks every category in categories
            if (category.second.find(fullTag) != category.second.end()){ //if the tag is found, then it strikes true
                tagFound = true;
                break;
            }
        } 

        //if the tag is not found
        if (!tagFound){
            cout << fullTag << " not found in existing categories. Please categorize it by selecting the corresponding number." << endl;

            //Display categories
            int i = 1;
            for (const auto& category: categories){
                cout << i++ << ". " << category.first << endl;
            }

            //makes sure the user inputs correctly
            int categorySelect;
            do {
                cout << "Type the corresponding number: ";
                cin >> categorySelect; 
            }while(categorySelect>5 || categorySelect < 1);
            cin.ignore();      

            i = 1;
            for (auto& category : categories){
                if (i==categorySelect){ //loops until i matches the categories select
                    category.second.insert(fullTag); //adds the tag to the chosen categories set of strings
                    saveCategories(categoryFile, categories); //saves the updated category file
                    break;
                }
                i++;
            }
        }
        nameFile << " " << fullTag;
    }
    nameFile << endl;
}

//FILTER BY TAG//

//MAIN FUNCTION FOR FILTERING BY TAGS
void filterByTag(const string& recipeFile, const vector<string>& searchTerm){ //main function for filtering by tag by using a vector file
    ifstream inFile(recipeFile);
    if (!inFile){
        cerr << "File Exploded" << endl;
        return;
    }

    string line;
    bool foundFlag = false;
    while(getline(inFile, line)){ //puts the line of the file into the line
        bool hasAllFilters = true;
        for (const auto& term: searchTerm){
            if (line.find(term) == string::npos){ //if the line does not have all filters (string::npos) the loop breaks
                hasAllFilters = false;
                break;
            }
        }//if it passes the test (doesn't become false) then it is free to
        if (hasAllFilters) {
            size_t pos = line.find('$'); //finds position $ in the line
            if (pos != string::npos){//prints from 0 to the $;
                cout << line.substr(0, pos) << endl;                
            }else{
                cout << line << endl;
            }

            foundFlag = true;
        }
    }

    if (!foundFlag){
        cout << "Recipe with these tags not found." << endl;
    }
    inFile.close();
}

//MENU OPTION 
//EXPLANATION: displayAllTags displays all possible tags without duplicates
void displayAllTags(const string& recipeFile) { //allows the user to display all available tags without duplicates
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "File is not located" << endl;
        return;
    }

    set<string> uniqueTags; //sets up a string container of tags (sets don't contain duplicates)
    string line; //buffer line

    while (getline(inFile, line)){
        size_t startPos = 0; //establishes the starting position of the finder

        while ((startPos = line.find('/', startPos)) != string::npos){ //finds a '/' character and makes sure there is one, if not, the loop terminates
            size_t endPos = line.find('/', startPos + 1); //this initiates the ending position as +1 of the starting position, iterates through the entire tag till it finds /
            if (endPos != string::npos){
                string tag = line.substr(startPos, (endPos - startPos) + 1); //substr gets the starting position and then the length of the substring (endpos-startpos+1 is the length + the two '/' characters)
                uniqueTags.insert(tag); //inserts this tag into the list
                startPos = endPos + 1; //moves the starting position to the otherside of the ending /, making it so that it reads to the right of that
            }
            else{
                break;
            }
        }

    }

    inFile.close();

    cout << "All Filters:" << endl;
    for (const auto& tag: uniqueTags){ //for every tag in unique tags, it prints.
        cout << tag << endl;
    }
}

//MENU OPTION
//EXPLANATION: searchByFilter gets a vector of search terms and passes it to filterByTag.
void searchByFilter(const string& recipeFile){ //gets the list of tags you want to filter by.
    vector<string> searchTerms; //this vector is a temporary list of strings of what you want to search
    string term; //buffer line to put into the vector
    cout << "Enter Filters: (Type 'end' to finish)" << endl;

    while(true){
        cin >> term;
        if (term == "end"){
            break;
        }
        searchTerms.push_back(term); //pushes a new term to the bottom of the "filter list"
    }

    filterByTag(recipeFile, searchTerms);
}

//CATEGORIES//

//FLOWCHART FOR THIS PART(displayTagsByCategory):
//DisplayAllCategories(Displays all categores) -> displayTagsByCategory(Asks user which category to see, and then displays all tags in that category)
//-> Asks user which tag it wants to see the recipes for, and displays all recipes with that tag using filterByTag.

void displayTagsByCategory(const map<string, set<string>>& categories, const string& recipeFile){
    int categorySelect;

    do {
        cout << "Type the corresponding number: ";
        cin >> categorySelect; 
    }while(categorySelect>6 || categorySelect < 1);
    cin.ignore();      

    if (categorySelect == 6){
        return;
    }

    //snippet iterates through the map until it hits the category it wants
    int i = 1, j = 1;
    vector<string> tagsList;
    for (const auto& category: categories){
        if (i==categorySelect){
            cout << category.first << ":" << endl;
            //this snippet here displays all tags in that category.
            for (const auto& tag: category.second){
                cout << j << ". " << tag << endl;
                tagsList.push_back(tag);
                j++;
            }
            cout << j << ". Back" << endl;
            break;
        }
        i++;
    }

    //asks you what you want to access, j will always be one more than the size of the list so it will be a a return function
    int tagSelect;
    do {
        cout << "Type the corresponding number of the tag you want to filter by:";
        cin >> tagSelect; 
    }while(tagSelect>j || tagSelect < 1);
    cin.ignore();  

    if (tagSelect == j){ //back function
        return;
    }

    vector<string> searchTerm = {tagsList[tagSelect-1]};
    filterByTag(recipeFile, searchTerm);
    
}

//TO BE A MENU OPTION
void displayAllCategories(const map<string, set<string>>& categories, const string& recipeFile){
    cout << "Select the corresponding category to display tags of." << endl;
    int i = 1;
    for (const auto& category: categories){ //checks every category in categories
        cout << i << ". " << category.first << endl;
        i++;
    }
    cout << i << ". Back" << endl; 

    displayTagsByCategory(categories, recipeFile);
    
    
}

//REMOVAL OF TAGS FROM CATEGORY 
//this function only removes tags that are no longer used in any recipe. if it detects that it is still used in a recipe, it will ask the user
//if it wants to remove that tag from all recipes too. if no, then no. 

//checks if a tag is used in any recipe.
bool isTagUsedInRecipes(const string& recipeFile, const string& tag) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "File exploded" << endl;
        return false;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.find(tag) != string::npos) {
            return true; // Tag is found in a recipe
        }
    }

    return false; // Tag is not found in any recipe
}

//removes instances of tag from all recipes 
void removeTagFromRecipes(const string& recipeFile, const string& tag) {
    ifstream inFile(recipeFile);
    if (!inFile) {
        cerr << "Failed to open the recipe file" << endl;
        return;
    }

    ofstream outFile("temp.txt"); //writes a temporary file
    if (!outFile) {
        cerr << "Failed to create a temporary file" << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) { //when it finds the tag, it erases it in the line
        size_t pos;
        while ((pos = line.find(tag)) != string::npos) {
            line.erase(pos, tag.length());
        }
        outFile << line << endl; //it writes these lines to the new temp file 
    }

    inFile.close();
    outFile.close();

    // Replace the original file with the updated file
    remove(recipeFile.c_str()); 
    rename("temp.txt", recipeFile.c_str());
}

//MENU OPTION/MAIN FUNCTION
void removeTagFromCategory(map<string, set<string>>& categories, const string& categoryFile, const string& recipeFile) {
    cout << "Tag Removal Selected." << endl;
    int categorySelect;

    // Display category selection menu
    do {
        cout << "Select a category by typing the corresponding number: " << endl;
        int i = 1;
        for (const auto& category : categories) {
            cout << i++ << ". " << category.first << endl;
        }
        cout << "6. Exit" << endl;
        cin >> categorySelect;
    } while (categorySelect > 6 || categorySelect < 1);
    cin.ignore();

    if (categorySelect == 6) {
        return; // Exit if user selects 6
    }

    // Find and display the selected category's tags
    int i = 1, j = 1;
    string selectedCategory;
    vector<string> tagsList; // Vector to store tags for selection
    for (const auto& category : categories) {
        if (i == categorySelect) {
            selectedCategory = category.first;
            cout << category.first << ":" << endl;
            for (const auto& tag : category.second) {
                cout << j << ". " << tag << endl;
                tagsList.push_back(tag); // Add each tag to the vector
                j++;
            }
            cout << j << ". Back" << endl; // Option to go back
            break;
        }
        i++;
    }

    // Allow user to select a tag to remove or go back
    int tagSelect;
    do {
        cout << "Select a tag to remove by typing the corresponding number (or select the last number to go back): ";
        cin >> tagSelect;
    } while (tagSelect > j || tagSelect < 1);
    cin.ignore();

    if (tagSelect == j) {
        return; // Go back if user selects the last number
    }

    // Check if the selected tag is used in any recipe
    string tagToRemove = tagsList[tagSelect - 1]; // No need to modify the tag string
    if (isTagUsedInRecipes(recipeFile, tagToRemove)) {
        cout << "Tag " << tagToRemove << " is still used in recipes. Do you want to remove it from all recipes as well? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();
        if (choice == 'y' || choice == 'Y') {
            removeTagFromRecipes(recipeFile, tagToRemove);
        } else {
            cout << "Tag " << tagToRemove << " was not removed from the category." << endl;
            return;
        }
    }

    // Remove the selected tag from the category
    categories[selectedCategory].erase(tagToRemove);
    saveCategories(categoryFile, categories);
    cout << "Tag " << tagToRemove << " removed from category " << selectedCategory << "." << endl;
}

int main() {
    //literally initializes the categories
    map<string, set<string>> categories;
    string recipesFile = "Names.txt";
    string categoryFile = "TagCategories.txt";
    loadCategories(categoryFile, categories);

    //microcosm menu
    int menuSelect;
    bool back = false;

    while(back==false){
        cout << "1. Create New Recipe" << endl << "2. Display All Available Tags" << endl << "3. Search by Tags" << endl << "4. Display All Categories" << endl;
        cout << "5. Remove a Tag from a Category" << endl << "6. End" << endl;

        cin >> menuSelect;
        cin.ignore();

        switch (menuSelect){

            case 1:
                createNewRecipe(recipesFile, categories, categoryFile);
                break;
            case 2:
                displayAllTags(recipesFile);
                break;        
            case 3:
                searchByFilter(recipesFile);
                break;
            case 4:
                displayAllCategories(categories, recipesFile);
                break;
            case 5:
                removeTagFromCategory(categories, categoryFile, recipesFile);
                break;
            case 6:
                back = true;
                break;
            default:
                cout << "Invalid input.";
        }        
    }

    return 0;
}