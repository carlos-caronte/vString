/*
 * File:   vstring.c
 * Author: jcr
 *
 * Created on 11 October, 2017
 */
 /*
 * Copyright (C) 2017 Juan Carlos Rey <carlos.caronte@gmail.com>
 *
 * This file is part of vString
 *
 * vString is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vString is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with vString.  If not, see <http://www.gnu.org/licenses/>.
 */


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fnmatch.h>
#include"vstring.h"

/***************************************************************
 *
 *                                  Constructor  -  Destructor
 *
 * *************************************************************/

/**
 * @brief                       Constructor. There is no data in the vstring
 *                                  object. After 'New', you have to use 'From'
 * @param capacity      Maximum number of characters in the object
 * @returns                   Returns a vstring object with its features
 *                                  by default. All the objects are safes: their capacity
 *                                  is inmutable
 *
 */
vstring_t * vstring_New(int capacity) {

    // Capacity has to be greater than Zero. Otherwise it throws the error
    // invalid arguments.
    //
     s_assert(capacity > 0, S_ERR_INVALID_ARGUMENT);

    // We ask for to Memory Manager contigous memory in the HEAP
    //
     vstring_t *s = malloc(sizeof(vstring_t));

     // If memory error, exit,,,
     //
     s_assert(s, S_ERR_ALLOCATE_MEMORY);

     s->len = 0;
     s->capacity = capacity;
     s->ele_size = sizeof(char);
     s->safe = S_SAFE;

     s->data = calloc(1, s->capacity * s->ele_size + 1);

     s_assert(s->data, S_ERR_ALLOCATE_MEMORY);

    return s;
}


/**
 * @brief                       Constructor. We append the data array with 'str'
 * @param str               String to append the data in the vstring object
 * @param capacity      Maximum number of characters in the object
 * @returns                   Returns a vstring object with its features
 *                                  by default. All the objects are safes: their capacity
 *                                  is inmutable
 */
vstring_t * vstring_with_Capacity(char * str, int capacity) {

    vstring_t *s = vstring_New(capacity);
    /*
     * The function vstring_From appends the data array or throws an
     * ERR_UNSAFE_CAPACITY, that is, the object is SAFE and the length
     * of 'str' is greater than capacity
     */
    s_stat status = vstring_From(s, str);
    char *pointer;
    switch(status) {
        case  S_OK:
            return s;
        case S_ERR_UNSAFE_CAPACITY:

            // Well, an error has occurried: we fix the length of 'str'
            //
            pointer = str_Substring(str, 0, vstring_Capacity(s));
            vstring_From(s, pointer);
            free(pointer);
    }
    return s;
}


/**
 * @brief           Destructor
 * @param s      The vstring object to free
 *
 *
 */
void vstring_Destroy(vstring_t *s) {
            free(s->data);
            free(s);
}


/**
 * @brief                Array destructor.
 * @param s          The pointer array to free
 * @param len       The number of vstring objects in the array
 *
 */
void vstring_Destroy_Array(array_vstring_t s, int len) {
    for (size_t i = 0; i < len; i++) {
        free(s[i]->data);
        free(s[i]);
    }
    free(s);
}


/***************************************************************
 *
 *                                  Informative Methods
 *
 * *************************************************************/

/**
 * @brief           Returns the capacity of the specified object. The capacity
 *                      is the maximum number of characters that an object
 *                      can hold.
 *                      Capacity has to be greater than Zero
 *
 * @param s     Object whose capacity is being returned
 * @returns       The capacity of the object
 *
 */
int vstring_Capacity(const vstring_t *s)
{
    return s->capacity;
}


/**
 * @brief                   Count the number of words in a vstring object
 * @param s             The vstring object
 * @param count      Variable to return the number of words in the object
 *
 */
void vstring_Count_words(vstring_t *s, int *count) {

    int length = vstring_Len(s);
    char item;
    int num_of_words = 1;
    for (int index = 0; index< length; index++) {
        vstring_At(s, &item, index);

        // The delimiter character is SPACE
        //
        if (item == ' ')
            num_of_words++;
    }
    *count = num_of_words;
}


