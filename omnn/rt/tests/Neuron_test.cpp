// Neuron_test.cpp
#define BOOST_TEST_MODULE Neuron test
#include <boost/test/included/unit_test.hpp>

#include "../Neuron.h"
#include <memory>

using namespace omnn::rt;

BOOST_AUTO_TEST_CASE(Neuron_gpu_flag) {
    Neuron n;
    BOOST_TEST(!n.isGPUEnabled());
    n.enableGPU(true);
    BOOST_TEST(n.isGPUEnabled());
    n.enableGPU(false);
    BOOST_TEST(!n.isGPUEnabled());
}

BOOST_AUTO_TEST_CASE(Neuron_connections) {
    Neuron a, b;
    BOOST_TEST(a.activationHistory().empty());
    a.connect(b, 0.25f);
    a.disconnect(b);
}

BOOST_AUTO_TEST_CASE(Neuron_process_message) {
    Neuron n;
    auto msg = std::make_shared<Message<double>>();
    msg->value = 1.0;
    msg->payload = {2.0, 3.0};
    n.process(msg);
    BOOST_TEST(n.predictNext() == n.predictNext());
}

BOOST_AUTO_TEST_CASE(Neuron_prediction_and_history) {
    Neuron n;
    n.setSum(0.5);
    BOOST_TEST(!n.activationHistory().empty());
    const auto before = n.activationHistory().size();
    double pred = n.predictNext();
    (void)pred;
    BOOST_TEST(n.activationHistory().size() == before);
}

BOOST_AUTO_TEST_CASE(Neuron_scale_schedule) {
    Neuron n;
    auto sched = n.defaultScaleSchedule();
    BOOST_TEST(!sched.empty());
    BOOST_TEST(sched.front() == 1);
}