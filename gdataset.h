// ============ GDATASET_H ================

#ifndef GDATASET_H
#define GDATASET_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <execinfo.h>
#include <errno.h>
#include <string.h>
#include "pberr.h"
#include "gset.h"
#include "pbmath.h"
#include "pbjson.h"
#include "pbfilesys.h"
#include "neuranet.h"

// Define locally the needed types and functions for libraries that were
// not included to allow the user to include only what's needed for her
// application
#ifndef GENBRUSH_H
typedef struct GenBrush GenBrush; 
typedef enum GBScaleMethod {GBScaleMethod_Default} GBScaleMethod;
GenBrush* GBCreateFromFile(const char* const fileName);
GenBrush* GBScale(const GenBrush* const that, 
  const VecShort2D* const dim, const GBScaleMethod scaleMethod);
void GBFree(GenBrush** that);
VecShort2D* GBDim(const GenBrush* const that);
void _GDSGenBrushPairRemoveAllSample();
void _GDSGenBrushPairAddSample();
#endif

// ================= Define ==================

typedef enum GDataSetType {
  GDataSetType_VecFloat, GDataSetType_GenBrushPair
} GDataSetType;

// ================= Data structures ===================

typedef struct GDataSet {
  // Name of the data set
  char* _name;
  // Description of the data set
  char* _desc;
  // Type of set
  GDataSetType _type;
  // Nb of samples
  long _nbSample;
  // Set of samples
  GSet _samples;
  // Dimensions of each sample, they must have all the same dimension
  // e.g.: 
  // if samples are VecFloat<3> then _dim = VecShort<1>[3]
  // if samples are GenBrush then _dim = VecShort<2>[width, height]
  VecShort* _sampleDim;
  short _nbInputs;
  short _nbOutputs;
  // Splitting of samples
  VecLong* _split;
  // Sets of splitted samples
  GSet* _categories;
  // Iterators on the sets of splitted samples
  GSetIterForward* _iterators;
} GDataSet;

typedef struct GDataSetVecFloat {
  // Generic GDataSet
  GDataSet _dataSet;
} GDataSetVecFloat;

typedef struct GDataSetGenBrushPair {
  // Generic GDataSet
  GDataSet _dataSet;
  // Format of images
  char* _format;
  // Dimensions of images
  VecShort2D _dim;
  // Nb of mask per img
  int _nbMask;
  // Path to the config file of the data set
  char* _cfgFilePath;
} GDataSetGenBrushPair;

#define GDS_NBMAXMASK 100
typedef struct GDSFilePathPair {
  char* _path[1 + GDS_NBMAXMASK];
} GDSFilePathPair;

typedef struct GDSGenBrushPair {
  GenBrush* _img;
  GenBrush* _mask[GDS_NBMAXMASK];
} GDSGenBrushPair;

typedef struct GDSVecFloatCSVImporter {
  // Size (nb of lines) of the header
  unsigned int _sizeHeader;
  // Separator
  char _sep;
  // Number of column in CSV
  unsigned int _nbCol;
  // Size of the resulting sample
  unsigned int _sizeSample;
  // Field converter function
  void (*_converter)(
    int col, 
    char* val, 
    VecFloat* sample);
} GDSVecFloatCSVImporter;

// ================ Functions declaration ====================

// Create a new GDataSet of type 'type'
GDataSet GDataSetCreateStatic(GDataSetType type);

// Free the memory used by a GDataSet
void GDataSetFreeStatic(GDataSet* const that);

// Load the GDataSet 'that' from the stream 'stream'
// Return true if the GDataSet could be loaded, false else
bool GDataSetLoad(GDataSet* that, FILE* const stream);
bool _GDSLoad(GDataSet* that, FILE* const stream);

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetDecodeAsJSON(GDataSet* that, const JSONNode* const json);

// Create a new GDataSetVecFloat defined by the file at 'cfgFilePath'
GDataSetVecFloat GDataSetVecFloatCreateStaticFromFile(
  const char* const cfgFilePath);

// Reset the categories of the GDataSet 'that' to one unshuffled
// category
void GDSResetCategories(GDataSet* const that);

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetVecFloatDecodeAsJSON(GDataSetVecFloat* that, 
  const JSONNode* const json);

