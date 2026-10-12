#include <Arduino.h>
#include <aifes.h>
#include <CSVUtils.h>

#include "models/MNISTNet/aifes_f32_fnn.h"
#include "models/MNISTNet/aifes_f32_weights.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.printf("\nAIfES MNIST inference demo\n");

    // Configure the AIfES model for inference    
    aifes_f32_fnn_create_model();
    uint8_t inference_config_status = aifes_f32_fnn_configure_inference_model();
    if (inference_config_status != 0) {
        Serial.printf("Failed to configure inference model\n");
        return;
    }

    // Configure the SD card and the test dataset CSV file
    Serial.printf("Reading /test.csv from SD card...\n");
    if (!SD.begin(D8)) {
        Serial.printf("SD initialization failed (check wiring, CS pin, FAT32 format)\n");
        return;
    }

    char filename[10] = "/test.csv";
    CSVFile testDataset(filename, 49, 1, 8, 1, true, false);
    if (!testDataset.isValid()) {
        Serial.printf("Invalid test dataset\n");
        return;
    }

    // Run inference on the test dataset
    Serial.printf("Running inference on test dataset...\n");
    int32_t correct_predictions = 0;
    float pixels[49], true_digit[1], output_activations[10];
    unsigned long inferenceStart = millis();
    for (uint32_t i = 0; i < testDataset.numRows(); ++i) {
        ReadStatus status = testDataset.readRow(i, pixels, true_digit);
        if (status != READ_OK) { 
            Serial.printf("Error reading row %d: ", i);
            switch (status) {
                case INVALID_BUFFER:
                    Serial.printf("Invalid buffer\n");
                    break;
                case FILE_EOF:
                    Serial.printf("End of file reached\n");
                    break;
                case TOKEN_NOT_VALID:
                    Serial.printf("Token not valid\n");
                    break;
                case FILE_NOT_VALID:
                    Serial.printf("File not valid\n");
                    break;
                default:
                    Serial.printf("Unknown error\n");
            }
            continue;  // Skip to the next row
        }

        aifes_f32_fnn_inference(pixels, output_activations);

        uint8_t predicted_digit = 0;
        for (uint8_t j = 1; j < 10; ++j)
            if (output_activations[j] > output_activations[predicted_digit])
                predicted_digit = j;
        
        if (predicted_digit == true_digit[0]) ++correct_predictions;
    }
    unsigned long inferenceEnd = millis();

    Serial.printf(
        "Inference completed in %lu ms (%u samples)\n",
        inferenceEnd - inferenceStart,
        testDataset.numRows()
    );
    Serial.printf(
        "Correct predictions: %d / %d (accuracy: %.2f%%)\n",
        correct_predictions, testDataset.numRows(),
        (float)correct_predictions / testDataset.numRows() * 100.0
    );
    
    aifes_f32_fnn_free_inference_memory();
}

void loop() {

}
