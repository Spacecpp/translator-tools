# translator-tools
The goal of this set of tools is to make the process of translating a software the most straightforward as possible.

# How to use:
For each language your program is going to support you create a .txt file containing every translatable string relating key and value. Like this:
```
hello = ola
world = mundo
how = como
are = esta
you = voce
```
The order doesn't matter. You can also leave comments on the file by beginning the line with a //. Blank lines are also ok.

# To the code:
Include DictionaryManager.cpp on your project. 
Dictionary is the class that stores key and value for one language.
DictionaryManager is the class that does handles the list of dictionaries and allows easily retrieving the right strings when needed. You need one object of this class.
```
DictionaryManager dictionaryManager;
```

Dictionaries can be accessed either by an integer id or a string. You can set both by adding it to the list first:
```
dictionaryManager.add_dictionary("pt_br", PT_BR);
```

You can add pairs of key/value one by one...
```
dictionaryManager.add_translation(PT_BR, "hello", "ola");
```

..or preferably by loading a text file containing all of them
```
dictionaryManager.load(PT_BR, "pt_br.txt");
```

Finally, you retrieve the translated strings
```
std::string str1 = dictionaryManager.get_translation(PT_BR, "hello") + " " + dictionaryManager.get_translation(PT_BR, "world") + "!";
```

You may use a variable to store the current program language to easily access the right string.
```
int currentLanguage = PT_BR;
.
.
.
std::string str1 = dictionaryManager.get_translation(currentLanguage, "hello") + " " + dictionaryManager.get_translation(currentLanguage, "world") + "!";
```

When the requested string is not available or the requested dictionary is empty it will simply return the input string. So, no need to create a dictionary for the program default language.


# Creating multiple translation files
Once you have your first translation ready you will essentially make a copy of said file, rename, and them proceed to change every string inside.
This process can quickly become very laboring and dull, especially if you are doing all the work alone using automatic translations. 
That's why there is this small program TranslationCreator.cpp.
Just compile and run it. it offers some usefull automations:

1-Remove the values (translations) from a file.  
2-Remove the keys.  
3-Create a complete translation file based on a file composed just of the keys, and another composed of values.  
4-Fix (match) the case of values to the keys.  

With these automations you can create a file with just the strings to be translated, copy paste it at google translator, and them quickly combine everything to a new file. 

# Note
When creating translations to non-latin alphabet languages make sure the .txt is saved in UTF-8 encoding.

# Optional preprocessor directives:
**DICTIONARY_MANAGER_DEBUG** - Enables debug messages. Warns when a key has been add more than once, or when a requested translation is missing.  
**DICTIONARY_MANAGER_USE_ALLEGRO** - If your program uses the Allegro5 library set this to make all the file reading be done by its functions (required to read inside zip files through the physfs addon).  
