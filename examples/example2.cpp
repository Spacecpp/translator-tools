// This example simulates the translation of a game main menu to 5 languages

#include <iostream>
#include <stdio.h>
#include <string>

#include "DictionaryManager.h"

// the languages we are going to use, identified by a number or string
namespace LANGUAGE { enum { EN_US, PT_BR, ES, FR, DE, COUNT }; }
const char * language_identifier[LANGUAGE::COUNT] = { "en_us", "pt_br", "es", "fr", "de" };


// the selected language id
int currentLanguage = 0;
DictionaryManager dictionaryManager;


std::string get_translated_string(const char * text) 
{ 
	return dictionaryManager.get_translation(currentLanguage, text); 
}

std::string get_translated_string(const std::string & text)
{ 
	return dictionaryManager.get_translation(currentLanguage, text); 
}


void init_dictionaries()
{
	
	// create all dictionaries, setting both numerical id and string identificators
	for (int i = 0; i < LANGUAGE::COUNT; i++)
		dictionaryManager.add_dictionary( language_identifier[i], i);
	
}



int load_translations()
{
	
	int load_count = 0;
	
	// skip english, as it is already the program base language
	for (int i = LANGUAGE::EN_US + 1; i < LANGUAGE::COUNT; i++)
	{
		
		std::string filepath = std::string(language_identifier[i]) + ".txt";
		
		if (dictionaryManager.load(i, filepath) )
			load_count++;
		else
			printf("Failed to load %s dictionary\n", language_identifier[i] );
		
	}
	
	return load_count;
	
}



int main() 
{
	
	// loads all translated strings
	init_dictionaries();
	load_translations();
	
	// language selection menu
	printf("Choose a language: \n");
	for (int i = 0; i < LANGUAGE::COUNT; i++)
		printf("%d - %s\n", i, language_identifier[i] );
	
	scanf("%d", &currentLanguage);
	
	// just in case the user inputs an invalid id
	if (currentLanguage < 0 || currentLanguage >= LANGUAGE::COUNT)
		currentLanguage = 0;
	
	printf("\n");
	
	// prints the game main menu
	printf("1 - %s\n", get_translated_string("New game").c_str() );
	printf("2 - %s\n", get_translated_string("Multiplayer").c_str() );
	printf("3 - %s\n", get_translated_string("Options").c_str() );
	printf("4 - %s\n", get_translated_string("Exit").c_str() );
	
    return 0;
	
}
