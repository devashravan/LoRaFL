/*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
  
  You can find more AIfES tutorials here:
  https://create.arduino.cc/projecthub/aifes_team 

  AIfES-Express: Configuration file automatically generated from Keras

  Paste the following code into your project and insert your inputs/features:
  ---------------------------------------------------------------------------  
    float input_data[49]; // AIfES input data
    float output_data[10]; // AIfES output data

    aifes_e_f32_fnn_inference((float*)input_data,(float*)output_data);
  ---------------------------------------------------------------------------

  Copyright (C) 2022  Fraunhofer Institute for Microelectronic Circuits and Systems.
  All rights reserved.
  AIfES-Converter is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <aifes.h>
#include "aifes_e_f32_weights.h"

#ifndef AIFES_E_F32_FNN
#define AIFES_E_F32_FNN

#define DATASETS 1
#define INPUTS 49
#define OUTPUTS 10

// Model parameters
#define FNN_LAYERS 3
uint32_t FNN_structure[FNN_LAYERS] = {49, 16, 10};
AIFES_E_activations FNN_activations[FNN_LAYERS - 1] = {AIfES_E_relu, AIfES_E_softmax};

// Training parameters
#define LOSS AIfES_E_crossentropy                   // PLACEHOLDER
#define OPTIMIZER AIfES_E_sgd                       // PLACEHOLDER
#define LEARNING_RATE 0.01f                         // PLACEHOLDER
#define SGD_MOMENTUM 0.0f                           // PLACEHOLDER
#define BATCH_SIZE 1                                // PLACEHOLDER
#define EPOCHS 1                                    // PLACEHOLDER
#define EPOCH_LOSS_PRINT_INTERVAL 1                 // PLACEHOLDER
#define EARLY_STOPPING AIfES_E_early_stopping_off   // PLACEHOLDER
#define EARLY_STOPPING_TARGET_LOSS 0.0f             // PLACEHOLDER

void print_loss(float loss)
{
    // PLACEHOLDER
    return;
}

// Model
aimodel_t model;

// Memory used for the model, inference and training
static void *model_memory = NULL;
static void *inference_memory = NULL;
static void *training_memory = NULL;

// Optimizer used for training
aiopti_t *optimizer = NULL;

AIFES_E_model_parameter_fnn_f32 FNN = {
    .layer_count = FNN_LAYERS,
    .fnn_structure = FNN_structure,
    .fnn_activations = FNN_activations,
    .flat_weights = (void*)aifes_e_f32_flat_weights
};

AIFES_E_training_parameter_fnn_f32 FNN_TRAIN = {
    .loss = LOSS,
    .optimizer = OPTIMIZER,
    .learn_rate = LEARNING_RATE,
    .sgd_momentum = SGD_MOMENTUM,
    .batch_size = BATCH_SIZE,
    .epochs = EPOCHS,
    .epochs_loss_print_interval = EPOCH_LOSS_PRINT_INTERVAL,
    .loss_print_function = print_loss,
    .early_stopping = EARLY_STOPPING,
    .early_stopping_target_loss = EARLY_STOPPING_TARGET_LOSS
};

int8_t aifes_e_f32_fnn_create_model()
{
    return AIFES_E_create_fnn_f32(&FNN, NULL, &model, &model_memory);
}

int8_t aifes_e_f32_fnn_create_trainable_model()
{
    return AIFES_E_create_fnn_f32(&FNN, &FNN_TRAIN, &model, &model_memory);
}

void aifes_e_f32_fnn_free_model_memory()
{
    AIFES_E_free_model_memory_fnn_f32(&model_memory);
}

int8_t aifes_e_f32_fnn_allocate_inference_memory()
{
    return AIFES_E_set_inference_mode_fnn_f32(&model, &inference_memory);
}

int8_t aifes_e_f32_fnn_inference(float* input_data, float* output_data)
{
    uint16_t input_shape[2] = {DATASETS, INPUTS};
    uint16_t output_shape[2] = {DATASETS, OUTPUTS};

    // Create input and output tensors
    aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);
    aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

    return AIFES_E_inference_fnn_f32(&input_tensor, &model, &output_tensor);
}

void aifes_e_f32_fnn_free_inference_memory()
{
    AIFES_E_free_inference_memory_fnn_f32(&inference_memory);
}

int8_t aifes_e_f32_fnn_allocate_training_memory()
{
    return AIFES_E_set_training_mode_fnn_f32(&FNN_TRAIN, &model, &optimizer, &training_memory);
}

int8_t aifes_e_f32_fnn_training(float* input_data, float* target_data, float* output_data)
{
    uint16_t input_shape[2] = {DATASETS, INPUTS};
    uint16_t output_shape[2] = {DATASETS, OUTPUTS};

    // Create input, target and output tensors
    aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);
    aitensor_t target_tensor = AITENSOR_2D_F32(output_shape, target_data);
    aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

    return AIFES_E_training_fnn_f32(&input_tensor,
                                    &target_tensor,
                                    &FNN_TRAIN,
                                    &model,
                                    optimizer,
                                    &output_tensor);
}

void aifes_e_f32_fnn_free_training_memory()
{
    AIFES_E_free_training_memory_fnn_f32(&optimizer, &training_memory);
}

#endif // AIFES_E_F32_FNN