// Function which return the JSON encoding of 'that' 
JSONNode* GDataSetVecFloatEncodeAsJSON(
  const GDataSetVecFloat* const that);

// Function which return the JSON encoding of the category 'iCat'
// of'that' 
JSONNode* GDataSetVecFloatEncodeCategoryAsJSON(
  const GDataSetVecFloat* const that,
             const unsigned int iCat);

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetGenBrushPairDecodeAsJSON(GDataSetGenBrushPair* that, 
  const JSONNode* const json);
  
// Free the memory used by a GDataSetVecFloat
void GDataSetVecFloatFreeStatic(GDataSetVecFloat* const that);

// Create a new GDataSetGenBrushPair defined by the file at 'cfgFilePath'
GDataSetGenBrushPair GDataSetGenBrushPairCreateStaticFromFile(
  const char* const cfgFilePath);

// Free the memory used by a GDataSetGenBrushPair
void GDataSetGenBrushPairFreeStatic(GDataSetGenBrushPair* const that);

// Get the total number of samples in the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetSize(const GDataSet* const that);

// Get the number of input values in one sample of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
short _GDSGetNbInputs(const GDataSet* const that);

// Get the number of output values in one sample of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
short _GDSGetNbOutputs(const GDataSet* const that);

// Set the number of input values in one sample of the GDataSet 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif 
void _GDSSetNbInputs(GDataSet* const that, const short nb);

// Set the number of output values in one sample of the GDataSet 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif 
void _GDSSetNbOutputs(GDataSet* const that, const short nb);

// Get the number of masks in the GDataSet 'that'
int _GDSGetNbMask(const GDataSet* const that);

// Get the number of masks in the GDataSetGenBrushPair 'that'
#if BUILDMODE != 0
static inline
#endif 
int GDSGetNbMaskGenBrushPair(const GDataSetGenBrushPair* const that);

// Get the total number of samples in the GDataSet 'that' for the 
// category 'iCat'. Return 0 if the category doesn't exists
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetSizeCat(const GDataSet* const that, const long iCat);

// Split the samples of the GDataSet 'that' into several categories
// defined by 'cat'. The dimension of 'cat' gives the number of 
// categories and the value for each dimension of 'cat' gives the 
// number of samples in the corresponding category. For example <3,4> 
// would mean 2 categories with 3 samples in the first one and 4 
// samples in the second one. There must me at least as many samples 
// in the data set as the sum of samples in 'cat'.
// Each category must have at least one sample.
// If 'that' was already splitted the previous splitting is discarded.
void _GDSSplit(GDataSet* const that, const VecLong* const cat);

// Unsplit the GDataSet 'that', i.e. after calling GDataSetUnsplit 'that' 
// has only one category containing all the samples
#if BUILDMODE != 0
static inline
#endif 
void _GDSUnsplit(GDataSet* const that);

// Shuffle the samples of the GDataSet 'that'.
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffle(GDataSet* const that);

// Shuffle the samples of the category 'iCat' of the GDataSet 'that'.
// Reset the iterator of the category
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffleCat(GDataSet* const that, const long iCat);

// Shuffle the samples of all the categories of the GDataSet 'that'.
// Reset the iterator of the categories
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffleAll(GDataSet* const that);

// Get the name of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const char* _GDSName(const GDataSet* const that);

// Get the description of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const char* _GDSDesc(const GDataSet* const that);

// Get the type of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
GDataSetType _GDSGetType(const GDataSet* const that);

// Get the number of categories of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetNbCat(const GDataSet* const that);

// If there is a next sample move to the next sample of the category 
// 'iCat' and return true, else return false
#if BUILDMODE != 0
static inline
#endif 
bool _GDSStepSample(const GDataSet* const that, const long iCat);

// Reset the iterator on category 'iCat' of the GDataSet 'that', i.e. 
// the next call to GDataSetGetNextSample will give the first sample of 
// the category 'iCat'
#if BUILDMODE != 0
static inline
#endif 
void _GDSReset(GDataSet* const that, const long iCat);

