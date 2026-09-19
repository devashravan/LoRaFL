#include <Arduino.h>
#include <aifes.h>
#include <CSVUtils.h>

#include "models/MNISTNet/aifes_e_f32_fnn.h"
#include "models/MNISTNet/aifes_e_f32_weights.h"


void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\nAIfES MNIST inference");
    Serial.println("Reading /test.csv from SD card and performing inference...");

    if (!SD.begin(D8)) {
        Serial.println("SD initialization failed (check wiring, CS pin, FAT32 format)");
        return;
    }

    CSVFile csvFile("/test.csv", 196, 1, true);
    float pixels[196], true_digit[1], output_activations[10];
    int32_t correct_predictions = 0;

    for (uint32_t i = 0; i < csvFile.numRows(); ++i) {
        ReadStatus status = csvFile.readRow(i, pixels, true_digit);
        if (status != READ_OK) { 
            Serial.print("Error reading row ");
            Serial.print(i);
            Serial.print(": ");
            switch (status) {
                case INVALID_BUFFER:
                    Serial.println("Invalid buffer");
                    break;
                case FILE_EOF:
                    Serial.println("End of file reached");
                    break;
                case FILE_NOT_VALID:
                    Serial.println("File not valid");
                    break;
                case TOKEN_NOT_VALID:
                    Serial.println("Token not valid");
                    break;
                case COLUMN_NUM_MISMATCH:
                    Serial.println("Column number mismatch");
                    break;
                default:
                    Serial.println("Unknown error");
            }
            continue;  // Skip to the next row
        }

        aifes_e_f32_fnn_inference(pixels, output_activations);

        int predicted_digit = 0;
        for (int j = 1; j < 10; ++j)
            if (output_activations[j] > output_activations[predicted_digit]) predicted_digit = j;

        if (predicted_digit == true_digit[0]) ++correct_predictions;
    }

    Serial.print("correct predictions: ");
    Serial.println(correct_predictions);
    Serial.print("total predictions: ");
    Serial.println(csvFile.numRows());
    Serial.print("accuracy: ");
    Serial.print((float)correct_predictions / csvFile.numRows() * 100.0, 2);
    Serial.println("%");
}

void loop() {

}
