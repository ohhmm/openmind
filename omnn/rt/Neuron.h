//
// Created by Сергей Кривонос on 22.12.16.
//

#pragma once
#include <algorithm>
#include <atomic>
#include <vector>
#include <memory>
#include <numeric>
#include <functional>
#include <deque>
#include <cstddef>
#include <type_traits>

namespace omnn {
    namespace rt {

        // Message used for asynchronous data flow between neurons.
        template <typename T = double>
        struct Message {
            T value = T{};
            std::size_t timestamp = 0;
            std::vector<T> payload;
        };

        template <typename T = double>
        class Neuron
        {
        public:
            using value_type = T;
            using MessagePtr = std::shared_ptr<Message<T>>;

            // bind receptors through all layers
            // potentially may be any source
            template <class InputsT> // put trait strategy here to override inputs source
            void bindInputs(const InputsT& inputs) {
                readInput = [&](size_t i) { return inputs[i]; };
                getInputsNum = [&]() { return inputs.size(); };

                for(auto i = getInputsNum(); i-->0; )
                {
                    using Elem = std::remove_reference_t<decltype(inputs[i])>;
                    if constexpr (std::is_convertible_v<Elem*, Neuron*>)
                    {
                        Neuron* n = static_cast<Neuron*>(&inputs[i]);
                        if (n)
                        {
                            n->subscribe([i,this](T d) {fix(i, d); });
                        }
                    }
                }
            }

            void setSum(T s)
            {
                sum = s;
                setActivation();
            }

            void setActivation()
            {
                T prev = activation;
                activation = sigmoid(sum);
                notify(activation - prev);
                recordActivation(activation);
            }

            void notify(T d) {
                for (auto& s : subscribers)
                    s(d);
            }

            void fix(int n, T d) {
                if (static_cast<std::size_t>(n) < w.size())
                    sum += d * w[static_cast<std::size_t>(n)];
                else
                    sum += d;
                setActivation();
            }

            void subscribe(std::function<void(T)> updated) {
                subscribers.push_back(updated);
            }

            /// add ref to file or http data to learn asynchronously to the network work
            void addToLearn(std::function<T(size_t)> readInputFn,
                std::function<size_t()> getInputsNumFn,
                T activationSupposed) {
                readInput = std::move(readInputFn);
                getInputsNum = std::move(getInputsNumFn);
                (void)activationSupposed;
            }

            // --- Unified spacetime autoregressive helpers ---
            void recordActivation(T value) {
                activationHistory_.push_back(value);
                while (activationHistory_.size() > temporalWindow_)
                    activationHistory_.pop_front();
            }

            T predictNext() const {
                T acc = bias_;
                for (std::size_t i = 0; i < activationHistory_.size(); ++i) {
                    const T weight = (i < temporalWeights_.size())
                        ? temporalWeights_[i]
                        : (temporalWeights_.empty() ? T{1} : temporalWeights_.back());
                    acc += activationHistory_[i] * weight;
                }
                return sigmoid(acc);
            }

            void process(MessagePtr msg) noexcept {
                if (!msg)
                    return;
                T delta = msg->value;
                if (!msg->payload.empty())
                    delta = std::accumulate(msg->payload.begin(), msg->payload.end(), T{});
                sum += delta;
                setActivation();
            }

            void connect(Neuron& target, T weight) {
                connections_.push_back({&target, weight});
            }

            void disconnect(const Neuron& target) {
                connections_.erase(std::remove_if(connections_.begin(), connections_.end(),
                    [&target](const Connection& c) { return c.target == &target; }),
                    connections_.end());
            }

            void enableGPU(bool use_gpu = true) {
                gpuEnabled_.store(use_gpu);
            }

            bool isGPUEnabled() const noexcept {
                return gpuEnabled_.load();
            }

            std::vector<int> defaultScaleSchedule() const {
                return {1, 2, 3, 4, 6, 8, 12, 16};
            }

            const std::deque<T>& activationHistory() const { return activationHistory_; }

            virtual ~Neuron() {}

        private:
            struct Connection {
                Neuron* target = nullptr;
                T weight = T{};
            };

            T sum = T{};
            T activation = T{};
            std::deque<T> w; // weights
            std::function<T(size_t)> readInput;
            std::function<size_t()> getInputsNum;
            T lastSuccessfulValue = T{};
            T lastFailedValue = T{};
            std::deque<std::function<void(T)>> subscribers;
            std::function<T(T)> sigmoid = [](T d) {
                return d / (T{1} + d);
            };

            std::atomic<bool> gpuEnabled_{false};
            std::deque<T> activationHistory_;
            std::size_t temporalWindow_ = 8;
            std::deque<T> temporalWeights_ = {T{0.5}, T{0.25}, T{0.125}, T{0.0625}};
            T bias_ = T{};
            std::deque<Connection> connections_;
        };

    }
}
