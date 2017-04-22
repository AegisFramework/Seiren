#include <stdlib.h>
#include <string.h>

/* Concatenates two given strings and returns the result
 *
 * @param char* str1 - String prefix
 * @param char* str1 - String suffix
 * @return char* - result from joining the two strings
 */
char* concatenate (char *str1, char *str2) {
	char* result = malloc(strlen(str1) + strlen(str2) + 1);
	strcat (result, str1);
	strcat (result, str2);
	return result;
}

/* Check if a string contains a given char
 *
 * @param char* str - String to search in
 * @param char key - Char to search for
 * @return int - true or false
 */
int contains (char *str, char key) {
	return strchr(str, key) != NULL;
}