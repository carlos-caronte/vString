/*
 * File:   check_vstring.c
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
 * along with Vector.  If not, see <http://www.gnu.org/licenses/>.
 */


#include<stdio.h>
#include<stdlib.h>
#include<check.h>
#include"../src/vstring.h"

#define CAPACITY 1000
#define RANDOM 1300000
#define N 5000


START_TEST(vstring)
{

        int n = random() % N;

        char **a = calloc(n, sizeof (char *));  /* n pointers to char *  */
        int i;
        for (i = 0; i < n; i++) {
                int n1 = random() % N;
                a[i] = calloc(n1 + 1, sizeof (char));
                int j;
                for (j = 0; j < n1; j++) {
                        a[i][j] = random() % 93 + 32;
                }
                a[i][n1] = 0;
        }
        s_stat status;
        char value;
        int x = 0, k = 0;
        vstring_t *s = vstring_New(CAPACITY);
        s->safe = S_UNSAFE;

        for (i = 0; i < n; i++) {

                status = vstring_From (s, a[i]);

                ck_assert(status == S_OK);
                vstring_Begin(s, &value);
                ck_assert(!strncmp(&value, &a[i][0], 1));

                k = strlen(a[i]);

                vstring_End(s, &value);
                ck_assert(!strncmp(&value, &a[i][k-1], 1));

                x = vstring_Len(s);
                ck_assert_int_eq(x, k);

                vstring_Clear(s);
                s->capacity = CAPACITY;


        }

        for (i = 0; i < n; i++) {
                free(a[i]);
        }
        free(a);

        vstring_Destroy(s);
}

END_TEST


Suite* vstring_suite()
{
        Suite *v = suite_create("vString");

        TCase *tc_string = tcase_create("vstring");
        tcase_add_test(tc_string, vstring);
        suite_add_tcase(v, tc_string);

        return v;
}

int main()
{
        int number_failed;
        Suite *v = vstring_suite();
        SRunner *vr = srunner_create(v);
        srunner_run_all(vr, CK_NORMAL);
        number_failed = srunner_ntests_failed(vr);
        srunner_free(vr);

        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
