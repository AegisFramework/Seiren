/* text.c
 *
 * Copyright (C) 2017 Aegis Framework
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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