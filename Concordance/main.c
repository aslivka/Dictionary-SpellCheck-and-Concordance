/*********************************************************************
** Program name:	CS261 - Assignment 5.2-Concordance (main.c)
** Author:			Artem Slivka
** Date:			11/10/17
** Description:		Implementation of a word frequency counter 
**					using a map/dictionary.
*********************************************************************/

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

//int hashFunction1(const char* key);

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
* Prints frequencies of each word in a given text file. 
* @param map
*/
void printWordFrequency(HashMap* map)
{
	printf("Printing word frequency count\n");
	for (int i = 0; i < map->capacity; i++)
	{
		HashLink* tempLink = map->table[i];
		while (tempLink != 0)
		{
			printf(" %s:  %d\n", tempLink->key, tempLink->value);
			tempLink = tempLink->next;
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
    const char* fileName = "input1.txt";
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);
    
    clock_t timer = clock();
    
    HashMap* map = hashMapNew(10);
    
    // --- Concordance code begins here ---
	/*Opening file*/
	FILE* filePointer;
	filePointer = fopen(fileName, "r");

	if (filePointer == NULL) {
		fprintf(stderr, "Error opening %s\n", fileName);
	}

	/*If file was open successfully, read the file, 
	count word frequency, then close the file.	*/
	else
	{
		char* newWord = nextWord(filePointer);
		while (newWord != 0)
		{
			int* wordFrequency;

			/*If found a key in the map, increment the frequency*/
			if (hashMapContainsKey(map, newWord) == 1)
			{
				wordFrequency = hashMapGet(map, newWord);
				/*Incrementing the counter since word is already in the map*/
				(*(wordFrequency))++;
			}

			/*No key found in map, so adding a new word link */
			else
			{
				hashMapPut(map, newWord, 1);
			}

			/* Be sure to free the word after you are done with it here. */
			if (newWord != 0)
			{
				free(newWord);
			}
			/*Receiving next word from the file stream */
			newWord = nextWord(filePointer);
		}
		
		if (newWord != 0)
		{
			free(newWord);
			newWord = 0;
		}

		/* Closing the file */
		fclose(filePointer);
	}

	/*Printing word frequency counts for each element in the map*/
	printWordFrequency(map);
    // --- Concordance code ends here ---
    
    hashMapPrint(map);
    
    timer = clock() - timer;
    printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
    printf("Number of links: %d\n", hashMapSize(map));
    printf("Number of buckets: %d\n", hashMapCapacity(map));
    printf("Table load: %f\n", hashMapTableLoad(map));
    
    hashMapDelete(map);
	getchar();
    return 0;
}