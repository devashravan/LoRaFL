#include <Arduino.h>
#include <aifes.h>
#include <CSVUtils.h>

#include "models/MNISTNet/aifes_e_f32_fnn.h"
#include "models/MNISTNet/aifes_e_f32_weights.h"


void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\nAIfES MNIST inference");

    if (!SD.begin(D8)) {
        Serial.println("SD initialization failed (check wiring, CS pin, FAT32 format)");
        return;
    }

    int32_t total_predictions = 0, correct_predictions = 0;
    for (uint8_t i = 1; i <= 10; ++i) {
        char filename[26] = "/node_1/test/batch_@@.csv";
        filename[19] = '0' + ((i/10) % 10);  // Replace '@' with the digit
        filename[20] = '0' + (i % 10);       // Replace '@' with the digit

        CSVFile csvFile(filename, 196, 1, true);
        if (!csvFile.isValid()) {
            Serial.print("Failed to initialize CSV file: ");
            Serial.println(filename);
            continue;
        }

        Serial.print("Reading file: ");
        Serial.println(filename);
        float pixels[196], true_digit[1], output_activations[10];
        for (uint32_t j = 0; j < csvFile.numRows(); ++j) {
            ReadStatus status = csvFile.readRow(j, pixels, true_digit);
            if (status != READ_OK) { 
                Serial.print("Error reading row ");
                Serial.print(j);
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

            uint8_t predicted_digit = 0;
            for (uint8_t k = 1; k < 10; ++k)
                if (output_activations[k] > output_activations[predicted_digit])
                    predicted_digit = k;
            
            if (predicted_digit == true_digit[0]) ++correct_predictions;
        }
        Serial.println("Done!");
        total_predictions += csvFile.numRows();
    }

    Serial.print("correct predictions: ");
    Serial.println(correct_predictions);
    Serial.print("total predictions: ");
    Serial.println(total_predictions);
    Serial.print("accuracy: ");
    Serial.print((float)correct_predictions / total_predictions * 100.0, 2);
    Serial.println("%");
}

void loop() {

}
