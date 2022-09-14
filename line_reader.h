#define MAX_SIZE 100


int count_tabs(char* line_of_code);
char* getNextTerm(const char* line_of_code, const int index_input, char* keyword_output, int* index_output);
char* skip_spaces(char* char_ptr);
int isNumeral(char c);
char* nextWordIs(char* keyword, char* ptr);
int isLetter(char c, int allow_underscores);

