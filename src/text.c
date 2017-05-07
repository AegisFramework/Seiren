#include <stdlib.h>
#include <string.h>

/* Concatenates two given strings and returns the result
 *
 * @param char* str1 - String prefix
 * @param char* str1 - String suffix
 * @return char* - result from joining the two strings
 */
char* concatenate (char *str1, char *str2) {
	char* result = malloc(strlen(str1) + strlen(str2));
	result[0]='\0';
	strncat (result, str1, strlen(str1));
	strncat (result, str2, strlen(str2));
	return result;
}

/* Check if a string contains a given char
 *
 * @param char* str - String to search in
 * @param char key - Char to search for
 * @return int - true or false
 */
int contains (char *str, char *key) {
	return strstr(str, key) != NULL;
}

char *split (char *delimiter, char *string) {
	return strtok (delimiter, string);
}

int equal (const char *str1, const char *str2) {
	return strncmp(str1, str2, strlen(str1)) == 0;
}