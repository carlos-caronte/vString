/*
 * File:   vstring.c
 * Author: jcr
 *
 * Created on 11 Octoberber, 2017
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
 * @brief
 * @param capacity
 * @returns
 *
 *
 */
vstring_t * vstring_New(int capacity) {

     s_assert(capacity > 0, S_ERR_INVALID_ARGUMENT);

     vstring_t *s = malloc(sizeof(vstring_t));
     s_assert(s, S_ERR_ALLOCATE_MEMORY);
     s->len = 0;
     s->capacity = capacity;
     s->ele_size = sizeof(char);
     s->safe = S_SAFE;
     s->data = calloc(1, s->capacity * s->ele_size + 1);
     // If memory error, exit,,,
     s_assert(s->data, S_ERR_ALLOCATE_MEMORY);

    return s;
}


/**
 * @brief
 * @param str
 * @param capacity
 * @returns
 *
 *
 */
vstring_t * vstring_with_Capacity(char * str, int capacity) {

    vstring_t *s = vstring_New(capacity);
    s_stat status = vstring_From(s, str);
    char *pointer;
    switch(status) {
        case  S_OK:
            return s;
        case S_ERR_UNSAFE_CAPACITY:
            pointer = substring(str, 0, vstring_Capacity(s));
            vstring_From(s, pointer);
            free(pointer);
    }
    return s;
}


/**
 * @brief
 * @param s
 *
 *
 */
void vstring_Destroy(vstring_t *s) {
            free(s->data);
            free(s);
}


/**
 * @brief
 * @param s
 * @param len
 *
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
 * @brief           Returns the capacity of the specified Vector. The capacity
 *                      is the maximum number of elements a Vector can hold
 *                      before it has to be resized. Capacity has to be greater
 *                      than Zero
 *
 * @param v     Vector object whose capacity is being returned
 * @returns       The capacity of the Vector object
 *
 *
 */
int vstring_Capacity(const vstring_t *s)
{
    return s->capacity;
}


/**
 * @brief           It Compares the size of elements of two Vector objects
 *                      (v1, v2) and if are equals, it compares the first n bytes
 *                      of memory area of v1->data and v2->data
 * @param v1    A vector object
 * @param v2    A vector object
 * @returns       True, if v1 and v2 are equals, otherwise False
 *
 *
 */
bool vstring_isEquals(const vstring_t *s1, const vstring_t *s2) {
    size_t len;
    len = (s1->len > s2->len) ? s1->len : s2->len;

    return memcmp(s1->data, s2->data, len) == 0;
}


/**
 * @brief           Returns True if Vector is Empty, that is when its length
 *                      is equal to Zero
 * @param v     Pointer to vector_t type
 * @returns       Bool
 *
 */
bool vstring_isEmpty(const vstring_t  *s) {
    return s->len == 0;
}


/**
 * @brief
 * @param v
 * @param item
 * @returns
 *
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
 * @brief           Returns the number of elements in the Vector,
 *                      the Vector's length
 * @param v     Pointer to vector_t type
 * @returns       v->len, the number of elements in Vector
 *
 */
int vstring_Len(const vstring_t *s) {

    return s->len;
}


