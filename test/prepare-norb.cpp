// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.


// read from the provided norb files, and :
// - create randomized version, for training
// - create smaller version, randomly sampled from the provided testing, for testing
//   (since, if only need testing accuracy accurate to 0.1%, only need 1000 testing samples.... )

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <random>

#include "NorbLoader.h"

using namespace std;

void prepareTraining( string norbDir ) {
    int N, numPlanes, imageSize;
    string imagesFilePath = norbDir + "/smallnorb-5x46789x9x18x6x2x96x96-training-dat.mat";
    string labelsFilePath = norbDir + "/smallnorb-5x46789x9x18x6x2x96x96-training-cat.mat";
    NorbLoader::getDimensions( imagesFilePath, &N, &numPlanes, &imageSize );
    unsigned char *training = new unsigned char[ N * numPlanes * imageSize * imageSize ];
    int *labels = new int[ N ];
    NorbLoader::load( imagesFilePath, training, labels );
//    int *labels = NorbLoader::loadLabels( labelsFilePath, N );
    // create random sequence of examples
    vector<int> sequence;
    sequence.reserve(N);
    for( int n = 0; n < N; n++ ) {
        sequence.push_back( n );
    }
    shuffle( sequence.begin(), sequence.end(), std::minstd_rand(0) ); // use seed 0, so repeatable
//    for( int i = 0; i < 10; i++ ) {
//        cout << i << "=" << sequence[i] << endl;
//    }
    // now sequence is shuffled, and has the numbers 0 to N, each exactly once
    // write out new, shuffled, training set
    int inputCubeSize = numPlanes * imageSize * imageSize;
    unsigned char *shuffledData = new unsigned char[N * inputCubeSize ];
    int *shuffledLabels = new int[N];
    for( int n = 0; n < N; n++ ) {
        int newLocation = sequence[n];
        memcpy( &(shuffledData[newLocation*inputCubeSize]), &(training[n * inputCubeSize]), sizeof(unsigned char) * inputCubeSize );
        shuffledLabels[newLocation] = labels[n];
    }

    NorbLoader::writeLabels( norbDir + "/training-shuffled-cat.mat", shuffledLabels, N );
    NorbLoader::writeImages( norbDir + "/training-shuffled-dat.mat", shuffledData, N, numPlanes, imageSize );

    delete[]training;
    delete[]labels;

    delete[] shuffledData;
    delete[] shuffledLabels;
}

void prepareTest( string norbDir, int numSamples ) {
    int N, numPlanes, imageSize;
    string testingStem = "smallnorb-5x01235x9x18x6x2x96x96-testing";
    unsigned char *training = NorbLoader::loadImages( norbDir + "/" + testingStem + "-dat.mat", &N, &numPlanes, &imageSize );
    int *labels = NorbLoader::loadLabels( norbDir + "/" + testingStem + "-cat.mat", N );

    // create sequence of numSamples sample indexes
    vector<int> sequence;
    sequence.reserve(numSamples);
    std::minstd_rand rand(0);
    for( int n = 0; n < numSamples; n++ ) {
        int sampleIndex = rand() % N;
        sequence.push_back( sampleIndex );
    }

    // write out samples
    int inputCubeSize = numPlanes * imageSize * imageSize;
    unsigned char *sampledData = new unsigned char[numSamples * inputCubeSize ];
    int *sampledLabels = new int[numSamples];
    for( int n = 0; n < numSamples; n++ ) {
        int sampleIndex = sequence[n];
        memcpy( &(sampledData[n*inputCubeSize]), &(training[sampleIndex * inputCubeSize]), sizeof(unsigned char) * inputCubeSize );
        sampledLabels[n] = labels[sampleIndex];
    }

    NorbLoader::writeLabels( norbDir + "/testing-sampled-cat.mat", sampledLabels, numSamples );
    NorbLoader::writeImages( norbDir + "/testing-sampled-dat.mat", sampledData, numSamples, numPlanes, imageSize );

    delete[]training;
    delete[]labels;

    delete[] sampledData;
    delete[] sampledLabels;
}

int main( int argc, char *argv[] ) {
    string norbDir = "../data/norb";

    cout << "shuffling training set...." << endl;
    prepareTraining( norbDir );
    cout << "sampling test set...." << endl;
    prepareTest( norbDir, 1000 );
    cout << "done" << endl;

    return 0;
}