// Reset the iterator on all categories of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
void _GDSResetAll(GDataSet* const that);

// Get the current sample in the category 'iCat' of the GDataSet 'that'
void* _GDSGetSample(
  const GDataSet* const that, const int iCat);
VecFloat* GDSGetSampleVecFloat(
  const GDataSetVecFloat* const that, const int iCat);
GDSGenBrushPair* GDSGetSampleGenBrushPair(
  const GDataSetGenBrushPair* const that, const int iCat);

// Get the inputs of the current sample in the category 'iCat' of
// the GDataSet 'that'
VecFloat* GDSGetSampleInputsVecFloat(
  const GDataSetVecFloat* const that, const int iCat);

// Get the outputs of the current sample in the category 'iCat' of
// the GDataSet 'that'
VecFloat* GDSGetSampleOutputsVecFloat(
  const GDataSetVecFloat* const that, const int iCat);

// Release the memory used by the FilePathPair 'that'
void GDSFilePathPairFree(GDSFilePathPair** const that);
#ifdef GENBRUSH_H
// Release the memory used by the GenBrushPair 'that'
void GDSGenBrushPairFree(GDSGenBrushPair** const that);
#endif

// Get the dimensions of the samples of GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const VecShort* _GDSSampleDim(const GDataSet* const that);

// Get the samples of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const GSet* _GDSSamples(const GDataSet* const that);
#if BUILDMODE != 0
static inline
#endif 
const GSetVecFloat* _GDSVecFloatSamples(
  const GDataSetVecFloat* const that);
#if BUILDMODE != 0
static inline
#endif 
const GSet* _GDSGenBrushPairSamples(
  const GDataSetGenBrushPair* const that);

// Center the GDataSet 'that' on its mean
void GDSMeanCenter(GDataSetVecFloat* const that);

// Center the inputs of the GDataSet 'that' on its mean
void GDSMeanCenterInputs(GDataSetVecFloat* const that);

// Normalize the GDataSet 'that', ie normalize each of its vectors
void GDSNormalize(GDataSetVecFloat* const that);

// Normalize the inputs of GDataSet 'that', ie normalize each of its
// input vectors
void GDSNormalizeInputs(GDataSetVecFloat* const that);

// Get the mean of the GDataSet 'that'
VecFloat* GDSGetMean(const GDataSetVecFloat* const that);

// Get the max of the GDataSet 'that'
VecFloat* GDSGetMax(const GDataSetVecFloat* const that);

// Get a clone of the GDataSet 'that'
// All the data in the GDataSet are cloned except for the splitting
// categories which are reset to one category made of the original data
GDataSetVecFloat GDSClone(const GDataSetVecFloat* const that);

// Get the covariance matrix of the GDataSetVecFloat 'that'
MatFloat* GDSGetCovarianceMatrix(const GDataSetVecFloat* const that);

// Get the covariance matrix of inputs of the GDataSetVecFloat 'that'
MatFloat* GDSGetInpCovarianceMatrix(const GDataSetVecFloat* const that);

// Get the covariance of the variables at 'indices' in the
// GDataSetVecFloat 'that'
float GDSGetCovariance(const GDataSetVecFloat* const that,
  const VecShort2D* const indices);

// Create a GDataSetVecFloat by importing the CSV file 'csvPath' and 
// decoding it with the 'importer'
// Return an empty GDatasetVecFloat if the importation failed
GDataSetVecFloat GDataSetCreateStaticFromCSV(
                    const char* const csvPath,
  const GDSVecFloatCSVImporter* const importer);

// Create a CSV importer for a GDataSetVecFloat
GDSVecFloatCSVImporter GDSVecFloatCSVImporterCreateStatic(
  const unsigned int sizeHeader,
          const char sep,
  const unsigned int nbCol,
  const unsigned int sizeSample,
                void (*converter)(
                  int col, 
                  char* val, 
                  VecFloat* sample));
  
// Save the GDataSetVecFloat to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool GDSVecFloatSave(
  const GDataSetVecFloat* const that, 
                    FILE* const stream, 
                     const bool compact);