/**
 * @brief          Method to access the  string in the vstring object
 * @param s     the object
 * @returns      the string wrapped for the object
 *
 *
 */
const char const *vstring_Data(const vstring_t *s) {
    return (const char const *)s->data;
}


/**
 * @brief           It Compares the size of elements of two vstrings
 *                      (s1, s2) and if are equals, it compares the first n bytes
 *                      of memory area of s1->data and s2->data
 * @param s1    A vstring object
 * @param s2    A vstring object
 * @returns       True, if s1 and s2 are equals, otherwise False
 *
 *
 */
bool vstring_isEquals(const vstring_t *s1, const vstring_t *s2) {
    size_t len;
    len = (s1->len > s2->len) ? s1->len : s2->len;

    return memcmp(s1->data, s2->data, len) == 0;
}


/**
 * @brief           Returns True if object is Empty, that is, when its length
 *                      is equal to Zero
 * @param s     Pointer to vstring_t type
 * @returns       Bool
 *
 */
bool vstring_isEmpty(const vstring_t  *s) {
    return s->len == 0;
}


/**
 * @brief               Control pointers to stack
 * @param s         The vstring_t type object
 * @param item    The pointer.
 * @returns           True if the pointer points to HEAP. Otherwise returns
 *                          False
 *
 */
static bool vstring_Heap(const vstring_t *s, void *item) {

    long long t_v = (long long) s;
    long long t_item = (long long) item;
    size_t i;
    size_t tam = sizeof(long long);
    unsigned long long k = 1 << (tam * 8 - 1);

    for (i = 0; i < 24; i++) {

        if (((t_v & (k >> i)) == (k >> i)) !=
            ((t_item & (k >> i)) == (k >> i)))

            return false;
    }

    return true;

}


/**
 * @brief           Returns the number of characters in the data object,
 *                      the object's length
 * @param s     Pointer to vstring_t type
 * @returns       s->len, the number of characters in the data object
 *
 */
int vstring_Len(const vstring_t *s) {

    return s->len;
}


/**
 * @brief                    Control if supplied position is out of range or object
 *                               is empty.
 * @param s              Pointer to vstring_t type variable
 * @param position    Position to calculate
 * @returns                S_OK if position is correct and object is not empty
 *                               S_ERR_IS_EMPTY if the vstring is empty
 *                               S_ERR_OUT_OF_RANGE if position is not ok
*/
s_stat vstring_Pos_Err(const vstring_t *s, int position) {
    if (vstring_isEmpty(s))
        return S_ERR_IS_EMPTY;
    else {
        if (position < 0) {

            // length has to be greater than searched position, thus their
            // difference has to be greater than Zero

            if(s->len + position >= 0)
                return S_OK;
            else
                return S_ERR_OUT_OF_RANGE;

        } else if(s->len - position >= 0) {
                return S_OK;
            }
                else {
                    return S_ERR_OUT_OF_RANGE;
                }
    }
}


/**
 * @brief           Capacity bound. A proportional number over sizeof
 *                      integer
 * @param s      the object
 * @returns       Capacity bound
 *
 */
int vstring_Max_capacity(const vstring_t *s) {
    unsigned k = 1 << (sizeof(int) * 8 - 1);
    int m = s->ele_size * 8;
    return (k - 1) / m;
}


/***************************************************************
 *
 *                                  Element Access Methods
 *
 * *************************************************************/


/**
 * @brief               Print an array of vstring objects
 * @param s         A pointer to vstring_t object
 * @param array   An array of vstring objects
 * @param size     The number of vstrings objects in array
 *
 *
 */
void vstring_Array_print(vstring_t *s, const array_vstring_t array,
                                         int size)
{
    printf("(");
    foreach(s, array, size) {
        printf("%s, ", vstring_Data(s));
    }
    printf(")\n");
}


