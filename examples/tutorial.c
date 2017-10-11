/*
 * File:   tutorial.c
 * Author: jcr
 *
 * Created on 11 October, 2017
 */
 /*
 * Copyright (C) 2017 Juan Carlos Rey <carlos.caronte@gmail.com>
 *
 * This file is part of vString.
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
#include"vstring.h"


int main() {

    // We create the string.
    vstring_t *s =
            vstring_with_Capacity("Hello world! and welcome torticolis", 50);

    printf("The string is: %s\n", vstring_Data(s));
    printf("its length is: %d\n", vstring_Len(s));
    printf("its capacity is: %d\n", vstring_Capacity(s));

    // now we are asking for a position. In this case the last position
    char p;
    if(vstring_At(s, &p, -1) == S_OK)
        printf("At -1: %c\n", p);

    if (p == 's')
        printf("Ok, the last character is 's'...!!!!\n");

    // if we want to split the string, the library returns an array of strings
    // In len, the library returns the number of strings in array
    int len = 0;
    array_vstring_t v = vstring_Split(s, &len);
    printf("The string contains %d words,,\n", len);
    for (size_t index = 0; index < len; index++){
        printf("V[%d]: %s\n", index, v[index]->data);
    }

    // We have to use this Destructor, in this case
    vstring_Destroy_Array(v, len);


    // The library knows if two strings are equals or not
    vstring_t *u = vstring_with_Capacity("Hello world!", 50);
    vstring_t *y = vstring_with_Capacity("Hello world!", 50);
    printf("%s and %s are the same? %s\n", vstring_Data(u),
                            vstring_Data(y), vstring_isEquals(u, y)?"True":"False");
    vstring_From_at(y, "Ana", 6);
    printf("%s and %s are the same? %s\n", vstring_Data(u),
                            vstring_Data(y), vstring_isEquals(u, y)?"True":"False");

    vstring_Destroy(u);
    vstring_Destroy(y);

    vstring_t * pattern = vstring_New(50);
    printf("If we look for the pattern 'w*'...");
    if (vstring_Pattern(s, "w*", pattern) == S_OK)
        printf("we found: %s\n", vstring_Data(pattern));

    vstring_Destroy(pattern);

    printf("If we want remove strings with the pattern 'torticolis'...");
    if (vstring_Clear_ifPattern(s, "torticolis") == S_OK)
            printf("the string is removed ...: %s\n",
                        vstring_Data(s));

    vstring_Destroy(s);

    return 0;
}
