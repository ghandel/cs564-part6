#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{

	Status status;
	RID rid;
	Record rec;
	AttrDesc relInfo;
	const char *filter;

	HeapFileScan scan(relation, status);
	if (status != OK)
	{
		return status;
	}

	status = attrCat->getInfo(relation, attrName, relInfo);
	if (status != OK)
	{
		return status;
	}

	if (type == INTEGER)
	{
		int value = atoi(attrValue);
		filter = (char *)&value;
	}
	else if (type == FLOAT)
	{
		float value = atof(attrValue);
		filter = (char *)&value;
	}
	else
	{
		filter = attrValue;
	}

	status = scan.startScan(relInfo.attrOffset, relInfo.attrLen, type, filter, op);

	while (scan.scanNext(rid) == OK)
	{
		status = scan.getRecord(rec);
		if (status != OK)
		{
			return status;
		}

		status = scan.deleteRecord();
		if (status != OK)
		{
			return status;
		}
	}

	if (status != FILEEOF)
	{
		return status;
	}

	status = scan.endScan();
	if (status != OK)
	{
		return status;
	}

	return OK;

}


