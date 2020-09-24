#include "jsoncpp.h"

using namespace ns_json;

JsonValue::JsonValue()
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    // m_pJsonData = cJSON_CreateObject();
}

JsonValue::JsonValue(const std::string& strJson)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    Parse(strJson);
}

JsonValue::JsonValue(const JsonValue* pJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    if (pJsonObject)
    {
        Parse(pJsonObject->ToString());
    }
}

JsonValue::JsonValue(const JsonValue& oJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    Parse(oJsonObject.ToString());
}

JsonValue::~JsonValue()
{
    Clear();
}

JsonValue& JsonValue::operator=(const JsonValue& oJsonObject)
{
    Parse(oJsonObject.ToString().c_str());
    return(*this);
}

bool JsonValue::operator==(const JsonValue& oJsonObject) const
{
    return(this->ToString() == oJsonObject.ToString());
}

bool JsonValue::AddEmptySubObject(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateObject();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty object error!");
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_listKeys.clear();
    return(true);
}

bool JsonValue::AddEmptySubArray(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateArray();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty array error!");
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_listKeys.clear();
    return(true);
}

bool JsonValue::GetKey(std::string& strKey)
{
    if (IsArray())
    {
        return(false);
    }
    if (m_listKeys.size() == 0)
    {
        cJSON* pFocusData = NULL;
        if (m_pJsonData != NULL)
        {
            pFocusData = m_pJsonData;
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            pFocusData = m_pExternJsonDataRef;
        }
        else
        {
            return(false);
        }

        cJSON *c = pFocusData->child;
        while (c)
        {
            m_listKeys.push_back(c->string);
            c = c->next;
        }
        m_itKey = m_listKeys.begin();
    }

    if (m_itKey == m_listKeys.end())
    {
        strKey = "";
        m_itKey = m_listKeys.begin();
        return(false);
    }
    else
    {
        strKey = *m_itKey;
        ++m_itKey;
        return(true);
    }
}

