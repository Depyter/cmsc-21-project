# RECIPEASY — CMSC 21 PROJECT

###  Main concept: One key for multiple files

We will use a multiple files to store specific aspects of the recipe.  
Ex. One file will be used for ingredients, another will be for the specific steps, etc..  
We assign a key to a value which is used to determine which file and which line will be accessed.  
We will be using the list in c++ to store all the recipes for the program.

| Files |
| General information | Ingredients | Steps | Notes | 
| Important recipe details like cook time etc. used for the search | Ingredients | Steps | Extra bits of info for the recipe |

How it is stored in the program is up to who is in charge.

### 1. How to write to each file

Each line holds all the  information of a certain recipe.  
Put the key at the start of the line.

Ex. `\Abodo69\ 5 6 \Filipino For boyfie\ 30`  
Ex. `\Chokequeen\ \Steps: 5\ \Ingredients: 5\ \Cusine: Chinesey Choke me queen\ \Cook time: 30 minutes\`    

Ex. `\Abodo69\ \Chicken: 600g\ \Soy sauce: 250ml\ \Garlic: 3tbs\ \White vinegar: 120ml\`  
Ex. `\Abodo69\ \First....\ \After you do that....\ \Then....\ \Add the....\ \Serve with...\`  

The order does not matter, as long the parsing and writing is uniform.  
For now, \ denotes the start of a string/block of information and another \ as the end.  
**If there are suggestions to what character or way we can use please tell the group.** 

### 2. Editing existing recipes

To edit an existing recipe, we look at the block of information we want to change, then insert the change.  

Ex. `Recipe123 \First\ \Second\ \Third\`

Let's say I wanted to lengthen the second step, we would first look for the 'second' block.  
After which, we either insert or completely remove everything inside that block.

Ex. `Recipe123 \First\ \I inserted this\ \Third\`

### 3. Specific tasks to complete

1. Create the recipe object using classes and [constructor](https://www.geeksforgeeks.org/constructors-c/) 

The recipe object holds all the information about the recipe,  
like the recipe name, cusine, steps, etc.. 

`General flow`

Also create the constructor for the recipe class. Use function overloading to differentiate  
when the user is adding a new recipe vs loading in a recipe from the files.
Perhaps have an argument that takes in the user inputted recipe name.

Inside the constructor, you should ask the user for the recipe information.
Which order you ask the user is up to you.  
While loops for the steps and ingredients should work, steps and ingredients should be stored in a list. 
*Do read up on lists in c++* 

After the user has inputted the recipe information, write it to the file. 
*Use file manipulation to write to the file.*


Also handle the deletion of the recipe.

```
#include <iostream>
#include <list>
#include <string>
using namespace std;

// Create a Recipe class that will hold essential information about a recipe.
// This includes Titles, Ingredients, Keywords, Notes, and Steps.
class Recipe {

    private:
        list<string> ingredients;
        string keywords;
        string notes;
        list<string> steps;

    public:
        string title;
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
```

*DO use unique pointers if applicable in c++ if you need to use pointers*

2. Function to load in each recipe

Continuation of constructor above.
We need a function to load in each recipe in the file.

`General flow:`

Open the file general information file, since each line represents one recipe,   
the starting string should be the name of the recipe.  

Create a recipe object for that recipe.  

Also load in the other files and add the information to the recipe object.  
Recipe object should store the the information, like steps, ingredients.  
Also create the c++ list to store each recipe objects.  

```
// Read each line in the general information file
// Initialize a recipe object if not EOF (End of file)
// For each line, get the first string which is the recipe name
// Set the recipe name variable to the recipe name in the file
// Do similar things for the other information in file

// For the other files, check find the recipe name in that file is the same with a recipe object  
// I.E. check if that recipe object exists
// Otherwise, print out that there exists an object that does not have a recipe object
```

*DO use string manipulation*
*DO use file manipulation like fread etc.*
*DO use memory allocation in c++ like 'new'*
*DO use unique pointers if applicable in c++ if you need to use pointers*

3. Edit recipe

After loading in the recipes in the files, the user should be able to 
edit recipes.

It should follow roughly the flow of editing recipe above.

*DO use string manipulation*
*DO use file manipulation like fread etc.*

4. Menu for user

Make the menu for the user.

5. Filter by keyword

User should be able to search by certain keywords, like cuisine, cook time, and also search by notes  
and unique keywords/tags. 

6. Search 

Copy and paste fuzzy search and implement it in our program.