#include "DictionaryManager.h"

//#define DICTIONARY_MANAGER_USE_ALLEGRO 1
//#define DICTIONARY_MANAGER_DEBUG 1

#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
#include <allegro5/allegro.h>
#endif

#define LINE_MAX_LENGTH 512


// static
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


static std::string get_filename_from_path(std::string filename, bool keep_extension)
{
	
	const size_t last_slash_idx = filename.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
		filename.erase(0, last_slash_idx + 1);
	
	if (!keep_extension)
	{
		
		const size_t period_idx = filename.rfind('.');
		if (std::string::npos != period_idx)
			filename.erase(period_idx);
	
	}
	
	//printf("result: %s\n", filename.c_str() );
	return filename;
	
}



#ifdef DICTIONARY_MANAGER_USE_ALLEGRO

static void al_skip_BOM(ALLEGRO_FILE * file)
{
	
	bool bom_found = false;
	unsigned char bytes[3];
	
	size_t bytes_read = al_fread(file, bytes, 3);
    if (bytes_read == 3) 
	{
        
		if (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) // UTF-8 BOM
			bom_found = true;
		
    }
	
	if (!bom_found)
        al_fseek(file, 0, ALLEGRO_SEEK_SET);
	
}

#else

static void skip_BOM(FILE * file)
{
	
	bool bom_found = false;
	unsigned char bytes[3];
	
	size_t bytes_read = fread(bytes, 1, 3, file);
    if (bytes_read == 3) 
	{
        
		if (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) // UTF-8 BOM
			bom_found = true;
		
    }
	
	if (!bom_found)
        fseek(file, 0, SEEK_SET);
	
}

#endif

//----------------



Dictionary::Dictionary(): id(0)
{
}

Dictionary::Dictionary(int id): id(id)
{
}

Dictionary::Dictionary(const std::string & name, int id): id(id), name(name)
{
}


// private
bool Dictionary::parse_line(const std::string & line)
{
	
	bool success = false;
	
	// search for the '=' to get the key
	size_t equal_index = line.find('=');
	if (equal_index != std::string::npos)
	{
		
		std::string key = std::string(line, 0, equal_index);
		const size_t key_length = key.length();
		
		if (key_length > 0)
		{
			
			// get the value
			if (line.length() > equal_index + 1)
			{
				
				std::string value = std::string(line, equal_index + 1);
				
				// remove white spaces
				trim(key);
				trim(value);
				
				// add if valid
				if (!key.empty() && !value.empty() )
				{
					success = true;
					add_translation(key, value);
				}
				
				#ifdef DICTIONARY_MANAGER_DEBUG
				// if it had key but no value, put it on the missing translations list and do not report again
				else if (!key.empty() && value.empty() )
				{
					// ignore if //
					if (key.find("//") != 0)
					{
						// check if the string doesnt exists already
						if (missingContent.find(key) == missingContent.end() )
							missingContent.insert(key);
					}
				}
				#endif
			
			}
			
		}
		
	}
	
	return success;
	
}

//---


void Dictionary::reset()
{
	
	content.clear();
	
	#ifdef DICTIONARY_MANAGER_DEBUG
	missingContent.clear();
	#endif
	
}



void Dictionary::add_translation(const std::string & key, const std::string & translation) 
{
	
	#ifdef DICTIONARY_MANAGER_DEBUG
	if (content.find(key) != content.end() ) // warns for duplicates
		printf("Duplicate: %s\n", key.c_str() );
	#endif
	
	content[key] = translation;
	
}



std::string Dictionary::get_translation(const std::string & key) 
{
	
	if (content.find(key) != content.end() ) 
		return content[key];
	
	#ifdef DICTIONARY_MANAGER_DEBUG
	// if it is the first time the string was requested, warns once on console
	if (missingContent.find(key) == missingContent.end() )
	{
		printf("Missing: %s\n", key.c_str() );
		missingContent.insert(key);
	}
	#endif
	
	return key; // returns the key itself it no translation is available
	
}



bool Dictionary::load(const char * filename)
{
	
	reset();
	
	#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
	ALLEGRO_FILE * file = al_fopen(filename, "r");
	#else
	FILE * file = fopen(filename, "r");
	#endif
	
	if (file)
	{
		
		#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
		al_skip_BOM(file);
		#else
		skip_BOM(file);
		#endif
		
		#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
		while( !al_feof(file) )
		#else
		while( !feof(file) )
		#endif
		{
			
			char linec[LINE_MAX_LENGTH];
			
			#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
			if (al_fgets(file, linec, LINE_MAX_LENGTH) )
			#else
			if (fgets(linec, LINE_MAX_LENGTH, file) )
			#endif
			{
				
				std::string line_str(linec);
				bool success = parse_line(line_str);
				
				#ifdef DICTIONARY_MANAGER_DEBUG
				if (!success)
				{
					
					trim(line_str);
					if (!line_str.empty() )
						printf("Skipped: %s\n", line_str.c_str() );
				
				}
				#else
				(void)success;
				#endif
				
			}
			
			/*
			#ifdef DICTIONARY_MANAGER_DEBUG
			else printf("fgets false\n");
			#endif
			*/
		}
		
		#ifdef DICTIONARY_MANAGER_USE_ALLEGRO
		al_fclose(file);
		#else
		fclose(file);
		#endif
		
		//printf("%d words read\n", content.size() );
		
		return true;
		
	}
	
	return false;
	
}



