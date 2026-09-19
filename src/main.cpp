#include <Arduino.h>
#include <aifes.h>
#include "models/MNISTNet/aifes_e_f32_fnn.h"
#include "models/MNISTNet/aifes_e_f32_weights.h"


void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\nAIfES MNIST inference");

    // Input feature vector
    static float input_data[196] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,74,202,223,250,91,0,0,0,0,0,0,0,0,0,92,56,38,71,224,30,0,0,0,0,0,0,0,0,0,0,0,26,235,69,0,0,0,0,0,0,0,0,0,0,13,211,177,1,0,0,0,0,0,0,0,0,0,0,21,249,172,1,0,0,0,0,0,0,0,0,0,0,0,20,197,60,0,0,0,0,0,0,0,10,39,0,0,12,194,94,0,0,0,0,0,0,0,92,106,0,3,168,240,30,0,0,0,0,0,0,0,34,208,124,205,254,130,0,0,0,0,0,0,0,0,2,153,254,219,91,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    // Output vector to hold the inference results
    static float output_data[10];

    // Run inference using the AIfES model
    aifes_e_f32_fnn_inference((float*)input_data, (float*)output_data);

    Serial.println("\nOutput:");

    for (int i = 0; i < 10; i++) {
        Serial.print("Class ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(output_data[i], 6);
    }

    int predicted_class = 0;
    for (int i = 1; i < 10; i++) {
        if (output_data[i] > output_data[predicted_class]) {
            predicted_class = i;
        }
    }

    Serial.print("Predicted class: ");
    Serial.println(predicted_class);
}

void loop() {

}
