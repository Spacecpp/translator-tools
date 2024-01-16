#ifndef DICTIONARYMANAGER_H
#define DICTIONARYMANAGER_H

#include <map>
#include <string>
#include <deque>
#include <unordered_set>


class Dictionary 
{
	
	private:
		// 2 possible identification methods
		int id;
		std::string name;
		
		std::map<std::string, std::string> content; 
		std::unordered_set<std::string> missingContent; // for debugging purposes
		
		bool parse_line(const std::string & line);
	
	public:
		Dictionary();
		Dictionary(int id);
		Dictionary(const std::string & name, int id = 0);
		
		void reset();
		
		void add_translation(const std::string & key, const std::string & translation);
		std::string get_translation(const std::string & key);
		
		bool load(const char * filename);
		bool save(const char * filename);
		
		void print_words();
		size_t get_content_size_in_bytes();
		
		int get_id() { return id; }
		std::string get_name() { return name; } 
		int get_word_count() { return content.size(); }
		bool is_empty() { return content.empty(); }
		
		friend class DictionaryManager;
		
};



class DictionaryManager
{
	
	private:
		std::deque<Dictionary> content;
		
		// if said dictionary doesn't exists it creates
		Dictionary * get_dictionary_add(int id);
		Dictionary * get_dictionary_add(const std::string & name);
		
	public:
		void reset();
		void soft_reset(); // keep dictionaries, just empty them
		
		Dictionary * get_dictionary(int id);
		Dictionary * get_dictionary(const std::string & name);
		
		void add_dictionary(int id);
		void add_dictionary(const std::string & name, int id = 0);
		
		void add_translation(int id, const std::string & key, const std::string & translation);
		void add_translation(const std::string & name, const std::string & key, const std::string & translation);
		std::string get_translation(int id, const std::string & key);
		std::string get_translation(const std::string & name, const std::string & key);
		
		bool load(int id, const char * filename);
		bool load(int id, const std::string & filename);
		bool load(const std::string & name, const char * filename);
		bool load(const std::string & name, const std::string & filename);
		bool load(const char * filename);
		
		bool save_all();
		
		int get_word_count();
		void print_dictionary_list();
		void print_words();
		size_t get_content_size_in_bytes();
		
		int get_dictionary_count() { return content.size(); }
		
};

#endif
