/*
 * File:   vstring.h
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

#ifndef VSTRING_H
#define VSTRING_H
#include<stdbool.h>


typedef enum S_safe {
    S_UNSAFE = 0,
    S_SAFE = 1,
} safe_t;


typedef struct Vstring{

        int len; /**<  Number of characters in data*/

        char *data; /**< Array of characters to hold the information  */

        int capacity; /**<  Capacity, in number of characters. Capacity has
                                 to be greater than Zero*/
        size_t ele_size; /**< Reserved to other encoders  */
        safe_t safe; /**<  By default all the objects are safe. That means
                                its capacity is inmutable*/

} vstring_t;


/**
 * @brief An array of vstring objects
 *
 */
typedef vstring_t ** array_vstring_t;



/**
 * @brief  Control of errors
 *
 *
 */
typedef enum S_stat {
    S_OK = 0, /**< All is ok, no errors */
    S_ERR_IS_EMPTY = 1, /**<  The Length in the object is Zero*/
    S_ERR_VALUE_NOT_FOUND = 2, /**< The object does not contains the
                                                            data searched */
    S_ERR_OUT_OF_RANGE = 3, /**< The position is out of the bounds  */
    S_ERR_FILE = 4, /**< The file is unavoible */
    S_ERR_STACK = 5, /**< The pointer points to the STACK */
    S_ERR_ALLOCATE_MEMORY = 6, /**< Error in Memory Manager with
                                                        malloc, calloc or realloc */
    S_ERR_MEMCPY = 7, /**< Error in Memory with memcpy function */
    S_ERR_MEMMOVE = 8, /**< Error in Memory with memmove function*/
    S_ERR_INVALID_ARGUMENT = 9, /**< The parameters in the function
                                                            are not valids */
    S_ERR_MAX_CAPACITY = 10, /**< Bound of capacity */
    S_ERR_UNSAFE_CAPACITY = 11, /**< The capacity is inmutable.
                                                             The object is S_SAFE */
    S_ERR_OPEN_FILE = 12, /**< The file is unavoible */
}s_stat;



/**
 * @brief     Length of strings or capacity of our vstring objects
 *
 */
#define BUFFER_SIZE 1024

#define _s_assert(X, Y, F, LINE)                                                                              \
{                                                                                                             \
    if(!X){                                                                                                   \
        vstring_Abort(                                                                                        \
                Y,                                                                                            \
                F,                                                                                            \
                LINE);                                                                                        \
    }                                                                                                         \
}

/**
 * @brief               Control of errors
 * @param X         Test. If it evaluates to FALSE it throws an error
 * @param Y         s_stat data to identifie the throwed error
 * @returns           Nothing if test evaluates to true. Otherwise it throws
 *                          an error. The catcher is the function vstring_Abort
 */
#define s_assert(X, Y) _s_assert(X, Y, __FILE__, __LINE__)

/**
 * @brief               Can be used like
 *                             int values[] = { 1, 2, 3 };
 *                             foreach(int *v, values) {
 *                                  printf("value: %d\n", *v);
 *                             }
 * @param item
 * @param array
 * @returns
 *
 *
 */
#define foreach(item, array, size)                                                                            \
    for(int keep = 1, \
            index= 0;                                                                                         \
        keep && index != size;                                                                                \
        keep = !keep, index++)                                                                                \
      for(item = *(array + index); keep; keep = !keep)

/***************************************************************
 *
 *                                  Constructor  -  Destructor
 *
 * *************************************************************/
vstring_t * vstring_New(int  capacity);
vstring_t  * vstring_with_Capacity(char * str, int capacity);

void vstring_Destroy(vstring_t *s);
void vstring_Destroy_Array(array_vstring_t s, int len);


/***************************************************************
 *
 *                                  Informative Methods
 *
 * *************************************************************/
int vstring_Capacity(const vstring_t *s);
void vstring_Count_words(vstring_t *s, int *count);
const char const *vstring_Data(const vstring_t *s);
bool vstring_isEquals(const vstring_t *s1, const vstring_t *s2);
bool vstring_isEmpty(const vstring_t  *s);
int vstring_Len(const vstring_t *s);
s_stat vstring_Pos_Err(const vstring_t *s, int position);
int vstring_Max_capacity(const vstring_t *s);

/***************************************************************
 *
 *                                  Element Access Methods
 *
 * *************************************************************/
void vstring_Array_print(vstring_t *item, const array_vstring_t array,
                                        int size);
s_stat vstring_Begin(const vstring_t *s, char *item);
s_stat vstring_End(const vstring_t *s, char *item);
s_stat vstring_At(const vstring_t *s, char *item, int position);
s_stat vstring_Pattern(vstring_t *s, const char *pattern,
                                            vstring_t *result);

array_vstring_t vstring_Split(vstring_t *s, int *len);



/***************************************************************
 *
 *                                  Modifiers Methods
 *
 * *************************************************************/
s_stat vstring_Capacity_edit(vstring_t *s, int capacity);
void vstring_Clear(vstring_t *s) ;
s_stat vstring_Clear_ifPattern(vstring_t *s, const char *pattern);
s_stat vstring_Concat(vstring_t *s, vstring_t *t);
s_stat vstring_From(vstring_t *s, const char *str);
void vstring_Inmutable(vstring_t *s);
s_stat vstring_From_at(vstring_t *s, const char *str, int position);
void vstring_From_file(vstring_t *s, const char *filename);
s_stat vstring_Rep(vstring_t *s, size_t num);
s_stat vstring_Truncate(vstring_t *s, int position);
void vstring_Unsafe(vstring_t *s);

/***************************************************************
 *
 *                                  Auxiliary Functions
 *
 * *************************************************************/

void vstring_Abort(s_stat status,
                            const char* file, int line_number);

int vstring_Pos(const vstring_t *s, int position);

size_t  str_Length( char* src, int position, int length,
                                                            char delimiter );

char * str_Substring(char *string, int position, int length);


char *str_Repeat (char *str, size_t count);


#endif /* VSTRING_H */
