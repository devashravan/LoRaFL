#include <Arduino.h>
#include <aifes.h>
#include "models/iris/aifes_e_f32_fnn.h"
#include "models/iris/aifes_e_f32_weights.h"


void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\nAIfES Iris inference");

    // Input feature vector
    float input_data[4] = {6.9, 3.2, 5.7, 2.3};

    // Output vector to hold the inference results
    float output_data[3];

    // Run inference using the AIfES model
    aifes_e_f32_fnn_inference((float*)input_data, (float*)output_data);

    Serial.println("\nOutput:");

    for (int i = 0; i < 3; i++) {
        Serial.print("Class ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(output_data[i], 6);
    }

    int predicted_class = 0;
    if (output_data[1] > output_data[predicted_class])
        predicted_class = 1;
    if (output_data[2] > output_data[predicted_class])
        predicted_class = 2;

    Serial.print("Predicted class: ");
    Serial.println(predicted_class);
}

void loop() {

}