// Save the category 'iCat' of the GDataSetVecFloat to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool GDSVecFloatSaveCategory(
  const GDataSetVecFloat* const that, 
                    FILE* const stream, 
                     const bool compact,
                      const int iCat);

// Run the prediction by the NeuraNet 'nn' on each sample of the category
// 'iCat' of the GDataSet 'that'. The index of columns in the samples
// for inputs and outputs are given by 'iInputs' and 'iOutputs'.
// Stop the prediction of samples when the result can't get better
// than 'threshold'
// Return the value of the NeuraNet on the predicted samples, defined
// as sum_samples(||output_sample-output_neuranet||)/nb_sample
// Higher is better, 0.0 is best value
float GDataSetVecFloatEvaluateNN(
  const GDataSetVecFloat* const that, 
  const NeuraNet* const nn, 
  const int iCat, 
  const VecShort* const iInputs,
  const VecShort* const iOutputs,
  const float threshold);

// Create a new GDataSetVecFloat
GDataSetVecFloat GDataSetVecFloatCreateStatic(void);

// Remove all the samples of the GDataSetVecFloat 'that'
#if BUILDMODE != 0
static inline
#endif
void _GDSVecFloatRemoveAllSample(GDataSetVecFloat* const that);

// Remove all the samples of the GDataSetGenBrushPair 'that'
#ifdef GENBRUSH_H
#if BUILDMODE != 0
static inline
#endif 
void _GDSGenBrushPairRemoveAllSample(GDataSetGenBrushPair* const that);
#endif 

// Append 'sample' in the GDataSetVecFloat 'that'
#if BUILDMODE != 0
static inline
#endif 
void _GDSVecFloatAddSample(
  GDataSetVecFloat* const that,
  VecFloat* sample);

// Append 'sample' in the GDataSetGenBrush 'that'
#ifdef GENBRUSH_H
#if BUILDMODE != 0
static inline
#endif 
void _GDSGenBrushPairAddSample(
  GDataSetGenBrushPair* const that,
  GDSGenBrushPair* sample);
#endif 

// Get the proximity matrix of the samples of category 'iCat' in the
// GDataSetVecFloat 'that'
// M[i,j] = euclidean distance between the i-th sample and the j-th sample
MatFloat* GDSVecFloatGetProxMat(
  const GDataSetVecFloat* that,
             unsigned int iCat);

// Get the nearest (in term of euclidean distance) sample of category
// 'iCat' to 'target' in the GDataSetVecFloat 'that' using the AESA
// nearest neighbour search
// TODO: should use a lookup table to get VecDist(sample, prevCandidate)
// but it's complicated by the fact I'm loosing the index of samples when
// using GSet
// Also, even if using a lookup table, and even if AESA actually reduces
// efficiently the number of VecDist(candidate, target), the cost of 
// VecDist() much be hugely bigger than the one of using a lookup table
// to make AESA relevant. It's probably meaningless on a VecFloat,
// maybe more meaningful on a GenBrush ?
// http://citeseerx.ist.psu.edu/viewdoc/download?
// doi=10.1.1.481.2200&rep=rep1&type=pdf
VecFloat* GDSVecFloatNearestNeighbourAESA(
  const GDataSetVecFloat* that,
          const VecFloat* target,
                      int iCat);

// Get the nearest (in term of euclidean distance) sample of category
// 'iCat' to 'target' in the GDataSetVecFloat 'that' using brute force
VecFloat* GDSVecFloatNearestNeighbourBrute(
  const GDataSetVecFloat* that,
          const VecFloat* target,
                      int iCat);

// Check if the 'iSample'-th sample in the category 'iCat' of the
// GDataSetVecFloat 'that' is an outlier. The outliers are identified as follow.
// For each sample, the nearest (in term of euclidian distance of inputs)
// sample with same output values is searched. Then, the number of nearer
// samples with different output values (called "opponents") is calculated.
// Finally, if there is more than 'nbOpponent' opponents, the sample is
// considered to be an outlier. The lower 'nbOpponent' is the more samples
// will be considered as outliers.
bool GDSVecFloatIsOutlierSampleCat(
  const GDataSetVecFloat* that,
          const VecFloat* sample,
                     long iCat,
             unsigned int nbOpponent);

