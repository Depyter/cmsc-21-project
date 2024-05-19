# RECIPEASY — CMSC 21 PROJECT

###  Main concept: One key for multiple files

We will use a multiple files to store specific aspects of the recipe.  
Ex. One file will be used for ingredients, another will be for the specific steps, etc..  
We assign a key to a value which is used to determine which file and which line will be accessed.  
We will be using the [unordered map](https://www.geeksforgeeks.org/unordered_map-in-cpp-stl/) in c++ to assign either integer keys or string keys.  

| Files |
| General information | Ingredients | Steps | Notes | 
| Important recipe details like cook time etc. used for the search | Ingredients | Steps | Extra bits of info for the recipe |

How it is stored in the program is up to who is in charge.

1. ### How to write to each file

Each line holds all the  information of a certain recipe.  
Put the key at the start of the line.

Ex. `Abodo69 5 6 \Filipino For boyfie\ 30`  
Ex. `Chokequeen \Steps: 5\ \Ingredients: 5\ \Cusine: Chinesey Choke me queen\ \Cook time: 30 minutes\`  

Ex. `Abodo69 \Chicken: 600g\ \Soy sauce: 250ml\ \Garlic: 3tbs\ \White vinegar: 120ml\`
Ex. `Abodo69 \First....\ \After you do that....\ \Then....\ \Add the....\ \Serve with...\`

The order does not matter, as long the parsing and writing is uniform.  
For now, \ denotes the start of a string/block of information and another \ as the end.  
> If there are suggestions to what character or way we can use please tell the group.  

2. ### Editing existing recipes

To edit an existing recipe, we look at the block of information we want to change, then insert the change.  

Ex. `Recipe123 \First\ \Second\ \Third\`

Let's say I wanted to lengthen the second step, we would first look for the 'second' block.  
After which, we either insert or completely remove everything inside that block.

Ex. `Recipe123 \First\ \I inserted this\ \Third\`

3. ### Recipe class

The recipe class should store all the information that is from the files and should store the key for the undordered map.  
The constructor for this class should handle the creation of a new recipe.  
It should ask the user for recipe steps, ingredients, keywords, etc..  

