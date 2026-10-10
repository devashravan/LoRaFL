#include "CSVUtils.h"

CSVFile::CSVFile(
    const char* filename,
    uint8_t numFeatures,
    uint8_t numOutputs,
    uint8_t charsPerInField,
    uint8_t charsPerOutField,
    bool hasHeader,
    bool checkValidity
) : _filename(nullptr),
    _numFeatures(numFeatures),
    _numOutputs(numOutputs),
    _charsPerInField(charsPerInField),
    _charsPerOutField(charsPerOutField),
    _charsPerDataRow(
        numFeatures * (charsPerInField + 1) +
        numOutputs  * (charsPerOutField + 1)
    ),
    _hasHeader(hasHeader),
    _checkValidity(checkValidity),
    _headerOffset(0),
    _rowCount(0),
    _isValid(false)
{
    if (_charsPerInField == 0 || _charsPerOutField == 0 || _charsPerDataRow == 0) return;

    if (filename == nullptr) 
        return;
    _filename = (char*)malloc(strlen(filename) + 1);
    if (_filename == nullptr)
        return;  // allocation failed
    strncpy(_filename, filename, strlen(filename) + 1);
    _filename[strlen(filename)] = '\0';

    _file = SD.open(_filename, FILE_READ);
    if (!_file)
        return;

    // Skip the header line if present
    while (_hasHeader && _file.available()) {
        int c = _file.read();
        _headerOffset++;
        if (c == '\n') break;
    }

    if (_checkValidity) {
        _checkFormat();
    } else {
        _rowCount = _file.size() / _charsPerDataRow;
        _isValid = true;
    }
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
    uint8_t charsPerField = 0;
    for (uint16_t fieldIdx = 0; fieldIdx < totalFields; ++fieldIdx) {
        charsPerField = (fieldIdx < _numFeatures) ? _charsPerInField : _charsPerOutField;
        char token[charsPerField + 1];
        for (uint8_t i = 0; i < charsPerField; ++i) {
            int c = _file.read();
            if (c == -1) return FILE_EOF; // unexpected end of file
            token[i] = char(c);
        }
        token[charsPerField] = '\0';

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

void CSVFile::_checkFormat() {
    uint8_t charIdx = 0;
    uint16_t fieldIdx = 0;
    uint16_t totalFields = _numFeatures + _numOutputs;
    uint32_t rowCount = 0;
    
    while (_file.available()) {
        int c = _file.read();
        uint8_t charsPerField = (fieldIdx < _numFeatures) ? _charsPerInField : _charsPerOutField;

        if (c == ',' || c == '\n') {
            if (charIdx != charsPerField)
                return;  // wrong number of characters in this field
            charIdx = 0;

            fieldIdx++;
            if (c == '\n') {
                if (fieldIdx != totalFields)
                    return;  // wrong number of columns in this row
                fieldIdx = 0;

                rowCount++;
            }
        } else {
            charIdx++;
            if (charIdx > charsPerField)
                return;  // field too long
        }
    }
    
    // Handle a final row with no trailing newline
    if (charIdx > 0 || fieldIdx > 0) {
        uint8_t charsPerField = (fieldIdx < _numFeatures) ? _charsPerInField : _charsPerOutField;
        if (charIdx != charsPerField)
            return;
        fieldIdx++;
        if (fieldIdx != totalFields)
            return;
        rowCount++;
    }
    
    _rowCount = rowCount;
    _isValid = true;
    return;
}
