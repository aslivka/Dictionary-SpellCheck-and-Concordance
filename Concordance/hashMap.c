/*********************************************************************
** Program name:	CS261 - Assignment 5.1-Hash Map (hashMap.c)
** Author:			Artem Slivka
** Date:			11/9/17
** Description:		Implementation of a dictionary using a Hash Map
**					with Linked Lists as array rows.
*********************************************************************/

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
	HashLink* currentLink;
	HashLink* prevLink;

	/*Deleting table*/
	for (int i = 0; i < map->capacity; i++)
	{
			/*Deleting the list starting from first element*/
			currentLink = map->table[i];

			while (currentLink != 0)
			{
				prevLink = currentLink;
				currentLink = currentLink->next;
				hashLinkDelete(prevLink);
				prevLink = 0;
				map->size--;
			}
	}
	free(map->table);
	map->table = 0;	
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
	int hashIndex = HASH_FUNCTION(key) % map->capacity;
	if (hashIndex < 0) hashIndex += map->capacity;

	HashLink* currentLink = map->table[hashIndex];
	int* currentValue;

	/*If found the key in the map, update the value field*/
	if (hashMapContainsKey(map, key) == 1)
	{
		while (currentLink != 0)
		{
			if (strcmp(currentLink->key, key) == 0)
			{
				currentValue = &(currentLink->value);	
				return currentValue;
			}

			currentLink = currentLink->next;
		}
	}

	/*Otherwise, set a new link as beginning of the bucket's linked list*/
	return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity. After allocating the new table, all of the links need to be
 * rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
	assert(map != 0);
	assert(capacity > 0);
	int oldCapacity = map->capacity;
	int hashIndex;
	HashLink** oldTable = map->table;
	HashLink* currentLink;
	HashLink* prevLink;

	/*Resizing the new table to double of original size*/
	hashMapInit(map, capacity);

	/*Copying elements over from old table */
	for (int i = 0; i < oldCapacity; i++)
	{
			/* Traversing the linked list and 
			allocating old values to the new table	*/
			currentLink = oldTable[i];

			while (currentLink != 0)
			{
				/*Hashing elements to new table index*/
				hashIndex = HASH_FUNCTION(currentLink->key) % map->capacity;
				if (hashIndex < 0) hashIndex += map->capacity;

				/*Storing the new link in the list*/
				map->table[hashIndex] = hashLinkNew(currentLink->key, currentLink->value, map->table[hashIndex]);
				map->size++;

				/*Continuing possibly to next link in the linked list*/
				currentLink = currentLink->next;
			}
	}

	/*Deleting old table  */
	for (int i = 0; i < oldCapacity; i++)
	{
			/*Deleting the list starting from first element*/
			currentLink = oldTable[i];

			while (currentLink != 0)
			{
				prevLink = currentLink;
				currentLink = currentLink->next;
				hashLinkDelete(prevLink);
				prevLink = 0;
			}
	}
	free(oldTable);
	oldTable = 0;
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
	int hashIndex = HASH_FUNCTION(key) % map->capacity;
	if (hashIndex < 0) hashIndex += map->capacity;
	HashLink* currentLink = map->table[hashIndex];

	/*If found the key in the map, update the value field*/
	if (hashMapContainsKey(map, key) == 1)
	{
		while (currentLink != 0)
		{
			if (strcmp(currentLink->key, key) == 0)
			{
				currentLink->value = value;
				return;
			}
			currentLink = currentLink->next;
		}
	}
	/*Otherwise, set a new link as beginning of the bucket's linked list*/
	else
	{
		map->table[hashIndex] = hashLinkNew(key, value, currentLink);
		map->size++;

		/*Resizing map if necessary*/
		float loadFactor = hashMapTableLoad(map);
		if (loadFactor >= MAX_TABLE_LOAD)
		{
			resizeTable(map, 2 * map->capacity);
		}
	}	
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
	int hashIndex = HASH_FUNCTION(key) % map->capacity;
	if (hashIndex < 0) hashIndex += map->capacity;

	HashLink* currentLink = map->table[hashIndex];
	HashLink* prevLink = currentLink;

	/*Searching for the element first */
	while (currentLink != 0)
	{
		if (strcmp(currentLink->key, key) == 0)
		{
			/*Special case - when found element is first in list
			there is no previous link to switch from */
			if (currentLink == prevLink)
			{
				map->table[hashIndex] = currentLink->next;
			}

			/*Regular case - when multiple elements in the list*/
			else
			{
				prevLink->next = currentLink->next;	
			}
			hashLinkDelete(currentLink);
			map->size--;
			return;
		}
		prevLink = currentLink;
		currentLink = currentLink->next;
	}
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
	int hashIndex = HASH_FUNCTION(key) % map->capacity;
	if (hashIndex < 0) hashIndex += map->capacity;
	HashLink* tempLink = map->table[hashIndex];

	while (tempLink != 0)
	{
		if (strcmp(tempLink->key, key) == 0)
		{
			return 1;
		}
		tempLink = tempLink->next;
	}
	
	/*key not found in linked list, so returning 0*/
	return 0; 	 
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
	int sum = 0;
	for (int i = 0; i < map->capacity; i++)
	{
		if (map->table[i] == 0)
		{
			sum++;
		}
	}
    return sum;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
	return ((float)(map->size) / map->capacity);
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink* link = map->table[i];
        if (link != NULL)
        {
            printf("\nBucket %i ->", i);
            while (link != NULL)
            {
                printf(" (%s, %d) ->", link->key, link->value);
                link = link->next;
            }
        }
    }
    printf("\n");
}