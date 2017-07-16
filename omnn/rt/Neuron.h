//
// Created by Сергей Кривонос on 22.12.16.
//

#pragma once
#include <algorithm>
#include <vector>
#include <memory>
#include <numeric>
#include <functional>

namespace omnn {
    namespace rt {


      
        class Neuron
        {
        public:
            // bind receptors through all layers
            // potentially may be any source
            template <class InputsT> // put trait strategy here to override inputs source
            void bindInputs(const InputsT& inputs) {
                readInput = [&](size_t i) { return inputs[i]; };
                getInputsNum = [&]() { return inputs.size(); };

                for(auto i = getInputsNum(); i-->0; )
                {
                    Neuron* n = dynamic_cast<Neuron*>(&inputs[i]);
                    if (n)
                    {
                        n->subscribe([i,this](double d) {fix(i, d); });
                    }
                }
            }

            void setSum(double s)
            {
                sum = s;
                setActivation();
            }

            void setActivation()
            {
                double prev = activation;
                activation = sigmoid(sum);
                notify(activation - prev);
            }

            void notify(double d) {
                for (auto& s : subscribers)
                    s(d);
            }

            void fix(int n, double d) {
                sum += d*w[n];
                setActivation();
            }

            //double operator() {
            //    double res = 1.;
            //    res = std::accumulate(inputNeurons.begin(), inputNeurons.end(), res, [this](auto& p) {
            //        return p.first * (*p.second)();
            //    }
            //    return res;
            //}

            // using subscribe = subscribers.push_back; // maybe in next std
            void subscribe(std::function<void(double)> updated) {
                subscribers.push_back(updated);
            }

            /// add ref to file or http data to learn asynchronously to the network work
            void addToLearn(std::function<double(size_t)> readInput,
                std::function<size_t()> getInputsNum,
                double activationSupposed) {


            }
            virtual ~Neuron() {}

        private:
            double sum = 0;
            double activation = 0;
            std::vector<double> w; // weights 
            std::function<double(size_t)> readInput; // parent inputs here too?
            std::function<size_t()> getInputsNum;
            double lastSuccessfulValue = 0;
            double lastFailedValue = 0;
            std::vector<std::function<void(double)>> subscribers;
            std::function<double(double)> sigmoid = [](double d) {
                //double y = d;
                return d / (1 + d);
            };
        };

    }
}
