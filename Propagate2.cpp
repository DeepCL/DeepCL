#include "Propagate2.h"
#include "stringhelper.h"

using namespace std;

#undef STATIC
#undef VIRTUAL
#define STATIC
#define VIRTUAL

Propagate2::Propagate2( OpenCLHelper *cl, LayerDimensions dim, ActivationFunction const*fn ) :
        Propagate( cl, dim, fn )
            {
    kernel = cl->buildKernel( "../ClConvolve.cl", "convolve_imagecubes_float2", "-D " + fn->getDefineName() );

    std::string options = "-D " + fn->getDefineName();
    options += " -D gUpstreamBoardSize=" + toString(dim.inputBoardSize);
    options += " -D gUpstreamBoardSizeSquared=" + toString(square(dim.inputBoardSize));
    options += " -D gFilterSize=" + toString(dim.filterSize);
    options += " -D gFilterSizeSquared=" + toString(square(dim.filterSize));
    options += " -D gOutBoardSize=" + toString(dim.outputBoardSize);
    options += " -D gOutBoardSizeSquared=" + toString(square(dim.outputBoardSize));
    options += " -D gPadZeros=" + toString(dim.padZeros ? 1 : 0);
    options += " -D gNumOutPlanes=" + toString(dim.numFilters);
    options += " -D gMargin=" + toString(dim.padZeros ? dim.filterSize >> 1 : 0);
    options += " -D gHalfFilterSize=" + toString( dim.filterSize >> 1 );
    options += " -D gUpstreamNumPlanes=" + toString(dim.inputPlanes);
    kernel = cl->buildKernel( "../ClConvolve.cl", "convolve_imagecubes_float3", options );
}
VIRTUAL Propagate2::~Propagate2() {
    delete kernel;
}
VIRTUAL void Propagate2::propagate( int batchSize, CLWrapper *dataWrapper, CLWrapper *weightsWrapper, CLWrapper *biasWeightsWrapper,
    CLWrapper *resultsWrapper ) {
    kernel->in(batchSize);
    kernel->input( dataWrapper );
    kernel->input( weightsWrapper);
    kernel->output( resultsWrapper );
//        cout << "square(outputBoardSize) " << square( outputBoardSize ) << endl;
    kernel->localFloats( square( dim.inputBoardSize ) );
    kernel->localFloats( square( dim.filterSize ) * dim.numFilters );
    int workgroupsize = square( dim.outputBoardSize );
    int numWorkgroups = dim.numFilters;
    int globalSize = workgroupsize * numWorkgroups;
//    cout << " globalsize " << globalSize << " workgroupsize " << workgroupsize << endl;
    kernel->run_1d( globalSize, workgroupsize );
    cl->finish();
}
