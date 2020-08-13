#ifndef __JSON_CONFIG_H
#define __JSON_CONFIG_H



#ifdef __cplusplus
extern "C" {
#endif

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Int 3
#define cJSON_Double 4
#define cJSON_String 5
#define cJSON_Array 6
#define cJSON_Object 7

#define cJSON_IsReference 256

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

/* The cJSON structure: */
typedef struct cJSON
{
    struct cJSON *next, *prev; /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    int type; /* The type of the item, as above. */

    char *valuestring; /* The item's string, if type==cJSON_String */
    uint64 valueint; /* The item's number, if type==cJSON_Number */
    double valuedouble; /* The item's number, if type==cJSON_Number */
    int sign;   /* sign of valueint, 1(unsigned), -1(signed) */

    char *string; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSON;

typedef struct cJSON_Hooks
{
    void *(*malloc_fn)(size_t sz);
    void (*free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc, realloc and free functions to cJSON */
extern void cJSON_InitHooks(cJSON_Hooks* hooks);

/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
extern cJSON *cJSON_Parse(const char *value);
/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */

extern char *cJSON_Print(cJSON *item);

/*ÊÍ·Åprint´òÓ¡*/
extern void cJSON_FreePtr(char *print_ptr);

/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char *cJSON_PrintUnformatted(cJSON *item);
/* Delete a cJSON entity and all subentities. */
extern void cJSON_Delete(cJSON *c);

/* Returns the number of items in an array (or object). */
extern int cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern cJSON *cJSON_GetArrayItem(cJSON *array, int item);
/* Get item "string" from object. Case insensitive. */
extern cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
extern const char *cJSON_GetErrorPtr();

/* These calls create a cJSON item of the appropriate type. */
extern cJSON *cJSON_CreateNull();
extern cJSON *cJSON_CreateTrue();
extern cJSON *cJSON_CreateFalse();
extern cJSON *cJSON_CreateBool(int b);
extern cJSON *cJSON_CreateDouble(double num, int sign);
extern cJSON *cJSON_CreateInt(uint64 num, int sign);
extern cJSON *cJSON_CreateString(const char *string);
extern cJSON *cJSON_CreateArray();
extern cJSON *cJSON_CreateObject();

/* These utilities create an Array of count items. */
extern cJSON *cJSON_CreateIntArray(int *numbers, int sign, int count);
extern cJSON *cJSON_CreateFloatArray(float *numbers, int count);
extern cJSON *cJSON_CreateDoubleArray(double *numbers, int count);
extern cJSON *cJSON_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
extern void cJSON_AddItemToArray(cJSON *array, cJSON *item);
extern void cJSON_AddItemToArrayHead(cJSON *array, cJSON *item);    /* add by Bwar on 2015-01-28 */
extern void cJSON_AddItemToObject(cJSON *object, const char *string,
                cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
extern void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
extern void cJSON_AddItemReferenceToObject(cJSON *object, const char *string,
                cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern cJSON *cJSON_DetachItemFromArray(cJSON *array, int which);
extern void cJSON_DeleteItemFromArray(cJSON *array, int which);
extern cJSON *cJSON_DetachItemFromObject(cJSON *object, const char *string);
extern void cJSON_DeleteItemFromObject(cJSON *object, const char *string);

/* Update array items. */
extern void cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
extern void cJSON_ReplaceItemInObject(cJSON *object, const char *string,
                cJSON *newitem);

#define cJSON_AddNullToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))


#ifdef __cplusplus
}
#endif

#endif // E_JSON_H
