/*
 * Polyfit_test.cpp
 *
 *  Created on: 23 авг. 2017 г.
 *      Author: sergejkrivonos
 */
#define BOOST_TEST_MODULE FANN test
#include <boost/test/unit_test.hpp>
#include <fann.h>



BOOST_AUTO_TEST_CASE(FANN_test_simple)
{
    const unsigned int num_input = 2;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 3;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 1000;

    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, TEST_SRC_DIR "/xor.data", max_epochs, epochs_between_reports, desired_error);

    fann_save(ann, "xor_float.net");

    fann_destroy(ann);
}
