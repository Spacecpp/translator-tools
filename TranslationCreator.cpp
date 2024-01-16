#include <stdio.h>

#include <string>
#include <list>


typedef std::list< std::string > StringList;

#define LINE_MAX_LENGTH 512

#define NO_DOUBLE_LINEBREAK 1


static void trim(std::string & str)
{
	
	if (!str.empty() )
	{
		
		const char * whiteSpace = " \t\v\r\n\f";
		std::size_t start = str.find_first_not_of(whiteSpace);
		std::size_t end = str.find_last_not_of(whiteSpace);
		
		if (start == std::string::npos && end == std::string::npos) // spaces only
			str.clear();
			
		else // trim
			str = str.substr(start, end - start + 1); 
		
	}
	
}



static void skip_BOM(FILE * file)
{
	
	bool bom_found = false;
	unsigned char bytes[3];
	
	size_t bytes_read = fread(bytes, 1, 3, file);
    if (bytes_read == 3) 
	{
        
		if (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) // UTF-8 BOM
		{
			bom_found = true;
		}
    }
	
	if (!bom_found)
	{
        fseek(file, 0, SEEK_SET);
	}
	
}



// debugging purposes
void print_list(StringList & strList)
{
	
	int i = 0;
	for (std::string & line : strList)
	{
		printf("%d-%s\n", i, line.c_str() );
		i++;
	}
	printf("\n");
	
}



// ---- list content manipulation
void remove_translations(StringList & strList)
{
	
	for (std::string & line : strList)
	{
		
		// removes everything past the '='
		size_t equal_index = line.find('=');
		if (equal_index != std::string::npos)
			line.erase(equal_index);
		
		// removes any whitespaces
		trim(line);
		
	}
	
}



void remove_keys(StringList & strList)
{
	
	for (std::string & line : strList)
	{
		
		// removes everything BEFORE the '=', or if empty, keeps the former
		size_t equal_index = line.find('=');
		if (equal_index != std::string::npos)
		{
			
			std::string value;
			if (equal_index + 1 < line.length() )
			{
				value = std::string(line, equal_index + 1);
				trim(value);
			}
			
			if (!value.empty() )
				line = value;
			else
				line.erase(equal_index);
			
		}
		
		// removes any whitespaces
		trim(line);
		
	}
	
}



bool combine_translation(StringList & resultList, StringList & baseList, StringList & translationList)
{
	
	if (baseList.size() != translationList.size() )
	{
		printf("Warning: lists have diferent sizes: %d / %d\n", baseList.size(), translationList.size() );
		//return false;
	}
	
	resultList.clear();
	
	// builds the resulting list
	StringList::iterator it1 = baseList.begin();
    StringList::iterator it2 = translationList.begin();
	
	while (it1 != baseList.end() && it2 != translationList.end() ) 
	{
		
		std::string result_string;
		
		// doesn't add an '=' if empty line
		if ( !it1->empty() )
		{
			
			result_string = *it1 + " = ";
			
			// if already equal, leave blank
			if (*it1 != *it2)
				result_string += *it2;
			
		}
		
		resultList.push_back(result_string);
		
        ++it1;
        ++it2;
		
    }
	
	return true;
	
}


bool split_key_and_value(const std::string & line, std::string & key, std::string & value)
{
	
	// search for the '=' divisor to get the key
	size_t equal_index = line.find('=');
	if (equal_index != std::string::npos)
	{
		
		key = std::string(line, 0, equal_index);
		const size_t key_length = key.length();
		
		if (key_length > 0)
		{
			
			// get the value
			if (line.length() > equal_index + 1)
			{
				
				value = std::string(line, equal_index + 1);
				
				// remove white spaces
				trim(key);
				trim(value);
				
				if (!key.empty() && !value.empty() )
					return true;
			
			}
			
		}
		
	}
	
	return false;
	
}



bool match_case(std::string & word1, std::string & word2)
{
	
	if (!word1.empty() && !word2.empty() )
	{
		
		// dont mess with unicode characters
		if (word1[0] >= 65 && word1[0] <= 122 && 
			word2[0] >= 65 && word2[0] <= 122)
		{
			
			bool word1_is_uppercase = isupper(word1[0]);
			bool word2_is_uppercase = isupper(word2[0]);
			
			if (word1_is_uppercase && !word2_is_uppercase)
			{
				word2[0] = toupper(word2[0]);
				return true;
			}
			
			else if (!word1_is_uppercase && word2_is_uppercase)
			{
				word2[0] = tolower(word2[0]);
				return true;
			}
			
		}
		
	}
	
	return false;
	
}