// Remove the outliers from the category 'iCat', check
// GDSVecFloatIsOutlierSampleCat to see how outliers are identified.
// Return a GSetVecFloat containing the removed outliers. The _split
// property is updated with the remaining number of sample in the
// category
GSetVecFloat* GDSVecFloatRemoveOutlierCat(
  const GDataSetVecFloat* that,
                     long iCat,
             unsigned int nbOpponent);

// ================= Polymorphism ==================

#define GDSRemoveAllSample(DataSet) _Generic(DataSet, \
  GDataSetVecFloat*: _GDSVecFloatRemoveAllSample, \
  GDataSetGenBrushPair*: _GDSGenBrushPairRemoveAllSample, \
  default: PBErrInvalidPolymorphism)(DataSet)

#define GDSAddSample(DataSet, Sample) _Generic(DataSet, \
  GDataSetVecFloat*: _GDSVecFloatAddSample, \
  GDataSetGenBrushPair*: _GDSGenBrushPairAddSample, \
  default: PBErrInvalidPolymorphism)(DataSet, Sample)

#define GDSDesc(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSDesc, \
  const GDataSet*: _GDSDesc, \
  GDataSetVecFloat*: _GDSDesc, \
  const GDataSetVecFloat*: _GDSDesc, \
  GDataSetGenBrushPair*: _GDSDesc, \
  const GDataSetGenBrushPair*: _GDSDesc, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSGetNbCat(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetNbCat, \
  const GDataSet*: _GDSGetNbCat, \
  GDataSetVecFloat*: _GDSGetNbCat, \
  const GDataSetVecFloat*: _GDSGetNbCat, \
  GDataSetGenBrushPair*: _GDSGetNbCat, \
  const GDataSetGenBrushPair*: _GDSGetNbCat, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSGetSample(DataSet, ICat) _Generic(DataSet, \
  GDataSet*: _GDSGetSample, \
  const GDataSet*: _GDSGetSample, \
  GDataSetVecFloat*: GDSGetSampleVecFloat, \
  const GDataSetVecFloat*: GDSGetSampleVecFloat, \
  GDataSetGenBrushPair*: GDSGetSampleGenBrushPair, \
  const GDataSetGenBrushPair*: GDSGetSampleGenBrushPair, \
  default: PBErrInvalidPolymorphism)(DataSet, ICat)

#define GDSGetSampleInputs(DataSet, ICat) _Generic(DataSet, \
  GDataSetVecFloat*: GDSGetSampleInputsVecFloat, \
  const GDataSetVecFloat*: GDSGetSampleInputsVecFloat, \
  default: PBErrInvalidPolymorphism)(DataSet, ICat)

#define GDSGetSampleOutputs(DataSet, ICat) _Generic(DataSet, \
  GDataSetVecFloat*: GDSGetSampleOutputsVecFloat, \
  const GDataSetVecFloat*: GDSGetSampleOutputsVecFloat, \
  default: PBErrInvalidPolymorphism)(DataSet, ICat)

#define GDSGetSize(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetSize, \
  const GDataSet*: _GDSGetSize, \
  GDataSetVecFloat*: _GDSGetSize, \
  const GDataSetVecFloat*: _GDSGetSize, \
  GDataSetGenBrushPair*: _GDSGetSize, \
  const GDataSetGenBrushPair*: _GDSGetSize, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSGetSizeCat(DataSet, ICat) _Generic(DataSet, \
  GDataSet*: _GDSGetSizeCat, \
  const GDataSet*: _GDSGetSizeCat, \
  GDataSetVecFloat*: _GDSGetSizeCat, \
  const GDataSetVecFloat*: _GDSGetSizeCat, \
  GDataSetGenBrushPair*: _GDSGetSizeCat, \
  const GDataSetGenBrushPair*: _GDSGetSizeCat, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet, ICat)

