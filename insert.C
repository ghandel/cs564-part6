#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	Status status;
	RID rid;
	Record rec;
	int attrLen = 0;
	int recLen = 0;
	int type = 0;
	AttrDesc* atInfo;
	RelDesc relInfo;

	InsertFileScan insertScan(relation, status);
	if (status != OK)
	{
		return status;
	}

	status = relCat->getInfo(relation, relInfo);
	if (status != OK)
	{
		return status;
	}

	status = attrCat->getRelInfo(relation, attrLen, atInfo);
	if (status != OK)
	{
		return status;
	}

	for (int i = 0; i < attrLen; i++)
	{
		for (int j = 0; j < attrCnt; j++)
		{
			if (strcmp(atInfo[i].attrName, attrList[j].attrName) == 0)
			{
				recLen += atInfo[i].attrLen;
			}
		}
	}

	rec.data = (char*) malloc(recLen);
	rec.length = recLen;

	for (int i = 0; i < attrLen; i++)
	{
		for (int j = 0; j < attrCnt; j++)
		{
			if (strcmp(attrList[i].attrName, atInfo[j].attrName) == 0)
			{
				type = atInfo[j].attrType;
				if (type == INTEGER)
				{
					int value = atoi((char*)attrList[i].attrValue);
					memcpy((char*)rec.data + atInfo[j].attrOffset, &value, atInfo[j].attrLen);
				}
				else if (type == FLOAT)
				{
					float value = atof((char*)attrList[i].attrValue);
					memcpy((char*)rec.data + atInfo[j].attrOffset, &value, atInfo[j].attrLen);
				}
				else
				{
					memcpy((char*)rec.data + atInfo[j].attrOffset, (char*)attrList[i].attrValue, atInfo[j].attrLen - 1);
				}
			}
		}
	}

	status = insertScan.insertRecord(rec, rid);

	return status;

}