void match_key_value_case(StringList & strList)
{
	
	int changes = 0;
	
	for (std::string & line : strList)
	{
		
		std::string key;
		std::string value;
		
		if (split_key_and_value(line, key, value) )
		{
			
			if (match_case(key, value) )
			{
				line = key + " = " + value;
				changes++;
			}
			
		}
		
	}
	
	printf("Changes: %d\n", changes);
	
}
// ----


// ---- file manipulation
bool load(StringList & strList, const char * filename)
{
	
	strList.clear();
	
	FILE * file = fopen(filename, "r");
	if (file)
	{
		
		skip_BOM(file);
		
		bool previous_empty = false;
		
		while( !feof(file) )
		{
			
			char linec[LINE_MAX_LENGTH];
			
			if (fgets(linec, LINE_MAX_LENGTH, file) )
			{
				
				std::string line(linec);
				trim(line);
				
				const bool current_empty = line.empty();
				
				#ifdef NO_DOUBLE_LINEBREAK
				if ( !(current_empty && previous_empty) )
				#endif
					strList.push_back(line);
				
				previous_empty = current_empty;
				
			}
			
		}
		
		printf("Read %d lines\n", strList.size() );
		
		fclose(file);
		
		return true;
		
	}
	
	printf("Error reading %s\n", filename);
	return false;
	
}



bool save(StringList & strList, const char * filename)
{
	
	FILE * file = fopen(filename, "w");
	if (file)
	{
		
		fprintf(file, "%c%c%c", '\xEF', '\xBB', '\xBF'); // utf-8 BOM
		
		for (std::string & line : strList)
		{
			fprintf(file, "%s\n", line.c_str() );
		}
		
		fclose(file);
		
		return true;
		
	}
	
	printf("Error creating %s\n", filename);
	return false;
	
}



bool get_line(FILE * file, std::string & str)
{
	
	std::string result;
	
	char linec[LINE_MAX_LENGTH];
	if (fgets(linec, LINE_MAX_LENGTH, file) )
	{
		
		result = linec;
		trim(result);
		
		if (!result.empty() )
		{
			str = result;
			return true;
		}
		
	}
	
	return false;
	
}
// ----




int main()
{
	
	int option = 0;
	
	// file names
	std::string translated = "target.txt";
	std::string keys = "keys.txt";
	std::string values = "values.txt";
	std::string exported = "result.txt";
	
	// try to load the filename list
	FILE * file = fopen("filenames.txt", "r");
	if (file)
	{
		
		get_line(file, translated);
		get_line(file, keys);
		get_line(file, values);
		get_line(file, exported);
		
		fclose(file);
		
	}
	
	
	// menu
	printf("Output filename: %s\n\n", exported.c_str() );
	printf("Select an option\n");
	printf("1- Remove translations from %s\n", translated.c_str() );
	printf("2- Remove keys from %s\n", translated.c_str() );
	printf("3- Create translation file from %s and %s\n", keys.c_str(), values.c_str() );
	printf("4- Fix case of %s\n", translated.c_str() );
	scanf("%d", &option);
	
	if (option == 1) // remove translations (and leave keys)
	{
		
		StringList lista1;
		if (load(lista1, translated.c_str() ) )
		{
			
			remove_translations(lista1);
			if ( save(lista1, exported.c_str() ) )
				printf("Saved %s suscessfully\n", exported.c_str() );
			
		}
		else printf("Error loading %s\n", translated.c_str() );
	
	}
	
	else if (option == 2) // remove keys (and leave translations)
	{
		
		StringList lista1;
		if (load(lista1, translated.c_str() ) )
		{
			
			remove_keys(lista1);
			if ( save(lista1, exported.c_str() ) )
				printf("Saved %s suscessfully\n", exported.c_str() );
			
		}
		else printf("Error loading %s\n", translated.c_str() );
	
	}
	
	else if (option == 3) // create translation
	{
		
		StringList lista_base;
		if ( load(lista_base, keys.c_str() ) )
		{
			
			StringList lista_traducoes;
			if ( load(lista_traducoes, values.c_str() ) )
			{
				
				StringList lista_resultado;
				if ( combine_translation(lista_resultado, lista_base, lista_traducoes) )
				{
					if ( save(lista_resultado, exported.c_str() ) )
						printf("Translation saved as %s suscessfully\n", exported.c_str() );
				}
				
			}
			else printf("Error loading %s\n", values.c_str() );
			
		}
		else printf("Error loading %s\n", keys.c_str() );
		
	}
	
	else if (option == 4) // fix case
	{
		
		StringList lista1;
		if (load(lista1, translated.c_str() ) )
		{
			
			match_key_value_case(lista1);
			if ( save(lista1, exported.c_str() ) )
				printf("Saved %s suscessfully\n", exported.c_str() );
			
		}
		else printf("Error loading %s\n", translated.c_str() );
	
	}
	
	else printf("No option\n");
	
	return 0;

}
