#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include <string>
#include "strmat.h"
#include "strmat_util.h"
#include "stree_ukkonen.h"
#include "strmat_stubs2.h"
#include "peter_io.h"

using namespace std;

static int stree_print_flag = TRUE;
static int stats_flag = TRUE;

#define NUM_STRINGS 4
#define UNIQUE_STRINGS 2

static int test1()
{
    int i, ok = 1;
    STRING *strings[NUM_STRINGS];

    for (i = 0; i < NUM_STRINGS; i++) {
        char title[129];
        char cstring[257];
        char dstring[257];
        CHAR_TYPE sequence[257];
        int length = 0;
        int j;
        sprintf(title, "string %02d", i+1);
 
        //sprintf(cstring, "hello_%02d,testing-%02d", i+1, i+1); 
        //sprintf(cstring, "ab%02dx", i+1);
        //sprintf(cstring, "%c%c%02d%c", 'a' + i, 'm' + i, i+1, 'x' + i);
        
        if (i % UNIQUE_STRINGS == 0) {
            sprintf(cstring, "abc");
        } else if (i % UNIQUE_STRINGS == 1) {
            sprintf(cstring, "ABC");
        } else if (i % UNIQUE_STRINGS == 2) {
            sprintf(cstring, "efg");
        } else if (i % UNIQUE_STRINGS == 3) {
            sprintf(cstring, "EFG");
        }
        strcpy(dstring, "xxx");
        strcat(dstring, cstring);
        strcat(dstring, "yyy");
           
        //printf("String %2d: '%s' '%s'\n", i, title, dstring);
#if 1
        length = 5;
        for (j = 0; j < length; j++) {
            dstring[j] = j + i % UNIQUE_STRINGS;
        }

        //length = strlen(dstring);
        //dstring[4] = 0;

        strings[i] = make_seqn(title, str_to_sequence(cstring, sequence, 257), length, stree_print_flag);
#else
        strings[i] = make_seq(title, dstring);
#endif
    }

    ok = strmat_ukkonen_build(strings, NUM_STRINGS, stats_flag, stree_print_flag);

    if (!ok) {
        fprintf(stderr, "strmat_ukkonen_build failed\n");
        return 0;
    }

   printf("Print any key to exit...");
   return 1;
}

static int num_calls = 0;
static BOOL base_test(int num_strings, int num_unique, int length, int max_char)
{
    int i;
    BOOL ok;
    STRING **strings = (STRING **)my_calloc(num_strings, sizeof(STRING *));
    CHAR_TYPE *cstring = (CHAR_TYPE *)my_calloc(length, sizeof(CHAR_TYPE));
    char title[129];
    
    printf("\n %3d: base_test(num_strings=%d, num_unique=%d, length=%d, max_char=%d)\n",
        num_calls++, num_strings, num_unique, length, max_char);

    for (i = 0; i < num_strings; i++) {
        int j;
        sprintf(title, "string %2d", i+1);
        
        for (j = 0; j < length; j++) {
            cstring[j] = (j + i % num_unique) % max_char;
        }

        strings[i] = make_seqn(title, cstring, length, stree_print_flag);
    }

    ok = strmat_ukkonen_build(strings, num_strings, stats_flag, stree_print_flag);

    if (!ok) {
        fprintf(stderr, "strmat_ukkonen_build failed\n");
    }

    for (i = 0; i < num_strings; i++) {
        free_seq(strings[i]);
    }
    free(strings);
    return ok;
}

static BOOL test2(int num_strings, int num_unique, int length, int max_char)
{
    return base_test(num_strings, num_unique, length, max_char);
}

static _int64 _last_val = 0;
static int range(int val, int min_val, int max_val)
{
    _last_val = (_last_val * 152700091 + val * 153102757) % 152500063;
    assert(_last_val >= 0);
    int result = min_val + _last_val % (max_val - min_val);
    assert(result >= 0);
    return result;
}

