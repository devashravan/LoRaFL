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

    int32_t correct_predictions = 0;

    Serial.println("Reading /test.csv from SD card...");
    char filename[10] = "/test.csv";
    unsigned long initStart = millis();
    CSVFile testDataset(filename, 49, 1, 8, 1, true);
    if (!testDataset.isValid()) {
        Serial.println("Failed to initialize test dataset");
        return;
    }
    unsigned long initEnd = millis();
    Serial.print("Dataset init time: ");
    Serial.print(initEnd - initStart);
    Serial.println(" ms");


    Serial.println("Running inference on test dataset...");
    float pixels[49], true_digit[1], output_activations[10];
    unsigned long inferenceStart = millis();
    for (uint32_t i = 0; i < testDataset.numRows(); ++i) {
        ReadStatus status = testDataset.readRow(i, pixels, true_digit);
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
                case TOKEN_NOT_VALID:
                    Serial.println("Token not valid");
                    break;
                case FILE_NOT_VALID:
                    Serial.println("File not valid");
                    break;
                default:
                    Serial.println("Unknown error");
            }
            continue;  // Skip to the next row
        }

        aifes_e_f32_fnn_inference(pixels, output_activations);

        uint8_t predicted_digit = 0;
        for (uint8_t j = 1; j < 10; ++j)
            if (output_activations[j] > output_activations[predicted_digit])
                predicted_digit = j;
        
        if (predicted_digit == true_digit[0]) ++correct_predictions;
    }
    unsigned long inferenceEnd = millis();

    Serial.print("Inference loop time: ");
    Serial.print(inferenceEnd - inferenceStart);
    Serial.println(" ms");


    Serial.print("correct predictions: ");
    Serial.println(correct_predictions);
    Serial.print("total predictions: ");
    Serial.println(testDataset.numRows());
    Serial.print("accuracy: ");
    Serial.print((float)correct_predictions / testDataset.numRows() * 100.0, 2);
    Serial.println("%");
}

void loop() {

}