#define GDSGetType(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetType, \
  const GDataSet*: _GDSGetType, \
  GDataSetVecFloat*: _GDSGetType, \
  const GDataSetVecFloat*: _GDSGetType, \
  GDataSetGenBrushPair*: _GDSGetType, \
  const GDataSetGenBrushPair*: _GDSGetType, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSName(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSName, \
  const GDataSet*: _GDSName, \
  GDataSetVecFloat*: _GDSName, \
  const GDataSetVecFloat*: _GDSName, \
  GDataSetGenBrushPair*: _GDSName, \
  const GDataSetGenBrushPair*: _GDSName, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSGetNbMask(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetNbMask, \
  const GDataSet*: _GDSGetNbMask, \
  GDataSetGenBrushPair*: GDSGetNbMaskGenBrushPair, \
  const GDataSetGenBrushPair*: GDSGetNbMaskGenBrushPair, \
  default: PBErrInvalidPolymorphism)(DataSet)

#define GDSReset(DataSet, ICat) _Generic(DataSet, \
  GDataSet*: _GDSReset, \
  const GDataSet*: _GDSReset, \
  GDataSetVecFloat*: _GDSReset, \
  const GDataSetVecFloat*: _GDSReset, \
  GDataSetGenBrushPair*: _GDSReset, \
  const GDataSetGenBrushPair*: _GDSReset, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, ICat)

#define GDSResetAll(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSResetAll, \
  const GDataSet*: _GDSResetAll, \
  GDataSetVecFloat*: _GDSResetAll, \
  const GDataSetVecFloat*: _GDSResetAll, \
  GDataSetGenBrushPair*: _GDSResetAll, \
  const GDataSetGenBrushPair*: _GDSResetAll, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet)

#define GDSSampleDim(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSSampleDim, \
  const GDataSet*: _GDSSampleDim, \
  GDataSetVecFloat*: _GDSSampleDim, \
  const GDataSetVecFloat*: _GDSSampleDim, \
  GDataSetGenBrushPair*: _GDSSampleDim, \
  const GDataSetGenBrushPair*: _GDSSampleDim, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSShuffle(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSShuffle, \
  GDataSetVecFloat*: _GDSShuffle, \
  GDataSetGenBrushPair*: _GDSShuffle, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet)

#define GDSShuffleCat(DataSet, ICat) _Generic(DataSet, \
  GDataSet*: _GDSShuffleCat, \
  GDataSetVecFloat*: _GDSShuffleCat, \
  GDataSetGenBrushPair*: _GDSShuffleCat, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, ICat)

#define GDSShuffleAll(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSShuffleAll, \
  GDataSetVecFloat*: _GDSShuffleAll, \
  GDataSetGenBrushPair*: _GDSShuffleAll, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet)

#define GDSSplit(DataSet, Cat) _Generic(DataSet, \
  GDataSet*: _Generic(Cat, \
    VecLong*: _GDSSplit, \
    const VecLong*: _GDSSplit, \
    VecLong2D*: _GDSSplit, \
    const VecLong2D*: _GDSSplit, \
    VecLong3D*: _GDSSplit, \
    const VecLong3D*: _GDSSplit), \
  GDataSetVecFloat*: _Generic(Cat, \
    VecLong*: _GDSSplit, \
    const VecLong*: _GDSSplit, \
    VecLong2D*: _GDSSplit, \
    const VecLong2D*: _GDSSplit, \
    VecLong3D*: _GDSSplit, \
    const VecLong3D*: _GDSSplit), \
  GDataSetGenBrushPair*: _Generic(Cat, \
    VecLong*: _GDSSplit, \
    const VecLong*: _GDSSplit, \
    VecLong2D*: _GDSSplit, \
    const VecLong2D*: _GDSSplit, \
    VecLong3D*: _GDSSplit, \
    const VecLong3D*: _GDSSplit), \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, (const VecLong*)Cat)

#define GDSStepSample(DataSet, ICat) _Generic(DataSet, \
  GDataSet*: _GDSStepSample, \
  const GDataSet*: _GDSStepSample, \
  GDataSetVecFloat*: _GDSStepSample, \
  const GDataSetVecFloat*: _GDSStepSample, \
  GDataSetGenBrushPair*: _GDSStepSample, \
  const GDataSetGenBrushPair*: _GDSStepSample, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet, ICat)

#define GDSUnsplit(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSUnsplit, \
  GDataSetVecFloat*: _GDSUnsplit, \
  GDataSetGenBrushPair*: _GDSUnsplit, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet)

