/*********************************************************************
** Program name:	CS261 - Assignment 5.3 - SpellChecker (spellChecker.c)
** Author:			Artem Slivka
** Date:			11/11/17
** Description:		Implementation of a word spell checker using
**					a map/dictionary.
*********************************************************************/

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashFunction1(const char* key);
int hashFunction2(const char* key);

/*Constants and macros*/
#define NUM_OF_SUGGESTIONS 5
#define MAX_WORD_LENGTH 64
#define MINIMUM3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/*Top list array functions*/
int minimum(HashLink* topList, HashLink* newLink, int* pos);
void updateTopList(HashLink* topList, HashLink* newLink);
void clearTopList(HashLink* topList);
void deleteTopList(HashLink* topList);

/*Distance functions*/
int levDistance(char* str1, char* str2, int rows, int cols, int* distArray);
void updateWordDistances(HashMap* map, char* inputWord, HashLink* topList, int* distArray);

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}


/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char* newWord;
    // FIXME: implement
	if (file == NULL) {
		printf("Error opening file\n");
	}

	/*If file was successfully open for reading*/
	else
	{
		/*Read the file and store words inside
		hash map after*/
		newWord = nextWord(file);

		while (newWord != 0)
		{
			hashMapPut(map, newWord, 1);

			/* Be sure to free the word after you are done with it here. */
			if (newWord != 0)
			{
				free(newWord);
			}
			newWord = nextWord(file);
		}

		/*Just in case the last newWord wasn't freed*/
		if (newWord != 0)
		{
			free(newWord);
			newWord = 0;
		}
	}
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
	char inputBuffer[256];
	/* Allocating top list - list of word suggestions from the dictionary */
	HashLink* topList = malloc(NUM_OF_SUGGESTIONS * sizeof(HashLink));
	for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
	{
		topList[i].key = malloc(sizeof(char) * (MAX_WORD_LENGTH + 1));
		*(topList[i].key) = '\0';
		topList[i].value = 50;
		topList[i].next = 0;
	}

	/*Allocating distance matrix beforehand - it reduces memory usage this way
	Max word length is 128, not including terminator*/
	int* distanceArray = malloc( (2 * (MAX_WORD_LENGTH) + 1) * sizeof(int));
	int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
        
        // Implement the spell checker code here..
		if (strcmp(inputBuffer, "quit") == 0)
		{
			quit = 1;
		}
		else if (hashMapContainsKey(map, inputBuffer))
		{
			printf("Word \"%s\" was found in the dictionary.\n", inputBuffer);
		}
		else
		{
			/*Recalculatating distance for majority of dictionary*/
			updateWordDistances(map, inputBuffer, topList, distanceArray);
			printf("Did you mean?\n");
			for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
			{
				/*printf("%s %d\n", topList[i].key, topList[i].value);*/
				printf("%s \n", topList[i].key);
			}
			clearTopList(topList);
		}     
    }
    //
	/*Freeing memory resources*/
	deleteTopList(topList);
	free(distanceArray);

    hashMapDelete(map);
	getchar();
    return 0;
}

/**
* Function checks values in topList array against the new link. 
* If new link's value is lower than one of values in the topList array,
* the function sets the position at which the link swap needs to happen.
* @param topList
* @param newLink
* @param pos
* @return 1 if new minimum is found, 0 otherwise.
*/
int minimum(HashLink* topList, HashLink* newLink, int* pos)
{
	for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
	{
		/*If new link has a lower distance than one of the
		values in the array, save the position for swapping later	*/
		if (newLink->value < topList[i].value)
		{
			*pos = i;
			return 1;
		}
	}
	return 0;
}

/**
* Function updates the topList array with a new minimum link, 
* at the position given by minimum function.
* @param topList
* @param newLink
*/
void updateTopList(HashLink* topList, HashLink* newLink)
{
	int pos;

	/*If found a new minimum compared to the current topList
	swap the two links*/
	if (minimum(topList, newLink, &pos) == 1)
	{
		strcpy(topList[pos].key, newLink->key);
		topList[pos].value = newLink->value;
	}
}

/**
* Function clears topList array of keys and values, 
* making the array ready for next user input.
* @param topList
*/
void clearTopList(HashLink* topList)
{
	for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
	{
		*(topList[i].key) =  '\0';	/*Clearing the string*/
		topList[i].value = 50;
	}
}

/**
* Function frees all topList array resources,
* @param topList
*/
void deleteTopList(HashLink* topList)
{
	for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
	{
		free(topList[i].key);
		topList[i].key = 0;
	}
	if (topList != 0)
	{
		free(topList);
		topList = 0;
	}
}


/**
* Function calculates Levenshtein distance between two strings
* Returns 0 if the two words are the same.
* @param str1, str2
* @param m, n
* @return distance
* Code source: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
*/
int levDistance(char* str1, char* str2, int rows, int cols, int* distArray)
{
	 int lastdiag, olddiag;

	for (int y = 1; y <= rows; y++)
	{
		distArray[y] = y;
	}

	for (int x = 1; x <= cols; x++)
	{
		distArray[0] = x;
		lastdiag = x - 1;
		for (int y = 1; y <= rows; y++) 
		{
			olddiag = distArray[y];
			distArray[y] = MINIMUM3(distArray[y] + 1, distArray[y - 1] + 1,
				lastdiag + (str1[y - 1] == str2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return(distArray[rows]);
}

/**
* Function updates distance for each word in dictionary to input word.
* @param map
* @param inputWord
* @param topList
* @param distArray
*/
void updateWordDistances(HashMap* map, char* inputWord, HashLink* topList, int* distArray)
{
	HashLink* tempLink;
	unsigned int strLenDiff;
	int inputStrLen, dictStrLen, wordDistance;

	for (int i = 0; i < map->capacity; i++)
	{
		tempLink = map->table[i];

		/*Preventing distance calculation if string lengths
		differ by more than 5 characters. 
		Implemented to reduce time spent on this calculation */
		if (tempLink != 0)
		{
			inputStrLen = strlen(inputWord);
			dictStrLen = strlen(tempLink->key);
			strLenDiff = abs(inputStrLen - dictStrLen);
		}
		while (tempLink != 0 && strLenDiff <= 5)
		{	
			wordDistance = levDistance(inputWord, tempLink->key, inputStrLen, dictStrLen, distArray);

			/*Updating map and list if new minimum distance word found */
			hashMapPut(map, tempLink->key, wordDistance);
			updateTopList(topList, tempLink);

			/*Going to next word in linked list*/
			tempLink = tempLink->next;
		}
	}
}