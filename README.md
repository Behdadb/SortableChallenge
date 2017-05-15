# SortableChallenge

## Running command

For compiling the code c++11 is needed (command: "g++ -std=c++11 main.cpp")
Then ("a > result.txt")

## Method

Program extracts keyword from all the producst into a dictionary of strings after some filters. Then each product can be represented as a bitmask of the keywords in them (about 12 64bit variables are used). Then each listing will be tokenized and the strings from it which already exist in the dictionary will remain. The remaining words will be represented in a same fasion as producst and will be matched with all the products of the same manufacturer. If a product matches with the listing with a high similarity and there exist no other product to be matched with the same value, the listing will be assigned to the most similar product.