#define GDSSamples(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSSamples, \
  const GDataSet*: _GDSSamples, \
  GDataSetVecFloat*: _GDSVecFloatSamples, \
  const GDataSetVecFloat*: _GDSVecFloatSamples, \
  GDataSetGenBrushPair*: _GDSGenBrushPairSamples, \
  const GDataSetGenBrushPair*: _GDSGenBrushPairSamples, \
  default: PBErrInvalidPolymorphism)(DataSet)

#define GDSSave(DataSet, Stream, Compact) _Generic(DataSet, \
  GDataSetVecFloat*: GDSVecFloatSave, \
  const GDataSetVecFloat*: GDSVecFloatSave, \
  default: PBErrInvalidPolymorphism)(DataSet, Stream, Compact)

#define GDSEvaluateNN(GDS, NN, Cat, Inputs, Outputs, Threshold) \
  _Generic(GDS, \
  GDataSetVecFloat*: GDataSetVecFloatEvaluateNN, \
  const GDataSetVecFloat*: GDataSetVecFloatEvaluateNN, \
  default: PBErrInvalidPolymorphism)( \
    GDS, NN, Cat, Inputs, Outputs, Threshold)

#define GDSLoad(DataSet, FP) _Generic(DataSet, \
  GDataSet*: GDataSetLoad, \
  GDataSetVecFloat*: _GDSLoad, \
  GDataSetGenBrushPair*: _GDSLoad, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, FP)

#define GDSGetProxMat(DataSet, Cat) _Generic(DataSet, \
  GDataSetVecFloat*: GDSVecFloatGetProxMat, \
  const GDataSetVecFloat*: GDSVecFloatGetProxMat, \
  default: PBErrInvalidPolymorphism)(DataSet, Cat)

#define GDSNearestNeighbour(DataSet, Target, ICat) \
  _Generic(DataSet, \
    GDataSetVecFloat*: GDSVecFloatNearestNeighbourBrute, \
    const GDataSetVecFloat*: GDSVecFloatNearestNeighbourBrute, \
    default: PBErrInvalidPolymorphism)(DataSet, Target, ICat)

#define GDSIsOutlierSampleCat(DataSet, ISample, ICat, NbOpponent) \
  _Generic(DataSet, \
    GDataSetVecFloat*: GDSVecFloatIsOutlierSampleCat, \
    const GDataSetVecFloat*: GDSVecFloatIsOutlierSampleCat, \
    default: PBErrInvalidPolymorphism)(DataSet, ISample, ICat, NbOpponent)

#define GDSGetNbInputs(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetNbInputs, \
  const GDataSet*: _GDSGetNbInputs, \
  GDataSetVecFloat*: _GDSGetNbInputs, \
  const GDataSetVecFloat*: _GDSGetNbInputs, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSGetNbOutputs(DataSet) _Generic(DataSet, \
  GDataSet*: _GDSGetNbOutputs, \
  const GDataSet*: _GDSGetNbOutputs, \
  GDataSetVecFloat*: _GDSGetNbOutputs, \
  const GDataSetVecFloat*: _GDSGetNbOutputs, \
  default: PBErrInvalidPolymorphism)((const GDataSet*)DataSet)

#define GDSSetNbInputs(DataSet, Nb) _Generic(DataSet, \
  GDataSet*: _GDSSetNbInputs, \
  const GDataSet*: _GDSSetNbInputs, \
  GDataSetVecFloat*: _GDSSetNbInputs, \
  const GDataSetVecFloat*: _GDSSetNbInputs, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, Nb)

#define GDSSetNbOutputs(DataSet, Nb) _Generic(DataSet, \
  GDataSet*: _GDSSetNbOutputs, \
  const GDataSet*: _GDSSetNbOutputs, \
  GDataSetVecFloat*: _GDSSetNbOutputs, \
  const GDataSetVecFloat*: _GDSSetNbOutputs, \
  default: PBErrInvalidPolymorphism)((GDataSet*)DataSet, Nb)

// ================ static inliner ====================

#if BUILDMODE != 0
#include "gdataset-inline.c"
#endif

#endif