/**
 * @brief                   Character at position in the vstring object.
 * @param s             Pointer to vstring_t type variable
 * @param item        To copy the value found at searched position
 * @param position   Position to search
 * @returns                S_OK if position is correct and data is not empty
 *                               S_ERR_IS_EMPTY if the data is empty
 *                               S_ERR_OUT_OF_RANGE if position is not ok
 *
 */
s_stat vstring_At(const vstring_t *s, char *item, int position) {

    s_stat status = vstring_Pos_Err(s, position);
    switch (status) {
        case S_OK:
                        position = vstring_Pos(s, position);
                        *item = s->data[position];
                        return S_OK;
        case S_ERR_IS_EMPTY:
                        return S_ERR_IS_EMPTY;
        case S_ERR_OUT_OF_RANGE:
                        return S_ERR_OUT_OF_RANGE;
    }
}


/**
 * @brief                   Item at data's beginning.
 * @param s             Pointer to vstring_t type variable
 * @param item        To copy the value found at searched position
 * @returns               S_OK if data is not empty
 *                              S_ERR_IS_EMPTY if the vstring object is empty
 *
 */
s_stat vstring_Begin(const vstring_t *s, char *item) {
    return vstring_At(s, item, 0);
}


/**
 * @brief                   Item at data's ending.
 * @param s             Pointer to vstring_t type variable
 * @param item        To copy the value found at searched position
 * @returns               S_OK if data is not empty
 *                              S_ERR_IS_EMPTY if the vstring object is empty
 *
 */
s_stat vstring_End(const vstring_t *s, char *item) {
    return vstring_At(s, item, -1);
}


/**
 * @brief          Returns the strings that matches the pattern in one
 *                     vstring object.
 *                     In this case, the search is, at least, Big-O lineal time of
 *                     complexity.
 *                     We use the C's library 'fnmatch' to pattern matching.
 *                     The pattern may include the following special characters:
 *
 *                    *  Matches zero of more characters.
 *                    ?  Matches exactly one character.

 *                [...]  Matches one character if it's in a range of characters.
 *                        If the first character is `!', matches if the character is
 *                        not in the range. Between the brackets, the range is
 *                        specified by listing the characters that are in the range,
 *                        or two characters separated by `-' to indicate all
 *                        characters in that range.
 *                        For example, `[a-d]' matches `a', `b', `c', or `d'.
 *                        If you want to include the literal `-' in the range, make
 *                        it the first character, like in `[-afz]'.

 *                    \   Causes the next character to not be treated as a
 *                         wildcard. For example, `\*' matches an asterisk.
 *
 * @param s              A pointer to vstring_t objet
 * @param pattern    Pattern searched. It is a string.
 * @param result       vstring object pointer to return the result
 * @returns               S_OK if the element was found, or
 *                              S_IS_EMPTY if the vstring object is empty or
 *                              S_ERR_VALUE_NOT_FOUND
 *                              S_ERR_INVALID ARGUMENT
 */
s_stat vstring_Pattern(vstring_t *s, const char *pattern,
                                    vstring_t *result) {

    int length = 0;

    if (vstring_isEmpty(s))
        return S_ERR_IS_EMPTY;

    if (vstring_Capacity(result) == 0)
        return S_ERR_INVALID_ARGUMENT;

   else {

            size_t index;
            void *item;
            void *destAddr;
            array_vstring_t v = vstring_Split(s, &length);

            if (length == 0) {
                vstring_Destroy_Array(v, length);
                vstring_Destroy(result);
                return S_ERR_VALUE_NOT_FOUND;
            }

            for (size_t index = 0; index < length; index++) {
                char *item =  strcat(v[index]->data, ", ");
                if (!fnmatch(pattern, item, 0)) {
                    vstring_From(result, item);
                }
            }

            vstring_Destroy_Array(v, length);

    }

    return S_OK;
}


/**
 * @brief           Split in words the vstring data object. The delimiter is
 *                      SPACE.
 * @param s      A pointer to vstring object
 * @param len   To copy the number of vstrings in the array
 *                       that is returns
 * @returns        An array of vstring objects
 *
 */
