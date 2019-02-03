#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "fstream"
#include "iostream"

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {
    readPageIdx = 0;
    readRecordIdx = 0;

    numPages = 0;
    file = new File();
    page = new Page();

    rwMode = write;
    fileType = heap;
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
// int DBFile::Create (char *f_path, fType f_type, void *startup) {
    //DOUBT: const type
    //DOUBT: when will return be 0

    fileType = f_type;
    
    file->Open(0, f_path);

    return 1;
    
    //TODO: write metadata to a f_path.header file

}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
    FILE *tableFile = fopen(loadpath, "r");
    Record temp;

    int numRecords = 0;

    while(temp.SuckNextRecord(&f_schema, tableFile) == 1) {
        this->Add(temp);
        numRecords++;
    }
}

// assumes that a file at f_path already exists
int DBFile::Open (const char *f_path) {
    file->Open(1, f_path);
    return 1;

    //DOUBT When will it return 0
}

void DBFile::MoveFirst () {
    readPageIdx = 0;
    readRecordIdx = 0;
    page->EmptyItOut();
    file->GetPage(page, 0);
    rwMode = read;
}

// returns 1 on success and 0 on failure
// closes with rwMode = read
int DBFile::Close () {
    off_t fileLen = file->GetLength();

    if (rwMode == write) {
        rwMode = read;
        if (fileLen == 0) {
            file->AddPage(page, fileLen);    
        }
        else {
            file->AddPage(page, fileLen - 1);
        }
    }

    page->EmptyItOut();
    
    numPages = file->GetLength();
    // TODO: Write metadata into the auxilary file.

    file->Close();

    //DOUBT When could return value be 0?
    return 1;
}

void DBFile::Add (Record &rec) {
    if (rwMode == read) {
        off_t fileLen = file->GetLength();
        page->EmptyItOut(); //DOUBT Needed?
        
        file->GetPage(page, fileLen - 2);
        rwMode = write;
    }

    // Record* rec_copy;
    // rec_copy->Copy(&rec);

    if (!(page->Append(&rec))) {
        off_t fileLen = file->GetLength();
        if (fileLen == 0) {
            file->AddPage(page, fileLen);    
        }
        else {
            file->AddPage(page, fileLen - 1);
        }
        
        page->EmptyItOut();
        page->Append(&rec);
    }
    // rec_copy = NULL;
}

int DBFile::GetNext (Record &fetchme) {
    if (rwMode == write) {
        this->SwitchToReadMode(fetchme);
    }

    if (page->GetFirst(&fetchme)) {
        readRecordIdx++;
        return 1;
    }
    else {
        off_t fileLen = file->GetLength();
        readPageIdx++;
        readRecordIdx = 0;

        if (readPageIdx >= fileLen - 1) {
            return 0;
        }
        page->EmptyItOut();
        file->GetPage(page, readPageIdx);
        return this->GetNext(fetchme);
    }
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    if (rwMode == write) {
        this->SwitchToReadMode(fetchme);
    }
    ComparisonEngine comp;

    while (this->GetNext(fetchme)) {
        if (comp.Compare(&fetchme, &literal, &cnf))
            return 1;
    }

    return 0;
}

void DBFile::SwitchToReadMode(Record &fetchme) {
    off_t fileLen = file->GetLength();
        rwMode = read;
        if (fileLen == 0) {
            file->AddPage(page, fileLen);
        }
        else {
            file->AddPage(page, fileLen - 2);
        }
        page->EmptyItOut();
        file->GetPage(page, readPageIdx);

        int tempReadRecordIdx = 0;
        fileLen = file->GetLength();


        while (tempReadRecordIdx < readRecordIdx) {
            if (page->GetFirst(&fetchme)) {
                tempReadRecordIdx++;
            }
        }
}