JsonValue& JsonValue::operator[](const std::string& strKey)
{
    std::map<std::string, JsonValue*>::iterator iter;
    iter = m_mapJsonObjectRef.find(strKey);
    if (iter == m_mapJsonObjectRef.end())
    {
        cJSON* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSON_Object)
            {
                pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSON_Object)
            {
                pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
            }
        }
        if (pJsonStruct == NULL)
        {
            JsonValue* pJsonObject = new JsonValue();
            m_mapJsonObjectRef.insert(std::pair<std::string, JsonValue*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            JsonValue* pJsonObject = new JsonValue(pJsonStruct);
            m_mapJsonObjectRef.insert(std::pair<std::string, JsonValue*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

JsonValue& JsonValue::operator[](unsigned int uiWhich)
{
    std::map<unsigned int, JsonValue*>::iterator iter;
    iter = m_mapJsonArrayRef.find(uiWhich);
    if (iter == m_mapJsonArrayRef.end())
    {
        cJSON* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSON_Array)
            {
                pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSON_Array)
            {
                pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
            }
        }
        if (pJsonStruct == NULL)
        {
            JsonValue* pJsonObject = new JsonValue();
            m_mapJsonArrayRef.insert(std::pair<unsigned int, JsonValue*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            JsonValue* pJsonObject = new JsonValue(pJsonStruct);
            m_mapJsonArrayRef.insert(std::pair<unsigned int, JsonValue*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

std::string JsonValue::operator()(const std::string& strKey) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSON_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if ((int64)pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if (pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSON_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSON_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

std::string JsonValue::operator()(unsigned int uiWhich) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSON_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if ((int64)pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if (pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSON_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSON_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

bool JsonValue::Parse(const std::string& strJson)
{
    Clear();
    m_pJsonData = cJSON_Parse(strJson.c_str());
    if (m_pJsonData == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    return(true);
}

void JsonValue::Clear()
{
    m_pExternJsonDataRef = NULL;
    if (m_pJsonData != NULL)
    {
        cJSON_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
    for (std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonArrayRef.clear();
    for (std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.begin();
                    iter != m_mapJsonObjectRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonObjectRef.clear();
    m_listKeys.clear();
}

bool JsonValue::IsEmpty() const
{
    if (m_pJsonData != NULL)
    {
        return(false);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::IsArray() const
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }

    if (pFocusData == NULL)
    {
        return(false);
    }

    if (pFocusData->type == cJSON_Array)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

std::string JsonValue::ToString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSON_PrintUnformatted(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSON_PrintUnformatted(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}

std::string JsonValue::ToFormattedString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSON_Print(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSON_Print(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}


bool JsonValue::Get(const std::string& strKey, JsonValue& oJsonObject) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSON_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool JsonValue::Get(const std::string& strKey, std::string& strValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool JsonValue::Get(const std::string& strKey, int32& iValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        iValue = (int32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        iValue = (int32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(const std::string& strKey, uint32& uiValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        uiValue = (uint32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        uiValue = (uint32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(const std::string& strKey, int64& llValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        llValue = (int64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        llValue = (int64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(const std::string& strKey, uint64& ullValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        ullValue = (uint64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        ullValue = (uint64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(const std::string& strKey, bool& bValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSON_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type;
    return(true);
}

bool JsonValue::Get(const std::string& strKey, float& fValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double)
    {
        fValue = (float)(pJsonStruct->valuedouble);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        fValue = (float)(pJsonStruct->valueint);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(const std::string& strKey, double& dValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double)
    {
        dValue = pJsonStruct->valuedouble;
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        dValue = (double)(pJsonStruct->valueint);
        return(true);
    }
    return(false);
}

bool JsonValue::IsNull(const std::string& strKey) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(const std::string& strKey, const JsonValue& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt(ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Add(const std::string& strKey, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::AddNull(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Delete(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON_DeleteItemFromObject(pFocusData, strKey.c_str());
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_listKeys.clear();
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, const JsonValue& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(const std::string& strKey, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::ReplaceWithNull(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonValue*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

int JsonValue::GetArraySize()
{
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            return(cJSON_GetArraySize(m_pJsonData));
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            return(cJSON_GetArraySize(m_pExternJsonDataRef));
        }
    }
    return(0);
}

bool JsonValue::Get(int iWhich, JsonValue& oJsonObject) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSON_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool JsonValue::Get(int iWhich, std::string& strValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool JsonValue::Get(int iWhich, int32& iValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        iValue = (int32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        iValue = (int32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(int iWhich, uint32& uiValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        uiValue = (uint32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        uiValue = (uint32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(int iWhich, int64& llValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        llValue = (int64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        llValue = (int64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(int iWhich, uint64& ullValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        ullValue = (uint64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        ullValue = (uint64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(int iWhich, bool& bValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSON_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type;
    return(true);
}

bool JsonValue::Get(int iWhich, float& fValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double)
    {
        fValue = (float)(pJsonStruct->valuedouble);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        fValue = (float)(pJsonStruct->valueint);
        return(true);
    }
    return(false);
}

bool JsonValue::Get(int iWhich, double& dValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double)
    {
        dValue = pJsonStruct->valuedouble;
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        dValue = (double)(pJsonStruct->valueint);
        return(true);
    }
    return(false);
}

bool JsonValue::IsNull(int iWhich) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(const JsonValue& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    unsigned int uiLastIndex = (unsigned int)cJSON_GetArraySize(pFocusData) - 1;
    for (std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= uiLastIndex)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool JsonValue::Add(const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(int iAnywhere, bool bValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Add(double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddNull()
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(const JsonValue& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    for (std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter++);
    }
    return(true);
}

bool JsonValue::AddAsFirst(const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(int iAnywhere, bool bValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddAsFirst(double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::AddNullAsFirst()
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Delete(int iWhich)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON_DeleteItemFromArray(pFocusData, iWhich);
    for (std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= (unsigned int)iWhich)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, const JsonValue& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)((uint64)llValue), -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::Replace(int iWhich, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonValue::ReplaceWithNull(int iWhich)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonValue*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

JsonValue::JsonValue(cJSON* pJsonData)
    : m_pJsonData(NULL), m_pExternJsonDataRef(pJsonData)
{
}