static BOOL test3()
{
    BOOL ok = TRUE;
    for (int i = 0; i < 10; i++) {
             
        int num_strings = range(i, 1, 100);
        int num_unique = range(i, 1, 50); 
        int length = range(i, 2, 1000);
        int max_char = range(i, 1, 255);

        if (!base_test(num_strings, num_unique, length, max_char)) {
            fprintf(stderr, "FAILURE !!!\n");
            ok = FALSE;
            break;
        }
    }
    return ok;
}

static BOOL test5()
{
    int num_strings = 4;
    int num_unique = 2; 
    int length = 200;
    int max_char = 155;
    BOOL ok = TRUE;
    
    for (int i = 0; i < 1000; i++) {
        stree_print_flag = (i == 999); 
        stats_flag = (i == 0) || stree_print_flag;
        if (!base_test(num_strings, num_unique, length, max_char)) {
            fprintf(stderr, "FAILURE !!!\n");
            ok = FALSE;
            break;
        }
    }
    return ok;
}

static string oki_dir("c:\\dev\\suffix\\find.page.markers\\hiperc\\");

static const char *oki_file_list[] = {
 "oki5650-pages=1-doc.prn",
 "oki5650-pages=1-pdf.prn",
 "oki5650-pages=17-pdf.prn",
 "oki5650-pages=2-doc.prn",
 "oki5650-pages=2-pdf.prn",
 "oki5650-pages=2.prn",
 "oki5650-pages=3-doc.prn",
 "oki5650-pages=3-pdf.prn",
 "pages=1-color-oki5100.prn",
 "pages=1-mono-oki5100.prn",
 "pages=2-blank-A4-landscape-oki5100.prn",
 "pages=2-blank-A4-oki5100.prn",
 "pages=2-blank-A5-oki5100.prn",
 "pages=4-blank-oki5100.prn",
 "pages=5-blank-oki5100.prn",
 "pages=5-simple-oki5100.prn"
};

static const int NUM_OKI_STRINGS = sizeof(oki_file_list)/sizeof(oki_file_list[0]);
static STRING **get_oki_file_strings()
{
    STRING **strings = (STRING **)my_calloc(sizeof(STRING *), NUM_OKI_STRINGS);

    for (int i = 0; i < NUM_OKI_STRINGS; i++) {
        string fname(oki_file_list[i]);
        string fpath;
        fpath = oki_dir + fname;
        cout << "path = " << fpath << endl;
        FileData file_data = read_file_data(fpath);
        strings[i] = make_seqn_from_bytes(fname.c_str(), file_data.get_data(), file_data.get_size(), stree_print_flag);
    }   
    return strings;
}

static BOOL test6()
{
    STRING **strings = get_oki_file_strings();
    BOOL ok = strmat_ukkonen_build(strings, NUM_OKI_STRINGS, stats_flag, stree_print_flag);
    if (!ok) {
        fprintf(stderr, "strmat_ukkonen_build failed\n");
    }
    for (int i = 0; i < NUM_OKI_STRINGS; i++) {
        free_seq(strings[i]);
    }
    free(strings);
    return ok;

}

int main(int argc, char *argv[]) 
{
   int test_num = 6;
    
   switch(test_num) {
    
    case 1:     // ASCII example
        test1(); 
        break;
    case 2:     // Simple binary example
        {
            int num_strings = 4;
            int num_unique = 2;
            int length = 6;
            int max_char = 4;
            test2(num_strings, num_unique, length, max_char);
        }
        break;
    case 3:     // Stress binary example
        stree_print_flag = FALSE;
        test3();
        break;
    case 4:     // big binary example
        stree_print_flag = FALSE;
        {
            int num_strings = 40;
            int num_unique = 20;
            int length = 10000;
            int max_char = 200;
            test2(num_strings, num_unique, length, max_char);
        }
        break;
    case 5:     // Many tests
        test5();
        break;
    case 6:     // Read binary strings from files
        test6();
        break;
   }

    printf("Print any key to exit...");
    _getch();
}