array_vstring_t  vstring_Split(vstring_t *s, int * len) {

        size_t dim = vstring_Len(s);

        // We create a temporal buffer with the string in the data object
        char *buffer = calloc(dim, s->ele_size);
        s_assert(buffer, S_ERR_ALLOCATE_MEMORY);
        strncpy(buffer, (const char *)s->data, dim);

        size_t pos, length;
        pos = 0;
        length =  dim;

        // The delimiter is SPACE
        char delimiter = ' ';

        size_t index = 0;

        // First we try to know the number of words in the object
        while (length) {
            length = str_Length(buffer, pos, dim, delimiter);
            if (length == 0)
                break;
            pos += (length + 1);
            index++; // index is the number of words in the object
        }

        // If the number of words is Zero, we return the same data
        // but in another object
        if (index  == 0) {
            array_vstring_t s_array;
            s_array =   calloc(1, sizeof(vstring_t **));
            s_assert(s_array, S_ERR_ALLOCATE_MEMORY);


            s_array[0] = vstring_with_Capacity(s->data, s->capacity);

            free(buffer);

            *len = 1;
            return s_array;

        } else {

                pos = 0;
                length =  dim;

                array_vstring_t s_array  =   calloc(index + 1,
                                                                        sizeof(vstring_t));
                s_assert(s_array, S_ERR_ALLOCATE_MEMORY);

                // The function split_String returns the length of the new word
                // found
                for (size_t i = 0; i < index; i++) {
                    length = str_Length(buffer, pos, dim, delimiter);
                    if (length == 0)
                        break;
                    char *token;

                    // Then, we create a substring from pos of length characters
                    token = str_Substring(buffer, pos, length);

                    // and, we create a new vstring object, an its direction is
                    // stored at s_array[index]
                    s_array[i] = vstring_with_Capacity(token, BUFFER_SIZE);
                    free(token);

                    //We move the position in each iteration
                    pos += (length + 1);
                }

                // Now we consider the last word in the data object string
                if (pos < dim) {
                    char *token;
                    length = dim - pos;
                    token = str_Substring(buffer, pos, length);
                    s_array[index] = vstring_with_Capacity(token,
                                                                                BUFFER_SIZE);
                    free(token);

                    index++;
                }
                free(buffer);
                *len = index;
                return s_array;
        }

}


/***************************************************************
 *
 *                                  Modifiers Methods
 *
 * *************************************************************/

/**
 * @brief                   To edit the capacity of the vstring object, but only if
 *                              the object is S_UNSAFE and new capacity is lesser
 *                              than MAX_CAPACITY
 * @param s              The object to edit
 * @param capacity   The new capacity
 * @returns                S_OK or S_ERR_UNSAFE_CAPACITY
 *
 *
 */
s_stat vstring_Capacity_edit(vstring_t *s, int capacity) {

        if (s->safe == S_UNSAFE) {
            int max_cty = vstring_Max_capacity(s);
            s_assert((capacity < max_cty),
                                        S_ERR_MAX_CAPACITY);
            s->capacity = capacity;
            s->data = realloc(s->data, s->capacity * s->ele_size);
            // If memory error, exit,,,
            s_assert(s->data, S_ERR_ALLOCATE_MEMORY);
        } else
                return S_ERR_UNSAFE_CAPACITY;

        if (s->len > s->capacity)
            s->len = s->capacity;

        return S_OK;

}

/**
 * @brief           Remove the data in the object, but it does not free
 *                      the object
 * @param s     It is the object whose data we have to remove
 *
 */
void vstring_Clear(vstring_t *s) {
    s->len = 0;
    s->data[0] = 0;
}


