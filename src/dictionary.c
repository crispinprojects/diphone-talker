/***************************************************************************
 *   Author Alan Crispin                                                   *
 *   crispinalan@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/
#include <gtk/gtk.h> 
#include "dictionary.h"

//talk calendar dictionary

//dictionary key value pair
typedef struct pair {
  char *key;
  char *value;
} pair_t;


// Key strings must be in strcmp() sorted order!
pair_t table[] = {

//a-words
{"a","ey-ey ey-ey"}, 
{"afternoon","ae-ae ae-ae ae-f f-t t-er er-pau pau-er er-n n-uw uw-uw uw-n"},
{"am","ae-ae ae-ae ae-m"},
{"and","ah-n n-d"},
{"april","ey-ey ey-p p-r r-ah ah-l"},
{"august","aa-aa aa-g g-ah ah-s s-t t-pau"},

//b-words

//c-words
{"christmas","k-r r-ih ih-ih ih-s s-m m-ah ah-s"},

//d-words
{"day","d-ey ey-ey"},
{"date","d-ey ey-ey ey-t"},
{"dates","d-ey ey-ey ey-t t-s s-pau"},
{"december","d-ih ih-s s-eh eh-eh eh-m m-b b-er er-pau"},

//e-words
{"easter","iy-iy iy-s s-t t-er er-pau"},
{"eight","ey-ey ey-t"},
{"eighteen","eh-ey ey-t t-iy iy-iy iy-n"},
{"eighteenth","ey-ey ey-t t-iy iy-iy iy-n n-th th-pau"},
{"eighth","eh-ey ey-t t-th th-pau"},
{"eleven","ih-l l-eh eh-eh eh-v v-ah ah-n"},
{"eleventh","ih-l l-eh eh-eh eh-v v-ah ah-n n-th th-pau"},
{"evening","iy-iy iy-v v-n n-ih ih-ng ng-pau"},
{"event","ih-v v-eh eh-eh eh-n n-t"},

//f-words
{"february","f-eh eh-eh eh-b b-y y-ah ah-w w-eh eh-eh eh-r r-iy iy-pau"},
{"fifteen","f-ih ih-ih ih-f f-t t-iy iy-iy iy-n"},
{"fifteenth","f-ih ih-f f-t t-iy iy-iy iy-n n-th th-pau"},
{"fifth","f-ih ih-ih ih-f f-th th-pau"},
{"fifty","f-ih ih-ih ih-f f-t t-iy iy-pau"},
{"first","f-er er-er er-s s-t t-pau pau-t"},
{"five","f-ay ay-ay ay-v"},
{"for","f-ao ao-ao ao-r"},
{"forty","f-ao ao-ao ao-r r-t t-iy iy-pau"},
{"four","f-ao ao-ao ao-r"},
{"fourteen","f-ao ao-ao ao-r r-t t-iy iy-iy iy-n"},
{"fourteenth","f-ao ao-ao ao-r r-t t-iy iy-iy iy-n n-th th-pau pau-th"},
{"fourth","f-ao ao-ao ao-r r-th th-pau"},
{"friday","f-r r-ay ay-ay ay-d d-iy iy-pau"},

//g-words

//h-words
{"halloween","hh-ae ae-ae ae-l l-ah ah-w w-iy iy-iy iy-n"},
{"holiday","hh-aa aa-aa aa-l l-ah ah-d d-ey ey-ey"},
{"hour","aw-aw aw-r r-pau"},
{"hours","aw-aw aw-r r-z"},

//i-words
{"i","ay-ay ay-pau"},

//j-words
{"january","jh-ae ae-ae ae-n n-y y-uw uw-eh eh-eh eh-r r-iy iy-pau"},
{"july","jh-uw uw-uw uw-l l-ay ay-ay ay-pau"},
{"june","jh-uw uw-uw uw-n n-pau"},

//k-word

//l-words

//m-words
{"march","m-aa aa-aa aa-r r-ch ch-pau"},
{"may","m-ey ey-ey ey-pau"},
{"monday","m-ah ah-ah ah-n n-d d-iy iy-pau"},
{"morning","m-ao ao-ao ao-r r-n n-ih ih-ng"},

//n-words
{"nine","n-ay ay-ay ay-n"},
{"nineteen","n-ay ay-ay ay-n n-t t-iy iy-iy iy-n"},
{"nineteenth","n-ay ay-ay ay-n n-t t-iy iy-iy iy-n n-th th-pau pau-th"},
{"ninth","n-ay ay-ay ay-n n-th th-pau"},
{"november","n-ow ow-v v-eh eh-eh eh-m m-b b-er er-pau"},

//o-words
{"october","aa-k k-t t-ow ow-ow ow-b b-er"},
{"one","w-ah ah-ah ah-n"},

//p-words
{"pause","pau-pau"},
{"public","p-ah ah-ah ah-b b-l l-ih ih-k k-pau"},

//q-words

//r-words
{"reminder","r-iy iy-m m-ay ay-ay ay-n n-d d-er er-pau"},

//s-words
{"saturday","s-ae ae-ae ae-t t-ih ih-ih ih-d d-ey ey-ey"},
{"second","s-eh eh-eh eh-k k-ah ah-n n-d d-pau"},
{"september","s-eh eh-p p-t t-eh eh-eh eh-m m-b b-er er-pau"},
{"seven","s-eh eh-eh eh-v v-ah ah-n"},
{"seventeen","s-eh eh-eh eh-v v-ah ah-n n-t t-iy iy-iy iy-n"},
{"seventeenth","s-eh eh-eh eh-v v-ah ah-n n-t t-iy iy-iy iy-n n-th th-pau"},
{"seventh","s-eh eh-eh eh-v v-ah ah-n n-th th-pau"},
{"six","s-ih ih-ih ih-k k-s"},
{"sixteen","s-ih ih-k k-s s-t t-iy iy-iy iy-n"},
{"sixteenth","s-ih ih-k k-s s-t t-iy iy-iy iy-n n-th th-pau"},
{"sixth","s-ih ih-ih ih-k k-s s-th th-pau"},
{"speaking","s-p p-iy iy-iy iy-k k-ih ih-pau pau-ng"},
{"speech","s-p p-iy iy-iy iy-ch"},
{"sunday","s-ah ah-ah ah-n n-d d-ey ey-ey ey-pau pau-ey"},
{"synthesizer","s-ih ih-ih ih-n n-th th-ah ah-s s-ay ay-ay ay-er er-pau"},

//t-words
{"talking","t-ao ao-ao ao-k k-ih ih-ng"},
{"ten","t-eh eh-eh eh-n"},
{"tenth","t-eh eh-eh eh-n n-th th-pau"},
{"the","dh-ah ah-ah"},
{"third","th-er er-er er-d d-pau"},
{"thirteen","th-er er-er er-t t-iy iy-iy iy-n"},
{"thirteenth","th-er er-er er-t t-iy iy-iy iy-n n-th th-pau"},
{"thirtieth","th-er er-er er-t t-iy iy-eh eh-eh eh-th th-pau"},
{"thirty","th-er er-er er-d d-iy iy-iy iy-pau"},
{"three","th-r r-iy iy-iy"},
{"thursday","th-er er-er er-z z-d d-iy iy-pau"},
{"time","t-ay ay-ay ay-m"},
{"today","t-ah ah-d d-ey ey-ey"},
{"tuesday","t-uw uw-uw uw-z z-d d-iy iy-pau"},
{"twelfth","t-w w-eh eh-eh eh-l l-f f-th th-pau pau-th"},
{"twelve","t-w w-eh eh-eh eh-l l-v"},
{"twentieth","t-w w-eh eh-eh eh-n n-t t-iy iy-eh eh-eh eh-th th-pau"},
{"twenty","t-w w-eh eh-eh eh-n n-t t-iy iy-pau"},
{"two","t-uw uw-uw uw-pau"},

//u-words

//v-words

{"valentine","v-ae ae-ae ae-l l-ah ah-n n-t t-ay ay-ay ay-n"},
{"valentines","v-ae ae-ae ae-l l-ah ah-n n-t t-ay ay-ay ay-n n-z"},
{"valerie","v-ae ae-ae ae-l l-er er-iy iy-pau"},

//w-words
{"wednesday","w-eh eh-eh eh-n n-z z-d d-iy iy-pau"}, //or iy-iy
{"winter","w-ih ih-ih ih-n n-t t-er er-pau"},
{"world","w-er er-er er-l l-d"},

//x-words

//y-words
{"year","y-ih ih-ih ih-r"},


//z-words
{"zero","z-iy iy-iy iy-r r-ow ow-pau"},
	
};

static int compare_keys(const void *va, const void *vb) {
  const pair_t *a = va, *b = vb;
  return strcmp(a->key, b->key);
}

char* get_value(char *key) {
  pair_t key_pair[1] = {{key}};
  pair_t *pair = bsearch(key_pair, table,
      sizeof table / sizeof table[0], sizeof table[0], compare_keys);  
  
  if (pair ==NULL) return ""; //word not found
  else   return pair->value;
}


GList* word_to_diphones(const char* word_str) {
	
	//g_print("word_to_diphone: word_str =%s\n",word_str);
	
	GList *diphone_list=NULL;	//{"pau","pau-pau"},
	//diphone_list =g_list_concat(diphone_list,"pau-pau");
	char* word = g_ascii_strdown(word_str, -1); //convert to lower case
    //g_print("word_to_diphones: word =%s\n",word);
    char* dp_str ="";
    dp_str=get_value(word); 
	char buf[256];	
	strcpy(buf, dp_str);
	char *tok_str =strtok(buf, " ");	//space delimiter
	while(tok_str != NULL) {
		//g_print("tok_str = %s\n",tok_str);		
		char *copy_str = strdup(tok_str); //duplicate
		diphone_list = g_list_append(diphone_list, copy_str);		
		tok_str =strtok(NULL, " ");	
	}		
	return diphone_list;
}	