/**
 * @brief                    Control if supplied position is out of range or Vector
 *                               is empty.
 * @param v              Pointer to vector_t type variable
 * @param position    Position to calculate
 * @returns                V_OK if position is correct and Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *                               V_ERR_OUT_OF_RANGE if position is not ok
*/
s_stat vstring_Pos_Err(const vstring_t *s, int position) {
    if (vstring_isEmpty(s))
        return S_ERR_IS_EMPTY;
    else {
        if (position < 0) {
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
 * @brief
 * @param v
 * @returns
 *
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
 * @brief                   Item at position in the Vector object.
 * @param v             Pointer to vector_t type variable
 * @param item        To copy the value found at searched position
 * @param position   Position to search
 * @returns                V_OK if position is correct and Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *                               V_ERR_OUT_OF_RANGE if position is not ok
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
 * @brief                   Item at Vector's beginning.
 * @param v             Pointer to vector_t type variable
 * @param item        To copy the value found at searched position
 * @returns                V_OK if Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *
 */
s_stat vstring_Begin(const vstring_t *s, char *item) {
    return vstring_At(s, item, 0);
}


/**
 * @brief
 * @param s
 * @returns
 *
 *
 */
const char const *vstring_Data(const vstring_t *s) {
    return (const char const *)s->data;
}


/**
 * @brief                   Item at Vector's ending.
 * @param v             Pointer to vector_t type variable
 * @param item        To copy the value found at searched position
 * @returns                V_OK if Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *
 */
s_stat vstring_End(const vstring_t *s, char *item) {
    return vstring_At(s, item, -1);
}


/**
 * @brief                   Itertools, returns item at position in the Vector
 *                              object. If index==0, begin; if index==-1, end
 * @param v             Pointer to vector_t type variable
 * @param item        To copy the value found at searched position
 * @param index       Position to search
 * @returns                V_OK if position is correct and Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *                               V_ERR_OUT_OF_RANGE if position is not ok
 *
 */
s_stat vstring_Iter(const vstring_t *s, char *item, int index) {
    return vstring_At(s,item, index);
}


/**
 * @brief                   Itertools, returns Item at index+1 in the Vector
 *                              object.
 * @param v             Pointer to vector_t type variable
 * @param item        To copy the value found at searched position
 * @param index       Position to search
 * @returns                V_OK if position is correct and Vector is not empty
 *                               V_ERR_IS_EMPTY if the Vector Object is empty
 *                               V_ERR_OUT_OF_RANGE if position is not ok
 *
 */
s_stat vstring_Iter_next(const vstring_t *s, char *item, int index) {
  return vstring_At(s, item, index + 1);
}


/**
 * @brief          Returns the strings that matches the pattern in one slice.
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
 * @param v             A pointer to vector_t objet
 * @param pattern    Pattern searched. It is a string.
 * @param slice        Vector object pointer to return the slice
 * @returns               V_OK if the element was found, or
 *                              V_IS_EMPTY if the Vector Object is empty or
 *                              V_ERR_VALUE_NOT_FOUND
 *                              V_ERR_INVALID ARGUMENT
 *                              It returns items until it is reached the capacity
 *                              of slice
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
                char *item = v[index]->data;
                if (!fnmatch(pattern, item, 0)) {
                    strcat(item, ", ");
                    vstring_From(result, item);
                }
            }

            vstring_Destroy_Array(v, length);

    }

    return S_OK;
}


/**
 * @brief
 * @param s
 * @param len
 * @returns
 *
 *
 */
array_vstring_t  vstring_Split(vstring_t *s, int * len) {
        size_t dim = vstring_Len(s);
        char *buffer = calloc(dim, s->ele_size);
        s_assert(buffer, S_ERR_ALLOCATE_MEMORY);

        strncpy(buffer, (const char *)s->data, dim);

        size_t pos, length;
        pos = 0;
        length =  dim;
        char delimiter = ' ';

        size_t index = 0;
        while (length) {
            length = split_String(buffer, pos, dim, delimiter);
            if (length == 0)
                break;
            pos += (length + 1);
            index++;
        }

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

                for (size_t i = 0; i < index; i++) {
                    length = split_String(buffer, pos, dim, delimiter);
                    if (length == 0)
                        break;
                    char *token;
                    token = substring(buffer, pos, length);
                    s_array[i] = vstring_with_Capacity(token, BUFFER_SIZE);
                    free(token);
                    pos += (length + 1);
                }
                if (pos < dim) {
                    char *token;
                    length = dim - pos;
                    token = substring(buffer, pos, length);
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
 * @brief
 * @param s
 *
 *
 */
void vstring_Clear(vstring_t *s) {
    s->len = 0;
    s->data[0] = 0;
}


/**
 * @brief               Insert (an item of data) at the end of Vector object.
 * @param v         Pointer to vector_t type variable
 * @param item    Value to insert in Vector object
 */
s_stat vstring_From(vstring_t *s, const char *str) {

        int len_str = strlen(str);
        if (s->len + len_str > s->capacity) {
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
                                char * token = substring(buffer, 0, position);
                                vstring_From(s, token);
                                free(token);
                        }
                        vstring_From(s, str);
                        int res = dim - (position + length);
                        if (res > 1) {
                            char * token1 = substring(buffer, position + length,
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


s_stat vstring_Clear_ifPattern(vstring_t *s, const char *pattern) {

    int length = 0;

    if (vstring_isEmpty(s))
        return S_ERR_IS_EMPTY;

   else {

            size_t index;
            void *item;
            void *destAddr;
            array_vstring_t v = vstring_Split(s, &length);

            if (length == 0) {
                vstring_Destroy_Array(v, length);
                return S_ERR_VALUE_NOT_FOUND;
            }

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
size_t  split_String( char* src, int position, int length,
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
}  //end splitString()


/**
 * @brief                  To identifie substrings in another string.
 * @param string     The buffer to divide from position to (position +
 *                             length)
 * @param position  Substring from position.
 * @param length    Length of desired substring.
 * @returns             This function returns char pointers to Heap. That
 *                            implies the library is responsible to free them.  It does
 *                            whit the function Destroy_pointer.
 *
 *
 */
char *substring(char *string, int position, int length) {

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

/**
 * @brief
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
            exit(EXIT_FAILURE);
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
 * @brief                    Calculate new position if it is a negative number
 * @param v              Pointer to vector_t type variable
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