/**
 * @brief           Remove the data in the object, if matches the pattern.
 *
 *                     We use the C's library 'fnmatch' to pattern matching.
 *                     The pattern may include the following special characters:
 *
 *                    *  Matches zero of more characters.
 *                    ?  Matches exactly one character.

 *                [...]  Matches one character if it's in a range of characters.
 *                        If the first character is `!', matches if the character is
 *                        not in the range. Between the brackets, the range is
 *                        specified by listing the characters that are in the range,
 *                        or two characters separated by `-' to indicate all
 *                        characters in that range.
 *                        For example, `[a-d]' matches `a', `b', `c', or `d'.
 *                        If you want to include the literal `-' in the range, make
 *                        it the first character, like in `[-afz]'.

 *                    \   Causes the next character to not be treated as a
 *                         wildcard. For example, `\*' matches an asterisk.

 * @param s     It is the object whose data we have to remove. We do not
 *                      free the object.
 * @param pattern    Pattern searched. It is a string.
 * @returns               S_OK if the pattern matches, or
 *                              S_IS_EMPTY if the vstring object is empty or
 *                              S_ERR_VALUE_NOT_FOUND
 *
 */
s_stat vstring_Clear_ifPattern(vstring_t *s, const char *pattern) {

    int length = 0;

    if (vstring_isEmpty(s))
        return S_ERR_IS_EMPTY;

   else {

            size_t index;
            void *item;
            void *destAddr;

            // We split the data object, and we store the strings in an array
            //
            array_vstring_t v = vstring_Split(s, &length);

            if (length == 0) {
                vstring_Destroy_Array(v, length);
                return S_ERR_VALUE_NOT_FOUND;
            }

            // Then we try to match the pattern with each string in the array
            // of vstrings objects

            for (size_t index = 0; index < length; index++) {
                char *item = v[index]->data;
                if (!fnmatch(pattern, item, 0)) {
                    vstring_Clear(s);
                    break;
                }
            }

            vstring_Destroy_Array(v, length);

    }

    return S_OK;
}


/**
 * @brief           It concatenates two vstrings objects in the first object
 * @param s     The vstring object we append with the second object
 * @param t      vstring object to insert in the first object
 * @returns       S_OK, or an error S_ERR_UNSAFE_CAPACITY, if the len
 *                      of the string to append plus the len of the first data
 *                      object is greater than CAPACITY
 *
 */
s_stat vstring_Concat(vstring_t *s, vstring_t *t) {
    char *str = t->data;
    return vstring_From(s, str);
}


/**
 * @brief               Insert (an item of data) at the end of vstring object.
 * @param s         Pointer to vstring_t type variable
 * @param item    Value to insert in vstring object
 * @returns           S_OK, or an error S_ERR_UNSAFE_CAPACITY, if the len
 *                          of the string to append plus the len of the data object
 *                          is greater than CAPACITY
 */
s_stat vstring_From(vstring_t *s, const char *str) {

        int len_str = strlen(str);
        if (s->len + len_str > s->capacity) {

            // The objects are inmutables by default
            if (s->safe == S_UNSAFE) {
                s->capacity = (s->len + len_str + 1) * 2;

                int max_cty = vstring_Max_capacity(s);
                s_assert((s->capacity < max_cty),
                                            S_ERR_MAX_CAPACITY);

                s->data = realloc(s->data, s->capacity * s->ele_size);
                // If memory error, exit,,,
                s_assert(s->data, S_ERR_ALLOCATE_MEMORY);
            } else
                    return S_ERR_UNSAFE_CAPACITY;
        }
        strcpy(&s->data[s->len], str);

        s->len += len_str;
        return S_OK;
}


/**
 * @brief                   Insert (an item of data) at position of vstring object.
 * @param s             Pointer to vstring_t type variable
 * @param str           The string to insert
 * @param position   Position at we insert the string. Position is Zero
 *                              index
 * @returns               We control the position. The function returns S_OK
 *                              or an error:
 *                              S_ERR_IS_EMPTY
 *                              S_ERR_OUT_OF_RANGE
 *
 */
s_stat vstring_From_at(vstring_t *s, const char *str, int position)

