#include "CSVUtils.h"

#define MAX_TOKEN_LEN 16   // max token buffer length, including null terminator

CSVFile::CSVFile(
    const char* filename,
    uint8_t numFeatures,
    uint8_t numOutputs,
    bool hasHeader
) : _filename(nullptr),
    _offsets(nullptr),
    _rowCount(0),
    _numFeatures(numFeatures),
    _numOutputs(numOutputs),
    _hasHeader(hasHeader),
    _isValid(false)
{
    if (filename == nullptr) 
        return;

    _filename = (char*)malloc(strlen(filename) + 1);
    if (_filename == nullptr)
        return;  // allocation failed
    strncpy(_filename, filename, strlen(filename) + 1);
    _filename[strlen(filename)] = '\0';

    _isValid = buildIndex();
}

CSVFile::~CSVFile() {
    if (_file)
        _file.close();

    if (_filename != nullptr) {
        free(_filename);
        _filename = nullptr;
    }

    if (_offsets != nullptr) {
        free(_offsets);
        _offsets = nullptr;
    }
}

bool CSVFile::isValid() const {
    return _isValid;
}

uint32_t CSVFile::numRows() const {
    return _rowCount;
}

bool CSVFile::buildIndex() {
    _file = SD.open(_filename, FILE_READ);
    if (!_file)
        return false;

    bool isEmpty = true;
    uint32_t lineCount = 0;
    while (_file.available()) {
        char c = _file.read();
        isEmpty = false;
        if (c == '\n') lineCount++;
    }
    
    // Last line without a trailing newline counts as a line too
    if (!isEmpty) {
        _file.seek(_file.size() - 1);
        char lastChar = _file.read();
        if (lastChar != '\n') lineCount++;
    }

    if (lineCount == 0 && _hasHeader)
        return false;

    _rowCount = lineCount - _hasHeader;
    if (_rowCount == 0)
        return true;  // valid but empty dataset

    _offsets = (uint32_t*)malloc(sizeof(uint32_t) * _rowCount);
    if (_offsets == nullptr)
        return false;  // allocation failed

    uint32_t idx = 0;
    uint32_t pos = 0;
    _file.seek(0);

    if (_hasHeader) {
        // Skip past the header line
        while (_file.available()) {
            char c = _file.read();
            pos++;
            if (c == '\n') break;
        }
    }

    _offsets[idx++] = pos;

    while (_file.available() && idx < _rowCount) {
        char c = _file.read();
        pos++;
        if (c == '\n') {
            _offsets[idx++] = pos;
        }
    }

    return true;
}

ReadStatus CSVFile::readRow(uint32_t rowNumber, float* features, float* outputs) {
    if (features == nullptr || outputs == nullptr)
        return INVALID_BUFFER;

    if (!_isValid)
        return FILE_NOT_VALID;
    if (rowNumber >= _rowCount)
        return FILE_EOF;

    _file.seek(_offsets[rowNumber]);

    char token[MAX_TOKEN_LEN];
    uint8_t tokenLen = 0;
    uint16_t fieldIdx = 0;
    uint16_t totalFields = _numFeatures + _numOutputs;

    while (true) {
        int c = _file.read();

        if (c == '\r') continue;
        bool endOfField = (c == ',' || c == '\n' || c == -1);

        if (!endOfField) {
            if (tokenLen < MAX_TOKEN_LEN - 1) token[tokenLen++] = (char)c;
            else return TOKEN_NOT_VALID;  // token too long
            continue;
        }

        // End of a field
        token[tokenLen] = '\0';

        char* end;
        float val = strtof(token, &end);

        if (tokenLen == 0 || end == token || *end != '\0')
            return TOKEN_NOT_VALID;

        if (fieldIdx < _numFeatures) features[fieldIdx] = val;
        else if (fieldIdx < totalFields) outputs[fieldIdx - _numFeatures] = val;
        fieldIdx++;

        tokenLen = 0;

        if (c == '\n' || c == -1) break;
    }

    if (fieldIdx != totalFields)
        return COLUMN_NUM_MISMATCH;

    return READ_OK;
}