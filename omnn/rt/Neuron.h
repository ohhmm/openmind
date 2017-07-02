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
            using ptr_t = std::shared_ptr<Neuron>;
            void bind(ptr_t neuron) {
                inputNeurons.push_back(std::make_pair(.5,neuron));
            }

            // bind receptors through all layers
            // potentially may be any source
            template <class InputsT> // put trait strategy here to override inputs source
            void bindInputs(const InputsT& inputs) {
                readInput = [&](size_t num) { return inputs[num]; };
                getInputsNum = [&]() { return inputs.size(); };
            }

            double operator() {
                double res = 1.;
                if(inputNeurons.size())
                    res = std::accumulate(inputNeurons.begin(), inputNeurons.end(), res, [this](auto& p) {
                        return p.first * (*p.second)();
                    }

                return res;
            }

        private:
            double val = 0.;
            size_t hitpoints = 100;
            std::vector<std::pair<double,ptr_t>> inputNeurons;
            std::function<double(size_t)> readInput;
            std::function<size_t()> getInputsNum;
            double lastSuccessfulValue = 0;
            double lastFailedValue = 0;
        };

    }
}