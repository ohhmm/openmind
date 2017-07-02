#pragma once
#include <assert.h>
#include <ratio>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/divides.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/for_each.hpp>
#include "boost/mpl/get_tag.hpp"
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/transform_view.hpp>
#define BOOST_RATIO_EXTENSIONS
#include <boost/ratio.hpp>
#include <boost/ratio/mpl/rational_c_tag.hpp>
#include <boost/ratio/mpl/times.hpp>

namespace omnn { // OpenMind Neural Network
namespace constexp { // Constant Experience (compile-time)


	namespace learn_functions {

	}

    template<typename T>
    struct get_value_times_weight{
    	typedef typename T::value_times_weight_t type;
    };

    template<class Neuron, class MSE>
    struct propagated_neuron {
    	using type = typename Neuron::template backpropogated<MSE>;
    };

    template<class Neuron, class InputWeightNo>
    using get_input_weight = boost::mpl::at<
    		typename Neuron::direct_input_weights,
			InputWeightNo>;

template <
	class Value = boost::ratio<0,1>,
    class Weight = boost::ratio<0,1>,
	typename InputNeuronSequence = boost::mpl::vector<>
>
struct Neuron
{
    typedef Weight weight_t;

    typedef boost::mpl::times<Value,Weight> value_times_weight_t;
    typedef boost::mpl::accumulate<
    			InputNeuronSequence,
				boost::ratio<0,1>,
				boost::mpl::plus<
					boost::mpl::_1,
					get_value_times_weight<boost::mpl::_2>
    				>
				> input_sum;

    using output = boost::mpl::times<input_sum,Weight>;

    using type = Neuron<Value, Weight, InputNeuronSequence>;

    template<class MSE>
    using backpropogated = boost::mpl::fold<
    		InputNeuronSequence,
			boost::mpl::vector<>,
			boost::mpl::push_back<
				boost::mpl::_1,
				propagated_neuron<type,MSE>
    			>
    		>;

    using direct_input_weights = boost::mpl::transform_view<
    		InputNeuronSequence,
			boost::mpl::times<
				weight_t,
				boost::mpl::accumulate<
					boost::mpl::range_c<int, 0,
						boost::mpl::size<InputNeuronSequence>::value
    					>,
					boost::ratio<0,1>,
					boost::mpl::plus<
						boost::mpl::_1,
						get_input_weight<
							boost::mpl::at<InputNeuronSequence, boost::mpl::_2>,
							boost::mpl::_1
							>
    					>
    				>
    			>
    		>;
};


template<
	class NeuronT,
	class WeightT,
	class NeuronsCountT,
	class WeightLearnFn
>
struct FirstLayer
{
	typedef NeuronT neuron_t;
	typedef WeightT weight_t;
	typedef NeuronsCountT neurons_count_t;

//	template<class T>
//	const T operator(T input_signals) {
//		static_assert(sizeof(T...) == NeuronsCountT, "check params count");
//		T output;
//
//		return
//	}
};

template<typename Values>
struct Layer
{
	using type = boost::mpl::transform_view<
					Values,
					Neuron<boost::mpl::_1>
					>;
	using size = boost::mpl::size<type>;
};

template <class Values>
struct Layer<Layer<Values>>
{
	using previous_layer = Layer<Values>;
	using type = boost::mpl::transform_view<
					previous_layer,
					Neuron<
						boost::mpl::_1,
						boost::ratio<0,1>,
						previous_layer
						>
					>;
};

template <
	class InnerLayer,
	class Size = boost::mpl::size<InnerLayer>>
struct OutputLayer : public Layer<InnerLayer>
{
	using inputs_num = Size;
	using type = boost::mpl::transform_view<
					boost::mpl::range_c<int, 0,Size::value>,
					Neuron<
						boost::mpl::_1,
						boost::ratio<0,1>,
						Layer<InnerLayer>
						>
					>;