bool Dictionary::save(const char * filename)
{
	
	FILE * file = fopen(filename, "w");
	if (file)
	{
		
		fprintf(file, "%c%c%c", '\xEF', '\xBB', '\xBF'); // utf-8 BOM
		
		for (const auto & current : content)
		{
			
			std::string key = current.first;
			std::string word = current.second;
			fprintf(file, "%s = %s\n", key.c_str(), word.c_str() );
			
		}
		
		fclose(file);
		return true;
		
	}
	
	return false;
	
}



void Dictionary::print_words()
{
	
	for (const auto & current : content)
	{	
		std::string word = current.second;
		printf("%s\n", word.c_str() );
	}
	
}



size_t Dictionary::get_content_size_in_bytes()
{
	
	size_t size = 0;
	for (const auto & current : content)
		size += current.second.size();
		
	return size;
	
}


// ------------------------------

void DictionaryManager::reset()
{
	content.clear();
}


void DictionaryManager::soft_reset()
{
	for (Dictionary & current : content)
		current.reset();
}



// private
Dictionary * DictionaryManager::get_dictionary_add(int id)
{
	
	if (Dictionary * target = get_dictionary(id) )
		return target;
	
	content.push_back( Dictionary(id) );
	return &content.back();
	
}



Dictionary * DictionaryManager::get_dictionary_add(const std::string & name)
{
	
	if (Dictionary * target = get_dictionary(name) )
		return target;
	
	int new_id = content.size();
	content.push_back( Dictionary(name, new_id) );
	return &content.back();
	
}
//-----



Dictionary * DictionaryManager::get_dictionary(int id)
{
	
	for (Dictionary & current : content)
	{
		if (current.id == id)
			return &current;
	}
	
	return NULL;
	
}


Dictionary * DictionaryManager::get_dictionary(const std::string & name)
{
	
	for (Dictionary & current : content)
	{
		if (current.name == name)
			return &current;
	}
	
	return NULL;
	
}



void DictionaryManager::add_dictionary(int id)
{
	
	if (get_dictionary(id) == NULL)
		content.push_back( Dictionary(id) );
}


void DictionaryManager::add_dictionary(const std::string & name, int id)
{
	
	if (get_dictionary(name) == NULL)
		content.push_back( Dictionary(name, id) );
	
}




void DictionaryManager::add_translation(int id, const std::string & key, const std::string & translation)
{
	
	Dictionary * dictionary = get_dictionary_add(id);
	if (dictionary)
		dictionary->add_translation(key, translation);
	
}


void DictionaryManager::add_translation(const std::string & name, const std::string & key, const std::string & translation)
{
	
	Dictionary * dictionary = get_dictionary_add(name);
	if (dictionary)
		dictionary->add_translation(key, translation);
	
}


// two "get" options
std::string DictionaryManager::get_translation(int id, const std::string & key)
{
	
	Dictionary * dictionary = get_dictionary(id);
	if (dictionary)
		return dictionary->get_translation(key);
	
	return key;
	
}


std::string DictionaryManager::get_translation(const std::string & name, const std::string & key)
{
	
	Dictionary * dictionary = get_dictionary(name);
	if (dictionary)
		return dictionary->get_translation(key);
	
	return key;
	
}


// load
bool DictionaryManager::load(int id, const char * filename)
{
	
	Dictionary * dictionary = get_dictionary_add(id);
	if (dictionary)
		return dictionary->load(filename);
	
	return false;
	
}



bool DictionaryManager::load(int id, const std::string & filename)
{
	return load(id, filename.c_str() );
}



bool DictionaryManager::load(const std::string & name, const char * filename)
{
	
	Dictionary * dictionary = get_dictionary_add(name);
	if (dictionary)
		return dictionary->load(filename);
	
	return false;
	
}



bool DictionaryManager::load(const std::string & name, const std::string & filename)
{
	return load(name, filename.c_str() );
}



bool DictionaryManager::load(const char * filename)
{
	
	Dictionary * dictionary = get_dictionary_add( get_filename_from_path(filename, false) );
	if (dictionary)
		return dictionary->load(filename);
	
	return false;
	
}



bool DictionaryManager::save_all()
{
	
	int save_count = 0;
	
	int count = 0;
	for (Dictionary & current : content)
	{
		
		std::string name = !current.name.empty() ? current.name : std::string("dictionary") + std::to_string(count);
		
		if (!current.is_empty() )
		{
			
			std::string filename = name + ".txt";
			
			bool success = current.save(filename.c_str() );
			
			if (success)
				save_count++;
			
			#ifdef DICTIONARY_MANAGER_DEBUG
			if (success)
				printf("Saved %s\n", filename.c_str() );
			else
				printf("Error saving %s\n", filename.c_str() );
			#endif
			
		}
		#ifdef DICTIONARY_MANAGER_DEBUG
		else 
		{
			printf("%s is empty, skipping\n", name.c_str() );
		}
		#endif
		
		count++;
		
	}
	
	return save_count > 0;
	
}



int DictionaryManager::get_word_count()
{
	
	int count = 0;
	for (Dictionary & current : content)
		count += current.get_word_count();
	
	return count;
	
}


void DictionaryManager::print_dictionary_list()
{
	
	for (Dictionary & current : content)
	{
		if (!current.name.empty() )
			printf("%s, ", current.name.c_str() );
		else
			printf("%d, ", current.id);
	}
	printf("\n");
	
}



void DictionaryManager::print_words()
{
	
	for (Dictionary & current : content)
	{
		
		printf("%s\n", current.name.c_str() );
		current.print_words();
		printf("\n");
		
	}
	
}



size_t DictionaryManager::get_content_size_in_bytes()
{
	
	size_t size = 0;
	for (Dictionary & current : content)
		size += current.get_content_size_in_bytes();
		
	return size;
	
}
