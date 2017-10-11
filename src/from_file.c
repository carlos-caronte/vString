/*
 * File:   from_file.c
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
#include"vstring.h"

#define CAPACITY 1024

int main()
{

    /***************************************************************
     *
     *                                  Insert from FILE
     *
     * *************************************************************/

        vstring_t *v_file = vstring_New(CAPACITY);

        const char *filename = "../README.md";
        vstring_From_file(v_file, filename);
        printf("%s\n", vstring_Data(v_file));

        // Each record read from the file is a pointer to Heap. This function
        // clean up the memory.
        vstring_Destroy(v_file);

        return 0;
}
