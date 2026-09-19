#ifndef CSVFILE_H
#define CSVFILE_H

#include <Arduino.h>
#include <SD.h>

/*
  Status codes returned by CSVFile operations
*/
enum ReadStatus {
    READ_OK = 0,
    INVALID_BUFFER,
    FILE_EOF,
    FILE_NOT_VALID,
    TOKEN_NOT_VALID,
    COLUMN_NUM_MISMATCH
};

/*
Provides row-indexed access to a CSV dataset stored on an SD card in O(1) time per row, once the
file has been indexed. The CSV file must have one data entry per line, with comma-separated
numeric fields, the first line may optionally be a header. Each row must have exactly `numFeatures`
feature columns followed by `numOutputs` output columns, in that order.
*/
class CSVFile {
    public:
        /*
          @param filename     path on SD card
          @param numFeatures  number of input feature columns
          @param numOutputs   number of output columns
          @param hasHeader    true if the first line is a header to skip
        */
        CSVFile(
            const char* filename,
            uint8_t numFeatures,
            uint8_t numOutputs,
            bool hasHeader = true
        );

        ~CSVFile();

        /*
          @return true if the file was successfully opened and indexed, false otherwise. However,
          this doesn't guarantee that the contents of the file are valid.
        */
        bool isValid() const;

        /*
          @return Total number of data rows found during indexing (excluding header).
        */
        uint32_t numRows() const;

        /*
          Reads the `rowNumber`th row of the CSV file into the provided feature and output buffers.

          @param rowNumber  0-indexed row number to read (relative to first data row)
          @param features   pointer to caller-provided buffer for feature values
          @param outputs    pointer to caller-provided buffer for output values

          @return ReadStatus code indicating success or failure
        */
        ReadStatus readRow(uint32_t rowNumber, int32_t* features, int32_t* outputs);

    private:
        File _file;
        char* _filename;
        uint32_t* _offsets;
        uint32_t _rowCount;
        uint8_t _numFeatures;
        uint8_t _numOutputs;
        bool _hasHeader;
        bool _isValid;

        /*
          Scans the CSV file once when the CSVFile object is constructed, stores row count and
          builds an index of the starting byte of each data row in the file to allow O(1) access
          to any row.
        */
        bool buildIndex();

};

#endif // CSVFILE_H