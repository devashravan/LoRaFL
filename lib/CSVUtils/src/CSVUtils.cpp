#include "CSVUtils.h"

CSVFile::CSVFile(
    const char* filename,
    uint8_t numFeatures,
    uint8_t numOutputs,
    uint8_t charsPerField,
    bool hasHeader
) : _filename(nullptr),
    _numFeatures(numFeatures),
    _numOutputs(numOutputs),
    _charsPerField(charsPerField),
    _charsPerDataRow((numFeatures + numOutputs) * (charsPerField + 1)), // +1 for delimiter/newline
    _hasHeader(hasHeader),
    _headerOffset(0),
    _rowCount(0),
    _isValid(false)
{
    if (_charsPerField == 0 || _charsPerDataRow == 0) return;  // invalid configuration

    if (filename == nullptr) 
        return;
    _filename = (char*)malloc(strlen(filename) + 1);
    if (_filename == nullptr)
        return;  // allocation failed
    strncpy(_filename, filename, strlen(filename) + 1);
    _filename[strlen(filename)] = '\0';

    _isValid = checkValidity();
}

CSVFile::~CSVFile() {
    if (_file)
        _file.close();

    if (_filename != nullptr) {
        free(_filename);
        _filename = nullptr;
    }
}

bool CSVFile::isValid() const {
    return _isValid;
}

uint32_t CSVFile::numRows() const {
    return _rowCount;
}

ReadStatus CSVFile::readRow(uint32_t rowNumber, float* features, float* outputs) {
    if (features == nullptr || outputs == nullptr)
        return INVALID_BUFFER;

    if (!_isValid)
        return FILE_NOT_VALID;
    if (rowNumber >= _rowCount)
        return FILE_EOF;

    if (!_file)
        return FILE_NOT_VALID;
    _file.seek(_headerOffset + rowNumber * _charsPerDataRow);

    uint16_t totalFields = _numFeatures + _numOutputs;
    for (uint16_t fieldIdx = 0; fieldIdx < totalFields; ++fieldIdx) {
        char token[_charsPerField + 1];
        for (uint8_t i = 0; i < _charsPerField; ++i) {
            int c = _file.read();
            if (c == -1) return FILE_EOF; // unexpected end of file
            token[i] = char(c);
        }
        token[_charsPerField] = '\0';

        char* end;
        float val = strtof(token, &end);

        if (end == token || *end != '\0')
            return TOKEN_NOT_VALID;

        if (fieldIdx < _numFeatures) features[fieldIdx] = val;
        else if (fieldIdx < totalFields) outputs[fieldIdx - _numFeatures] = val;

        _file.read();  // Read the delimiter after the field
    }

    return READ_OK;
}

bool CSVFile::checkValidity() {
    _file = SD.open(_filename, FILE_READ);
    if (!_file)
        return false;

    // Skip the header line if present
    while (_hasHeader && _file.available()) {
        char c = _file.read();
        _headerOffset++;
        if (c == '\n') break;
    }

    uint8_t charIdx = 0;
    uint16_t fieldIdx = 0;
    uint16_t totalFields = _numFeatures + _numOutputs;
    uint32_t rowCount = 0;

    while (_file.available()) {
        int c = _file.read();

        if (c == ',' || c == '\n') {
            if (charIdx != _charsPerField)
                return false;  // wrong number of characters in this field
            charIdx = 0;

            fieldIdx++;
            if (c == '\n') {
                if (fieldIdx != totalFields)
                    return false;  // wrong number of columns in this row
                fieldIdx = 0;

                rowCount++;
            }
        } else {
            charIdx++;
            if (charIdx > _charsPerField)
                return false;  // field too long
        }
    }

    // Handle a final row with no trailing newline
    if (charIdx > 0 || fieldIdx > 0) {
        if (charIdx != _charsPerField)
            return false;
        fieldIdx++;
        if (fieldIdx != totalFields)
            return false;
        rowCount++;
    }

    _rowCount = rowCount;
    return true;
}
