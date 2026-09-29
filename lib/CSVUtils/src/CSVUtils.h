#ifndef CSVUTILS_H
#define CSVUTILS_H

#include <Arduino.h>
#include <SD.h>

/*
  Status codes returned by CSVFile operations
*/
enum ReadStatus {
    READ_OK = 0,
    INVALID_BUFFER,
    FILE_NOT_VALID,
    FILE_EOF,
    TOKEN_NOT_VALID
};

/*
  Provides row-indexed access to a CSV dataset stored on an SD card in O(1) time per row. The CSV
  file must have one data row per line, with comma-separated numeric fields, each feature field
  exactly `charsPerInField` characters wide and each output field exactly `charsPerOutField`
  characters wide (values must be pre-formatted to a fixed sign/decimal convention). The first line
  may optionally be a header. Each row must have exactly `numFeatures` feature columns followed by
  `numOutputs` output columns, in that order.
*/
class CSVFile {
    public:
        /*
          @param filename          path on SD card
          @param numFeatures       number of input feature columns
          @param numOutputs        number of output columns
          @param charsPerInField   number of characters per input feature field
          @param charsPerOutField  number of characters per output field
          @param hasHeader         true if the first line is a header to skip
        */
        CSVFile(
            const char* filename,
            uint8_t numFeatures,
            uint8_t numOutputs,
            uint8_t charsPerInField,
            uint8_t charsPerOutField,
            bool hasHeader = true
        );

        ~CSVFile();

        // Delete copy constructor and assignment operator to prevent copying
        CSVFile(const CSVFile&) = delete;
        CSVFile& operator=(const CSVFile&) = delete;

        /*
          @return true if the file was successfully opened and has valid structure, false otherwise.
          However, this doesn't guarantee that the contents of the file are valid.
        */
        bool isValid() const;

        /*
          @return Total number of data rows in the file.
        */
        uint32_t numRows() const;

        /*
          Reads the `rowNumber`th row of the CSV file into the provided feature and output buffers.

          @param rowNumber  0-indexed row number to read (relative to first data row)
          @param features   pointer to caller-provided buffer for feature values
          @param outputs    pointer to caller-provided buffer for output values

          @return ReadStatus code indicating success or failure
        */
        ReadStatus readRow(uint32_t rowNumber, float* features, float* outputs);

    private:
        File _file;
        char* _filename;
        uint8_t _numFeatures;
        uint8_t _numOutputs;
        uint8_t _charsPerInField;
        uint8_t _charsPerOutField;
        uint32_t _charsPerDataRow;
        bool _hasHeader;
        uint32_t _headerOffset;
        uint32_t _rowCount;
        bool _isValid;

        bool checkValidity();
};

#endif // CSVUTILS_H
