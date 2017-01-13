#pragma once
#include <ratio>
#include <type_traits>
#include <utility>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/divides.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/range_c.hpp>
//#include<boost/mpl/stack.hpp>
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
    	using type = typename Neuron::beckpropagated;
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



//    template<class ...T>
//    double operator()(T... inputs)
//    {
//    	static_assert(sizeof(...inputs)==boost::mpl::size<InputNeuronSequence>);
//    	for(T& i : inputs)
//    	{
//    		boost::mpl::for_each
//    }
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


template<class NeuralNet, class TargetOutputs>
struct TrainFor
{
	template<typename T>
	struct get_output {
		typedef typename T::value_times_weight_t type;
	};

	static_assert(
			boost::mpl::size<NeuralNet>::value==boost::mpl::size<TargetOutputs>::value,
			"outputs number has to be the same as targets number");

	using num = boost::mpl::size<NeuralNet>;

	using net_result = boost::mpl::transform_view<
			NeuralNet,
			get_output<boost::mpl::_1>>;
	using error = boost::mpl::transform_view<
			boost::mpl::range_c<int,0,num::value>,
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
	using mse = boost::mpl::divides<sum_squere_errors,num>;

	using propagated = typename NeuralNet::template propagated<mse>;

	using type = propagated;
};

template <class InputSequence, class TargetOutputValues,
	int LearningIterations = 1000>
struct Train
{
    using inputs = Layer<InputSequence>;
    using outputs = OutputLayer<inputs, boost::mpl::size<TargetOutputValues>>;

    using train = boost::mpl::fold<
    		boost::mpl::range_c<int, 0, LearningIterations>,
			typename TrainFor<boost::mpl::_1, TargetOutputValues>::propagated
			>;

    using type = train;
};


template <class InputAndTargetOutputSequencesPairSequence, int LearningIterations = 1000>
struct TrainBySamples
{
    using trained = boost::mpl::accumulate<
    		InputAndTargetOutputSequencesPairSequence,
			Train<
				typename boost::mpl::at<InputAndTargetOutputSequencesPairSequence, boost::mpl::int_<0>>::first,
				typename boost::mpl::at<InputAndTargetOutputSequencesPairSequence, boost::mpl::int_<0>>::second,
				LearningIterations
				>,
			TrainFor<boost::mpl::_1,
				boost::mpl::second<boost::mpl::_2>
				>
			>;
};
    

}
}
