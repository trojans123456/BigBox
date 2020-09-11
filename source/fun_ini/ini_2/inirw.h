/**
 * �ļ���inirw.h
 * �汾��1.0
 * ���ߣ�taoyuanmin@gmail.com
 *
 * ˵����ini�����ļ���д
 * 1��֧��;��#ע�ͷ��ţ�֧����βע�͡�
 * 2��֧�ִ�����'��"�ɶ�ƥ����ַ�������ȡʱ�Զ�ȥ���š������пɴ��������Ż�;#ע�ͷ���
 * 3��֧����section���section(����Ϊ��)��
 * 4��֧��10��16��8��������0x��ͷΪ16��������0��ͷΪ8���ơ�
 * 5��֧��section��key��=��ǰ����ո�
 * 6��֧��\n��\r��\r\n��\n\r���и�ʽ��
 * 7��������section��key��Сд����д��ʱ���´�Ϊ׼�����������Сд��
 * 8����������ʱ����section�������ڸý����һ����Ч���ݺ���ӣ��������ļ�β����ӡ�
 * 9��֧��ָ��key��������ɾ������ɾ���ü�ֵ������ע�͡�
 * 10�����Զ�������ʽ�����У��޸�ʱ��Ȼ������
 * 11���޸�ʱ����ԭע�ͣ���������ע�͡���βע��(����ǰ��ո�)��
 * 12���޸�ʱ����ԭ���С�����������Ҫ�Ǿ�������ԭ��ʽ��
 */
 

#ifndef _INI_RW_H_
#define _INI_RW_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ini__ ini_t;

/**
 * ����ini�ļ����ڴ�
 *
*/
ini_t *parser_ini_load(const char *filename);

/**
 * �ͷ�ini�ļ���ռ��Դ
*/
void parser_ini_free(ini_t *a_ini);

//��ȡ�ַ�������������
int iniGetString(ini_t *a_ini,const char *section, const char *key, char *value, int size, const char *defvalue);
//��ȡ����ֵ
int iniGetInt(ini_t *a_ini,const char *section, const char *key, int defvalue);
//��ȡ������
double iniGetDouble(ini_t *a_ini, const char *section, const char *key, double defvalue);

//�����ַ�������valueΪNULL����ɾ����key�����У�����ע��
int iniSetString(ini_t *a_ini,const char *section, const char *key, const char *value);
//��������ֵ��baseȡֵ10��16��8���ֱ��ʾ10��16��8���ƣ�ȱʡΪ10����
int iniSetInt(ini_t *a_ini,const char *section, const char *key, int value, int base);

#ifdef __cplusplus
}
#endif

#endif
