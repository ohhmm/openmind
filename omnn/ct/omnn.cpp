
#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>

typedef boost::mpl::vector<
        boost::mpl::pair<
        boost::mpl::vector_c<int, 0, 0>, // inputs
        boost::mpl::vector_c<int, 0>     // target output
        >,
        boost::mpl::pair<
        boost::mpl::vector_c<int, 0, 1>, // inputs
        boost::mpl::vector_c<int, 1>     // target output
        >,
        boost::mpl::pair<
        boost::mpl::vector_c<int, 1, 0>, // inputs
        boost::mpl::vector_c<int, 1>     // target output
        >,
        boost::mpl::pair<
        boost::mpl::vector_c<int, 1, 1>, // inputs
        boost::mpl::vector_c<int, 0>     // target output
        >
    > samples_t;

//    // test ignored
//#include<omnn/omnn.h>
//using namespace omnn::constexp;
//typedef TrainBySamples<samples_t> LearnedXorPerceptron; // test
