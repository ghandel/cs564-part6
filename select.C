#include "catalog.h"
#include "query.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;

    Status status;
    AttrDesc attrDesc;
    const char *filter;

    AttrDesc* projects = new AttrDesc[projCnt];

    for (int i = 0; i < projCnt; i++)
    {
    	status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, projects[i]);
    	if (status != OK)
    	{
    		return status;
    	}
    }

    if (attr != NULL)
    {
    	status = attrCat->getInfo(attr->relName, attr->attrName, attrDesc);
    	if (status != OK)
    	{
    		return status;
    	}
    }

    int reclen = 0;
    for (int i = 0; i < projCnt; i++)
    {
    	reclen += projects[i].attrLen;
    }

    int type = attrDesc.attrType;
    if (type == INTEGER)
    {
    	int val = atoi(attrValue);
    	filter = (char *)&val;
    }
    else if (type == FLOAT)
    {
    	float val = atof(attrValue);
    	filter = (char *)&val;
    }
    else
    {
    	filter = attrValue;
    }

    return ScanSelect(result, projCnt, projects, &attrDesc, op, filter, reclen);
}


const Status ScanSelect(const string & result, 
#include "stdio.h"
#include "stdlib.h"
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;

    Status status;
    RID rid;
    Record outRec;
    Record rec;

    InsertFileScan resultRel(result, status);
    if (status != OK)
    {
    	return status;
    }

    outRec.data = (char *) malloc(reclen);
    outRec.length - reclen;

    HeapFileScan scan(projNames->relName, status);

    if (status != OK)
    {
        return status;
    }

    status = scan.startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype)attrDesc->attrType, filter, op);
    if (status != OK)
    {
    	return status;
    }

    while (scan.scanNext(rid) == OK)
    {
    	status = scan.getRecord(rec);
    	if (status != OK)
    	{
    		return status;
    	}

    	int offset = 0;
    	for (int i = 0; i < projCnt; i++)
    	{
    		memcpy((char *)outRec.data + offset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
    		offset += projNames[i].attrLen;
    	}

    	RID outRid;
    	status = resultRel.insertRecord(outRec, outRid);
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
