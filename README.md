# vString

**Example**

```dart
    // We create the vstring object and append its data with one string.
    vstring_t *s =
            vstring_with_Capacity("Hello world! and welcome torticolis", 50);

    // This is the access method to the data
    printf("The string is: %s\n", vstring_Data(s));

    // This is the access method to the length of the stored string
    printf("its length is: %d\n", vstring_Len(s));

    // This is the maximum capacity of storage
    printf("its capacity is: %d\n", vstring_Capacity(s));

    // now we are asking for a position. In this case the last position
    // We can use negative number to access the position character in the
    // vstring object data
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
                            vstring_Data(y),
                            vstring_isEquals(u, y)?"True":"False");

    // We insert a string in the vstring object data, at position 6
    vstring_From_at(y, "Ana", 6);
    printf("%s and %s are the same? %s\n", vstring_Data(u),
                            vstring_Data(y),
                            vstring_isEquals(u, y)?"True":"False");

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
```
