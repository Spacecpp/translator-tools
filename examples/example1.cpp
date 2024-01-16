// This is an example showing basic usage of the DictionaryManager class

#include <iostream>
#include <stdio.h>
#include <string>

#include "DictionaryManager.h"


int main() 
{
	
	// an enum for the languages we are going to use
	enum { EN_US, PT_BR };
	
	DictionaryManager dictionaryManager;
	
	// first we add the languages we want to use
	// english is our "base" language, we can skip it
	
	//dictionaryManager.add_dictionary("en_us", EN_US);
	dictionaryManager.add_dictionary("pt_br", PT_BR);
	
	
	// load a dictionary file for brazilian portuguese
	if (!dictionaryManager.load(PT_BR, "pt_br.txt") )
		printf("Failed to load pt_br dictionary\n");
	
	// show all the data currently added
	printf("Dictionaries: %d, total words: %d\n", dictionaryManager.get_dictionary_count(), dictionaryManager.get_word_count() );
	dictionaryManager.print_dictionary_list();
	
	
	// the language we are going to translate to
	int currentLanguage = PT_BR;
	
	// lets try translating some strings, word by word
	std::string str1 = dictionaryManager.get_translation(currentLanguage, "hello") + " " + dictionaryManager.get_translation(currentLanguage, "world") + "!";
	
	// the words are stored lowercase, let's uppercase the first character
	str1[0] = toupper(str1[0]); 
	
	// we can get the strings using the language name too
	std::string str2 = dictionaryManager.get_translation("pt_br", "how") + " " + dictionaryManager.get_translation("pt_br", "are") + " " + dictionaryManager.get_translation("pt_br", "you") + "?";
	str2[0] = toupper(str2[0]);
	
	// print the results
	printf("%s\n%s\n", str1.c_str(), str2.c_str() );
	
	
	// we can add translations manually too
	dictionaryManager.add_translation(PT_BR, "Words", "Palavras");
	
	// if a translation isn't found, it simply returns the input string itself 
	printf("%s %s\n", dictionaryManager.get_translation(PT_BR, "Strange").c_str(), dictionaryManager.get_translation(PT_BR, "Words").c_str() );
	
	
	// now translating a whole sentence
	std::string str3 = dictionaryManager.get_translation(currentLanguage, "this is a long string");
	printf("%s\n", str3.c_str() );
	
    return 0;
	
}
