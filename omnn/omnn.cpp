
#include<omnn/omnn.h>

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

using namespace omnn::constexp;

typedef TrainBySamples<samples_t> LearnedXorPerceptron; // test
