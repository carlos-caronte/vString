/*
 * File:   vstring.h
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

#ifndef VSTRING_H
#define VSTRING_H
#include<stdbool.h>


typedef enum S_safe {
    S_UNSAFE = 0,
    S_SAFE = 1,
} safe_t;


typedef struct Vstring{
        int len; /**<  Number of elements in Vector object*/

        char *data; /**< Array to hold the information  */

        int capacity; /**<  Capacity, in number of elements. Capacity has
                                 to be greater than Zero*/
        size_t ele_size;
        safe_t safe;

} vstring_t;

typedef vstring_t ** array_vstring_t;



typedef enum S_stat {
    S_OK = 0,
    S_ERR_IS_EMPTY = 1,
    S_ERR_VALUE_NOT_FOUND = 2,
    S_ERR_OUT_OF_RANGE = 3,
    S_ERR_FILE = 4,
    S_ERR_STACK = 5,
    S_ERR_ALLOCATE_MEMORY = 6,
    S_ERR_MEMCPY = 7,
    S_ERR_MEMMOVE = 8,
    S_ERR_INVALID_ARGUMENT = 9,
    S_ERR_MAX_CAPACITY = 10,
    S_ERR_UNSAFE_CAPACITY = 11,
    S_ERR_OPEN_FILE = 12,
}s_stat;



/**
 * @brief     Length of strings that we want to split in substrings.
 *                This data is used in the feature Insert_from_file.
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

#define s_assert(X, Y) _s_assert(X, Y, __FILE__, __LINE__)


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
s_stat vstring_Begin(const vstring_t *s, char *item);
s_stat vstring_End(const vstring_t *s, char *item);
s_stat vstring_At(const vstring_t *s, char *item, int position);
s_stat vstring_Iter(const vstring_t *s, char *item, int index);
s_stat vstring_Iter_next(const vstring_t *s, char *item, int index);
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
s_stat vstring_From(vstring_t *s, const char *str);
s_stat vstring_From_at(vstring_t *s, const char *str, int position);
void vstring_From_file(vstring_t *s, const char *filename);
s_stat vstring_Clear_ifPattern(vstring_t *s, const char *pattern);
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
size_t  split_String( char* src, int position, int length,
                                                                char delimiter );
char * substring(char *string, int position, int length);

#endif /* VSTRING_H */