{
    size_t dim = vstring_Len(s);
    size_t length = strlen(str);

    char *buffer = calloc(dim, s->ele_size);
    s_assert(buffer, S_ERR_ALLOCATE_MEMORY);
    s_stat status = vstring_Pos_Err(s, position);
    switch (status) {
        case S_OK:
                        if (position < 0)
                            position = s->len + position + 1;

                        strncpy(buffer, (const char *)s->data, dim);
                        s->len = 0;

                        if (position > 0) {
                                char * token = str_Substring(buffer, 0, position);
                                vstring_From(s, token);
                                free(token);
                        }
                        vstring_From(s, str);
                        int res = dim - (position + length);
                        if (res > 1) {
                            char * token1 = str_Substring(buffer, position + length,
                                                                    dim - (position + length));
                            vstring_From(s, token1);
                            free(token1);

                        }

                        free(buffer);
                        return S_OK;

        case S_ERR_IS_EMPTY:
                        free(buffer);
                        return S_ERR_IS_EMPTY;
        case S_ERR_OUT_OF_RANGE:
                        free(buffer);
                        return S_ERR_OUT_OF_RANGE;
    }
}


/**
 * @brief                       We copy a file in a vstring object. In this case the
 *                                  object is UNSAFE
 * @param s                 The vstring object
 * @param filename      One string with the path and the number of the
 *                                  file.
 */
void vstring_From_file(vstring_t *s, const char *filename)

{
        FILE *p_file = fopen( filename, "r" );
        s_assert(p_file, S_ERR_OPEN_FILE);
        char buffer[BUFFER_SIZE];

        if (s->capacity < 1)
            s = vstring_New(BUFFER_SIZE);
        s->safe = S_UNSAFE;

        // Read each line into the buffer
        while( fgets(buffer, BUFFER_SIZE, p_file) != NULL ){
                vstring_From(s, buffer);
        }

        fclose( p_file );
}


/**
 * @brief               Set the object vstring inmutable, that is, SAFE and
 *                          capacity == len
 * @param s         the object vstring
 *
 *
 */
void vstring_Inmutable(vstring_t *s) {
    s->safe = S_SAFE;
    s->capacity = s->len;
}


/**
 * @brief           It concatenates the source vstring object n times
 * @param s     The source vstring object
 * @param num Number of times we repeat the vstring object
 * @returns       S_OK, or an error S_ERR_UNSAFE_CAPACITY, if the len
 *                      of the string to append plus the len of the vstring
 *                      object is greater than CAPACITY
 */
s_stat vstring_Rep(vstring_t *s,  size_t num) {

    s_assert(num > 0, S_ERR_INVALID_ARGUMENT);

    char *str = s->data;

    char *result =  str_Repeat (str, num);
    s_stat status = vstring_From(s, result);

    switch (status) {
        case S_ERR_UNSAFE_CAPACITY:
            free(result);
            return S_ERR_UNSAFE_CAPACITY;
        case S_OK:
            free(result);
            return S_OK;
    }
}


/**
 * @brief                   It truncates the data object at position
 * @param s             The data object
 * @param position   The position at we truncate the string
 * @returns               We control the position. The function returns S_OK
 *                              or an error:
 *                                  S_ERR_IS_EMPTY
 *                                  S_ERR_OUT_OF_RANGE
 */
s_stat vstring_Truncate(vstring_t *s, int position) {

    s_stat status = vstring_Pos_Err(s, position);
    switch (status) {
        case S_OK:
                        s->len = position;
                        s->data[position] = 0;
                        return S_OK;
        case S_ERR_IS_EMPTY:
                        return S_ERR_IS_EMPTY;
        case S_ERR_OUT_OF_RANGE:
                        return S_ERR_OUT_OF_RANGE;
    }
}


/**
 * @brief           To edit the state of the vstring object. By default the data
 *                      is inmutable, that is, we does not allow modify its
 *                      capacity
 * @param s     The object that we edit its state
 *
 *
 */
void vstring_Unsafe(vstring_t *s) {
    s->safe = S_UNSAFE;
}


/*******************************************************************
 *                          AUXILIARY FUNCTIONS
 *
 * *****************************************************************/



