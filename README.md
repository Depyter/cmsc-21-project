# RECIPEASY — CMSC 21 PROJECT

###  Main concept: One key for multiple files
We will use a multiple files to store specific aspects of the recipe.  
Ex. One file will be used for ingredients, another will be for the specific steps, etc..  
We assign a key to a value which is used to determine which file and which line will be accessed.  
We will be using the unordered map in c++ to assign either integer keys or string keys.  
More details will be written later.  
### How to write to each file
#### Important Recipe details
This file will have the important recipe details which will be used to in the search function.
These details include the Cuisine type. Number of steps, as determined by the group.  
###### How to write to this file
Each line holds all the information of a certain recipe.  
Put the key at the start of the line.
Could be of the form "Key (int)num_of_steps (int)num_of_ingredients (string)cusine_type etc. (string)Keyword/s"  

`Ex. Abodo69 5 6 Filipino For boyfie`  
`Ex. Chokequeen Steps: 5 Ingredients: 5 Cusine: Chinesey Choke me queen`  
Latter is more annoying to deal with.
#### Ingredients 
#### Steps 
#### Comments 
### Editing recipes
Editing recipes 
