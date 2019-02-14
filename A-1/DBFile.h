#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

typedef enum {heap, sorted, tree} fType;
typedef enum {read, write} opMode;

// stub DBFile header..replace it with your own DBFile.h 

class DBFile {

private:
	opMode rwMode; //readWriteMode
	int readPageIdx;
	off_t numPages;
	int readRecordIdx;
	File *file;
	Page *page;
	fType fileType;


public:
	DBFile (); 

	// Creates a new file at the given fpath. If path already exists,
	// it gets overwritten.
	int Create (const char *fpath, fType file_type, void *startup);


	// int Create (char *fpath, fType file_type, void *startup);
	int Open (const char *fpath);

	// closes the DBFile
	int Close ();

	//bulk loads the DBFile instance from a text file given in loadpath
	void Load (Schema &myschema, const char *loadpath);

	void MoveFirst ();

	void Add (Record &addme);

	// switches from write mode to read mode
	void SwitchToReadMode(Record &fetchme);
	
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	

};
#endif