/**
 * @brief                   To calculate the lenght of substring in the buffer,
 *                              from position, to position of the delimiter.
 * @param src           The buffer with the information
 * @param position    Position from that we search the delimiter
 * @param length      Length of the buffer
 * @param delimiter  Delimiter character
 * @returns               Two control characters: new line and delimiter. If
 *                              we find '\n', it returns Zero. If we find 'delimiter
 *                              char' it returns (index - position). Otherwise returns
 *                              length Zero of substring.
 *
 *
 */
size_t  str_Length( char* src, int position, int length,
                                                            char delimiter )
{
    char control;

   for( int idx=position; idx< length; idx++ )
   {
        control = src[idx];
        if(control == delimiter)
            return (idx - position);
   } // end for

   return 0;
}


/**
 * @brief                  To identifie substrings in another string.
 * @param string     The buffer to divide from position to (position +
 *                             length)
 * @param position  Substring from position.
 * @param length    Length of desired substring.
 * @returns              This function returns char pointers to Heap. That
 *                             implies the library is responsible to free them.
 *                             It does whith the function Destroy_pointer.
 *
 *
 */
char *str_Substring(char *string, int position, int length) {

    char *pointer;
    int c;

   pointer = malloc(sizeof(char)*(length+1));
   s_assert(pointer, S_ERR_ALLOCATE_MEMORY);

   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position);
      string++;
   }

    *(pointer+c) = '\0';

    return pointer;

}


char *str_Repeat (char *str, size_t count) {

    char *ret = malloc (strlen (str) * count + count);
    s_assert(ret, S_ERR_ALLOCATE_MEMORY);

    strcpy (ret, str);

    while (--count > 0) {
        strcat (ret, str);
    }

    return ret;
}


/**
 * @brief                           This function catcher errors that the macro
 *                                      s_assert throws
 * @param file
 * @param line_number
 *
 *
 */
void vstring_Abort(s_stat status, const char* file, int line_number) {

    switch(status) {
        case S_ERR_ALLOCATE_MEMORY :
            fprintf(stderr, "\nVirtual memory exhausted. File: %s Line:% d\n",
            file,
            line_number);
            exit(EXIT_FAILURE);
            break;

        case S_ERR_MEMCPY :
            fprintf(stderr,
            "\nCopy memory error [memcpy]. File: %s Line:% d\n",
            file,
            line_number);
            exit(EXIT_FAILURE);
            break;

        case S_ERR_MEMMOVE :
            fprintf(stderr,
            "\nMove memory error [memmove]. File: %s Line:% d\n",
            file,
            line_number);
            exit(EXIT_FAILURE);
            break;

        case S_ERR_INVALID_ARGUMENT :
            fprintf(stderr,
            "\nInvalid argument in Constructor. File: %s Line:% d\n",
            file,
            line_number);
            break;
        case S_ERR_STACK:
            fprintf(stderr,
            "\nDirection pointer to STACK. Warning!!!. File: %s : % d\n",
            file,
            line_number);
            break;
        case S_ERR_MAX_CAPACITY:
            // (2^(sizeof(int) * 8 - 1))  / v->size_ele * 8
            fprintf(stderr,
            "\nMax capacity. Memory exhausted. File: %s Line:% d\n",
            file,
            line_number);
            exit(EXIT_FAILURE);
            break;
        case S_ERR_UNSAFE_CAPACITY:
            fprintf(stderr,
            "\n Too much characters in the string to copy. File: %s : % d\n",
            file,
            line_number);
            break;
        case S_ERR_OPEN_FILE:
            fprintf(stderr,
            "\n Cannot open the file. File: %s : % d\n",
            file,
            line_number);
            break;
    }
}

/**
 * @brief                    It calculates new position if it is a negative number
 * @param s              Pointer to vstring_t type variable
 * @param position    Position to calculate
 * @returns                New position, if it is a negative number
 *
 */
int vstring_Pos(const vstring_t *s, int position) {

    if (position < 0) {
    /* -1 = (s->len - 1)
     * -2 = (s->len - 1) - 1
     * -3 = (s->len - 1) - 2
     */
        return (s->len + position);

    } else {
        return position;
    }
}