	template <class MSE>
	using propagated = boost::mpl::transform_view<
			type,
			typename propagated_neuron<
				boost::mpl::_1,
				MSE
				>::type
			>;
};

BOOST_MPL_HAS_XXX_TRAIT_DEF(inputs_num)

template<class NeuralNet, class ForInputs, class TargetOutputs>
struct TrainFor
{
	template<typename T>
	struct get_output {
		typedef typename T::value_times_weight_t type;
	};

    using inputs_num = boost::mpl::size<ForInputs>;
//	using inputs_num = typename boost::mpl::if_<
//            has_inputs_num<NeuralNet>,
//            typename get_inputs_num<NeuralNet>::type,
//            boost::mpl::size<NeuralNet>>::type;
    //boost::mpl::if<has_inputs_num<NeuralNet>::type::value_type, typename NeuralNet::inputs_num, boost::mpl::size<NeuralNet>>::type;
	static constexpr auto net_arity_value = inputs_num::value;

	static_assert(true,
			//typename num::type::value==boost::mpl::size<TargetOutputs>::type::value,
			"outputs number has to be the same as targets number");

	using net_result = boost::mpl::transform_view<
			NeuralNet,
			get_output<boost::mpl::_1>>;
	using error = boost::mpl::transform_view<
			boost::mpl::range_c<int,0,net_arity_value>,
			boost::mpl::minus<
				boost::mpl::at<TargetOutputs,boost::mpl::_1>,
				boost::mpl::at<NeuralNet,boost::mpl::_1>
				>
			>;
	using sum_squere_errors = boost::mpl::accumulate<
			error,
			boost::ratio<0,1>,
			boost::mpl::plus<
				boost::mpl::_1,
				boost::mpl::times<
					boost::mpl::_2,
					boost::mpl::_2
					>
				>
			>;
	using mse = boost::mpl::divides<sum_squere_errors,inputs_num>;

	using propagated = typename NeuralNet::template propagated<mse>;

	using type = propagated;
};

template <class InputSequence, class TargetOutputValues,
	int LearningIterations = 1000>
struct Train
{
    using inputs = Layer<InputSequence>;
    using inputs_num = boost::mpl::size<InputSequence>;
    using outputs = OutputLayer<inputs, boost::mpl::size<TargetOutputValues>>;

    using train = boost::mpl::accumulate<
    		boost::mpl::range_c<int, 0, LearningIterations>,
			outputs,
			typename TrainFor<
				boost::mpl::deref<boost::mpl::_1>,
				inputs,
				TargetOutputValues
				>::propagated
			>;

//    template<class Input>
//    using Eval =

    using type = train;
};


template <class InputAndTargetOutputSequencesPairSequence, int LearningIterations = 1000>
struct TrainBySamples
{
	using first_pair = boost::mpl::at<InputAndTargetOutputSequencesPairSequence, boost::mpl::int_<0>>;
	using first_pair_inputs = typename first_pair::first;
	using first_pair_output_targets = typename first_pair::second;
	using inputs_num = boost::mpl::size<first_pair_inputs>;
	using outputs_num = boost::mpl::size<first_pair_output_targets>;

	using trained = boost::mpl::accumulate<
    		InputAndTargetOutputSequencesPairSequence,
			typename Train<first_pair_inputs, first_pair_output_targets, LearningIterations>::type,
			typename TrainFor< // train
                    boost::mpl::_1,
					boost::mpl::first<boost::mpl::_2>, // for these inputs
                    boost::mpl::second<boost::mpl::_2> // these target outputs
                    >::type
			// TODO : keep previous training consistent
            >;

    std::vector<double> operator()(const std::vector<double>& inputs)
    {
    	std::vector<double> outputs;
//    	outputs.reserve(outputs_num::value);
//
//    	assert(inputs.size()==inputs_num::value);
//
//    	boost::mpl::for_each<trained>([&](auto neuron){
//
//    	});

    	return outputs;
    }

};
    

}
}
