// ============ GDATASET_C ================

// ================= Include =================

#include "gdataset.h"
#if BUILDMODE == 0
#include "gdataset-inline.c"
#endif

// ================ Functions implementation ====================

// Create a new GDataSet of type 'type'
GDataSet GDataSetCreateStatic(GDataSetType type) {
  // Declare the new GDataSet
  GDataSet that;
  // Set the properties
  that._name = NULL;
  that._desc = NULL;
  that._type = type;
  that._nbSample = 0;
  that._nbInputs = 0;
  that._nbOutputs = 0;
  that._samples = GSetCreateStatic();
  that._sampleDim = NULL; 
  that._split = NULL;
  that._categories = NULL;
  that._iterators = NULL;
  // Return the new GDataSet
  return that;
}

// Load the GDataSet 'that' from the stream 'stream'
// Return true if the GDataSet could be loaded, false else
bool GDataSetLoad(GDataSet* that, FILE* const stream) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'stream' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Load the whole encoded data
  JSONNode* json = JSONCreate();
  if (!JSONLoad(json, stream)) {
    return false;
  }
  // Decode the JSON
  if (!GDataSetDecodeAsJSON(that, json)) {
    return false;
  }
  // Free memory
  JSONFree(&json);
  // Return the success code
  return true;
}
bool _GDSLoad(GDataSet* that, FILE* const stream) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'stream' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Load the whole encoded data
  JSONNode* json = JSONCreate();
  if (!JSONLoad(json, stream)) {
    return false;
  }
  // Decode the JSON
  GDataSet dataset = GDataSetCreateStatic(0);
  if (!GDataSetDecodeAsJSON(&dataset, json)) {
    return false;
  }
  if (GDSGetType(&dataset) != GDSGetType(that)) {
    return false;
  } else {
    *that = dataset;
  }
  // Decode the samples according to the type of dataset
  switch(GDSGetType(that)) {
    case GDataSetType_VecFloat:
      if (!GDataSetVecFloatDecodeAsJSON((GDataSetVecFloat*)that, json)) {
        return false;
      }
      break;
    case GDataSetType_GenBrushPair:
      if (!GDataSetGenBrushPairDecodeAsJSON(
        (GDataSetGenBrushPair*)that, json)) {
        return false;
      }
      break;
    default:
      return false;
  }
  // Free memory
  JSONFree(&json);
  // Return the success code
  return true;
}

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetDecodeAsJSON(GDataSet* that, const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
  if (json == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'json' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // Free memory
  GDataSetFreeStatic(that);
  // Decode dataSetType
  JSONNode* prop = JSONProperty(json, "dataSetType");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dataSetType missing)");
    return false;
  }
  JSONNode* val = JSONValue(prop, 0);
  // Create the new data set
  *that = GDataSetCreateStatic(atoi(JSONLabel(val)));
  // Decode dataSet
  prop = JSONProperty(json, "dataSet");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dataSet missing)");
    return false;
  }
  val = JSONValue(prop, 0);
  that->_name = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (strlen(JSONLabel(val)) + 1));
  strcpy(that->_name, JSONLabel(val));
  // Decode desc
  prop = JSONProperty(json, "desc");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (desc missing)");
    return false;
  }
  val = JSONValue(prop, 0);
  that->_desc = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (strlen(JSONLabel(val)) + 1));
  strcpy(that->_desc, JSONLabel(val));
  // Decode dim
  prop = JSONProperty(json, "dim");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dim missing)");
    return false;
  }
  that->_sampleDim = NULL;
  VecDecodeAsJSON(&(that->_sampleDim), prop);
  // Decode nbSample
  prop = JSONProperty(json, "nbSample");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbSample missing)");
    return false;
  }
  val = JSONValue(prop, 0);
  that->_nbSample = atoi(JSONLabel(val));
  // Decode nbInputs
  prop = JSONProperty(json, "nbInputs");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbInputs missing)");
    return false;
  }
  val = JSONValue(prop, 0);
  that->_nbInputs = atoi(JSONLabel(val));
  // Decode nbOutputs
  prop = JSONProperty(json, "nbOutputs");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbOutputs missing)");
    return false;
  }
  val = JSONValue(prop, 0);
  that->_nbOutputs = atoi(JSONLabel(val));
  // Return the success code
  return true;
}

// Free the memory used by a GDataSet
void GDataSetFreeStatic(GDataSet* const that) {
  if (that == NULL)
    return;
  // Free memory
  if (that->_name)
    free(that->_name);
  if (that->_desc)
    free(that->_desc);
  for (int iCat = GDSGetNbCat(that); iCat--;) {
    GSetFlush(that->_categories + iCat);
  }
  if (that->_categories)
    free(that->_categories);
  if (that->_iterators)
    free(that->_iterators);
  if (that->_split)
    VecFree(&(that->_split));
  if (that->_sampleDim)
    VecFree(&(that->_sampleDim));
}

// Create a new GDataSetVecFloat defined by the file at 'cfgFilePath'
GDataSetVecFloat GDataSetVecFloatCreateStaticFromFile(
  const char* const cfgFilePath) {
#if BUILDMODE == 0
  if (cfgFilePath == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'cfgFilePath' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new GDataSetVecFloat
  GDataSetVecFloat that;
  *(GDataSet*)&that = GDataSetCreateStatic(GDataSetType_VecFloat);
  // Open the file
  FILE* stream = fopen(cfgFilePath, "r");
  // If the description file doesn't exist
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(GDataSetErr->_msg, "Can't open the configuration file %s",
      cfgFilePath);
    PBErrCatch(GDataSetErr);
  }
  // Load the whole encoded data
  JSONNode* json = JSONCreate();
  if (!JSONLoad(json, stream)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't load the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Decode the JSON data for the generic GDataSet
  if (!GDataSetDecodeAsJSON((GDataSet*)&that, json)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't decode the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Check the type
  if (GDSGetType(&that) != GDataSetType_VecFloat) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Invalid type");
    PBErrCatch(GDataSetErr);
  }
  // Check the samples' dimension
  if (VecGetDim(GDSSampleDim(&that)) != 1) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Invalid sample dimension");
    PBErrCatch(GDataSetErr);
  }
  // Decode the properties of the GDataSetVecFloat
  if (!GDataSetVecFloatDecodeAsJSON(&that, json)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't decode the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Free memory
  JSONFree(&json);
  fclose(stream);
  // Return the new GDataSetVecFloat
  return that;
}

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetVecFloatDecodeAsJSON(GDataSetVecFloat* that, 
  const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
  if (json == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'json' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // Load the samples
  JSONNode* prop = JSONProperty(json, "samples");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples missing)");
    return false;
  }
  if (JSONGetNbValue(prop) != that->_dataSet._nbSample) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples's number != nbSample)");
    return false;
  }
  that->_dataSet._samples = GSetCreateStatic();
  for (int iSample = 0; iSample < GDSGetSize(that); ++iSample) {
    JSONNode* val = JSONValue(prop, iSample);
    VecFloat* v = NULL;
    VecDecodeAsJSON(&v, val);
    GSetAppend((GSet*)GDSSamples(that), v);
  }
  // Create the initial category
  GDSResetCategories((GDataSet*)that);
  // Return the success code
  return true;
}

// Reset the categories of the GDataSet 'that' to one unshuffled
// category
void GDSResetCategories(GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  if (that->_split) 
    VecFree(&(that->_split));
  that->_split = VecLongCreate(1);
  VecSet(that->_split, 0, GDSGetSize(that));
  if (that->_categories) {
    for (int iCat = GDSGetNbCat(that); iCat--;) {
      GSetFlush(that->_categories + iCat);
    }
    free(that->_categories);
  }
  if (that->_iterators)
    free(that->_iterators);
  that->_categories = GSetCreate();
  GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
  if (GDSGetSize(that) > 0) {
    do {
      void* sample = GSetIterGet(&iter);
      GSetAppend(that->_categories, sample);
    } while (GSetIterStep(&iter));
  }
  that->_iterators = PBErrMalloc(GDataSetErr, sizeof(GSetIterForward));
  that->_iterators[0] = GSetIterForwardCreateStatic(that->_categories);
}

// Free the memory used by a GDataSetVecFloat
void GDataSetVecFloatFreeStatic(GDataSetVecFloat* const that) {
  if (that == NULL)
    return;
  // Free memory
  while (GSetNbElem(&(((GDataSet*)that)->_samples)) > 0) {
    VecFloat* sample = GSetPop(&(((GDataSet*)that)->_samples));
    VecFree(&sample);
  }
  GDataSetFreeStatic((GDataSet*)that);
}

// Create a new GDataSetGenBrushPair defined by the file at 'cfgFilePath'
// The random generator must have been initialized before calling 
// this function
GDataSetGenBrushPair GDataSetGenBrushPairCreateStaticFromFile(
  const char* const cfgFilePath) {
#if BUILDMODE == 0
  if (cfgFilePath == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'cfgFilePath' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new GDataSetVecFloat
  GDataSetGenBrushPair that;
  *(GDataSet*)&that = GDataSetCreateStatic(GDataSetType_GenBrushPair);
  // Copy the file path
  that._cfgFilePath = PBErrMalloc(GDataSetErr, strlen(cfgFilePath) + 1);
  strcpy(that._cfgFilePath, cfgFilePath);
  // Open the file
  FILE* stream = fopen(cfgFilePath, "r");
  // If the description file doesn't exist
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(GDataSetErr->_msg, "Can't open the configuration file %s",
      cfgFilePath);
    PBErrCatch(GDataSetErr);
  }
  // Load the whole encoded data
  JSONNode* json = JSONCreate();
  if (!JSONLoad(json, stream)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't load the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Decode the JSON data for the generic GDataSet
  if (!GDataSetDecodeAsJSON((GDataSet*)&that, json)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't decode the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Check the type
  if (GDSGetType(&that) != GDataSetType_GenBrushPair) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Invalid type");
    PBErrCatch(GDataSetErr);
  }
  // Check the samples' dimension
  if (VecGetDim(GDSSampleDim(&that)) != 2) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Invalid sample dimension (%ld=2)",
      VecGetDim(GDSSampleDim(&that)));
    PBErrCatch(GDataSetErr);
  }
  // Decode the properties of the GDataSetGenBrushPair
  if (!GDataSetGenBrushPairDecodeAsJSON(&that, json)) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't decode the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Free memory
  JSONFree(&json);
  fclose(stream);
  // Return the new GDataSetGenBrushPair
  return that;
}

// Function which decode from JSON encoding 'json' to 'that'
bool GDataSetGenBrushPairDecodeAsJSON(GDataSetGenBrushPair* that, 
  const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
  if (json == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'json' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // Get the nb of mask
  JSONNode* prop = JSONProperty(json, "nbMask");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbMask missing)");
    PBErrCatch(GDataSetErr);
  }
  that->_nbMask = atoi(JSONLblVal(prop));
  if (that->_nbMask >= GDS_NBMAXMASK) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (invalid nbMask %d>=%d)", 
      that->_nbMask, GDS_NBMAXMASK);
    PBErrCatch(GDataSetErr);
  }
  // Load the samples
  prop = JSONProperty(json, "samples");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples missing)");
    PBErrCatch(GDataSetErr);
  }
  if (JSONGetNbValue(prop) != GDSGetSize(that)) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples's number != nbSample)");
    PBErrCatch(GDataSetErr);
  }
  that->_dataSet._samples = GSetCreateStatic();
  for (int iSample = 0; iSample < GDSGetSize(that); ++iSample) {
    JSONNode* val = JSONValue(prop, iSample);
    // Allocate memory for the pair image/mask
    GDSFilePathPair* pair = PBErrMalloc(GDataSetErr, 
      sizeof(GDSFilePathPair));
    pair->_path[0] = NULL;
    for (int iMask = GDS_NBMAXMASK; iMask--;)
      pair->_path[1 + iMask] = NULL;
    // Decode img
    JSONNode* subProp = JSONProperty(val, "img");
    if (subProp == NULL) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, 
        "Invalid description file (samples.img missing)");
      PBErrCatch(GDataSetErr);
    }
    JSONNode* subVal = JSONValue(subProp, 0);
    pair->_path[0] = PBErrMalloc(GDataSetErr, 
      sizeof(char) * (strlen(JSONLabel(subVal)) + 1));
    strcpy(pair->_path[0], JSONLabel(subVal));
    // Decode mask
    subProp = JSONProperty(val, "mask");
    if (subProp == NULL) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, 
        "Invalid description file (samples.mask missing)");
      PBErrCatch(GDataSetErr);
    }
    for (int iMask = 0; iMask < that->_nbMask; ++iMask) {
      subVal = JSONValue(subProp, iMask);
      pair->_path[1 + iMask] = PBErrMalloc(GDataSetErr, 
        sizeof(char) * (strlen(JSONLabel(subVal)) + 1));
      strcpy(pair->_path[1 + iMask], JSONLabel(subVal));
    }
    // Add the pair to the samples
    GSetAppend((GSet*)GDSSamples(that), pair);
  }
  // Create the initial category
  GDSResetCategories((GDataSet*)that);
  // Return the new GDataSetVecFloat
  return that;
}

// Free the memory used by a GDataSetGenBrushPair
void GDataSetGenBrushPairFreeStatic(GDataSetGenBrushPair* const that) {
  if (that == NULL)
    return;
  // Free memory
  GDataSetFreeStatic((GDataSet*)that);
  if (that->_cfgFilePath)
    free(that->_cfgFilePath);
  while (GSetNbElem(&(((GDataSet*)that)->_samples)) > 0) {
    GDSFilePathPair* sample = GSetPop(&(((GDataSet*)that)->_samples));
    GDSFilePathPairFree(&sample);
  }
}

// Split the samples of the GDataSet 'that' into several categories
// defined by 'cat'. The dimension of 'cat' gives the number of 
// categories and the value for each dimension of 'cat' gives the 
// number of samples in the corresponding category. For example <3,4> 
// would mean 2 categories with 3 samples in the first one and 4 
// samples in the second one. There must me at least as many samples 
// in the data set as the sum of samples in 'cat'.
// Each category must have at least one sample.
// If 'that' was already splitted the previous splitting is discarded.
void _GDSSplit(GDataSet* const that, const VecLong* const cat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  long nb = 0;
  for (long iCat = VecGetDim(cat); iCat--;)
    nb += VecGet(cat, iCat);
  if (nb > GDSGetSize(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "Not enough samples for the requested splitting (%ld<%ld)",
        nb, GDSGetSize(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Free the current splitting if necessary
  if (that->_categories != NULL) {
    if (that->_split != NULL) {
      for (int iCat = GDSGetNbCat(that); iCat--;) {
        GSetFlush(that->_categories + iCat);
      }
    }
    free(that->_categories);
  }
  if (that->_iterators)
    free(that->_iterators);
  VecFree(&(that->_split));
  // Get the number of categories
  long nbCat = VecGetDim(cat);
  // Allocate memory for the categories
  that->_categories = PBErrMalloc(GDataSetErr, sizeof(GSet) * nbCat);
  for (long iCat = nbCat; iCat--;) {
    that->_categories[iCat] = GSetCreateStatic();
  }
  // Copy the splitting
  that->_split = VecClone(cat);
  // Declare an iterator on the samples
  GSetIterForward iter = GSetIterForwardCreateStatic(&(that->_samples));
  // Loop on categories
  for (long iCat = 0; iCat < nbCat; ++iCat) {
    // Get the nb of samples for this category
    long nbSample = VecGet(cat, iCat);
    // Loop on the sample
    for (long iSample = 0; iSample < nbSample; ++iSample) {
      // Get the sample
      void* sample = GSetIterGet(&iter);
      // Add the sample to the category
      GSetAppend(that->_categories + iCat, sample);
      // Move to the next sample
      GSetIterStep(&iter);
    }
  }
  // Allocate memory for the iterators
  that->_iterators = PBErrMalloc(GDataSetErr, 
    sizeof(GSetIterForward) * nbCat);
  for (long iCat = nbCat; iCat--;) {
    that->_iterators[iCat] = 
      GSetIterForwardCreateStatic(that->_categories + iCat);
  }
}

// Get the current sample in the category 'iCat' of the GDataSet 'that'
void* _GDSGetSample(
  const GDataSet* const that, const int iCat) {
  // Call the appropriate function according to the type
  switch (GDSGetType(that)) {
    case GDataSetType_VecFloat:
      return GDSGetSampleVecFloat((GDataSetVecFloat*)that, iCat);
      break;
    case GDataSetType_GenBrushPair:
      return GDSGetSampleGenBrushPair((GDataSetGenBrushPair*)that, iCat);
      break;
    default:
      return NULL;
      break;
  }
}

// Get the number of masks in the GDataSet 'that'
int _GDSGetNbMask(const GDataSet* const that) {
  // Call the appropriate function according to the type
  switch (GDSGetType(that)) {
    case GDataSetType_GenBrushPair:
      return GDSGetNbMaskGenBrushPair((GDataSetGenBrushPair*)that);
      break;
    default:
      return 0;
      break;
  }
}

VecFloat* GDSGetSampleVecFloat(
  const GDataSetVecFloat* const that, const int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%d<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecFloat* sample = GSetIterGet(((GDataSet*)that)->_iterators + iCat);
  return VecClone(sample);
}

// Get the inputs of the current sample in the category 'iCat' of
// the GDataSet 'that'
VecFloat* GDSGetSampleInputsVecFloat(
  const GDataSetVecFloat* const that, const int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%d<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecFloat* sample = GSetIterGet(((GDataSet*)that)->_iterators + iCat);
  VecFloat* inputs = VecFloatCreate(GDSGetNbInputs(that));
  for (short i = GDSGetNbInputs(that); i--;) {
    VecSet(inputs, i, VecGet(sample, i));
  }
  return inputs;
}

// Get the outputs of the current sample in the category 'iCat' of
// the GDataSet 'that'
VecFloat* GDSGetSampleOutputsVecFloat(
  const GDataSetVecFloat* const that, const int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%d<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecFloat* sample = GSetIterGet(((GDataSet*)that)->_iterators + iCat);
  VecFloat* outputs = VecFloatCreate(GDSGetNbOutputs(that));
  for (short i = GDSGetNbOutputs(that); i--;) {
    VecSet(outputs, i, VecGet(sample, i + GDSGetNbInputs(that)));
  }
  return outputs;
}

GDSGenBrushPair* GDSGetSampleGenBrushPair(
  const GDataSetGenBrushPair* const that, const int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%d<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  GDSFilePathPair* pairFile = 
    GSetIterGet(((GDataSet*)that)->_iterators + iCat);
  GDSGenBrushPair* pairSample = PBErrMalloc(GDataSetErr, 
    sizeof(GDSGenBrushPair));
  for (int iMask = 0; iMask < GDS_NBMAXMASK; ++iMask)
    pairSample->_mask[iMask] = NULL;
  char* root = PBFSGetRootPath(that->_cfgFilePath);
  char* path = PBFSJoinPath(root, pairFile->_path[0]);
  GenBrush* gb = GBCreateFromFile(path);
  // Rescale the sample if needed to always provide to the user
  // the dimensions defined in the configuration file of the data set
  if (gb != NULL && !VecIsEqual(GBDim(gb), GDSSampleDim(that))) {
    pairSample->_img = GBScale(gb, 
      (const VecShort2D*)GDSSampleDim(that), GBScaleMethod_Default);
    GBFree(&gb);
  } else {
    pairSample->_img = gb;
  }
  free(path);
  for (int iMask = 0; iMask < GDSGetNbMask(that); ++iMask) {
    path = PBFSJoinPath(root, pairFile->_path[1 + iMask]);
    gb = GBCreateFromFile(path);
    if (gb != NULL && !VecIsEqual(GBDim(gb), GDSSampleDim(that))) {
      pairSample->_mask[iMask] = GBScale(gb, 
        (const VecShort2D*)GDSSampleDim(that), GBScaleMethod_Default);
      GBFree(&gb);
    } else {
      pairSample->_mask[iMask] = gb;
    }
    free(path);
  }
  free(root);
  return pairSample;
}

// Release the memory used by the FilePathPair 'that'
void GDSFilePathPairFree(GDSFilePathPair** const that) {
  if (that == NULL || *that == NULL)
    return;
  for (int iMask = GDS_NBMAXMASK + 1; iMask--;)
    if ((*that)->_path[iMask] != NULL)
      free((*that)->_path[iMask]);
  free(*that);
  *that = NULL;
}

// Release the memory used by the GenBrushPair 'that'
void GDSGenBrushPairFree(GDSGenBrushPair** const that) {
  if (that == NULL || *that == NULL)
    return;
  GBFree(&((*that)->_img));
  for (int iMask = GDS_NBMAXMASK; iMask--;)
    GBFree(&((*that)->_mask[iMask]));
  free(*that);
  *that = NULL;
}

// Center the GDataSet 'that' on its mean
void GDSMeanCenter(GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the mean of the dataset
  VecFloat* mean = GDSGetMean(that);
  // Translate all the data by the mean of the data set
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* sample = GSetIterGet(&iter);
      VecOp(sample, 1.0, mean, -1.0);
    } while (GSetIterStep(&iter));
  }
  // Free memory
  VecFree(&mean);
}

// Center the inputs of the GDataSet 'that' on its mean
void GDSMeanCenterInputs(GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the mean of the dataset
  VecFloat* mean = GDSGetMean(that);
  // Translate all the data by the mean of the data set
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* sample = GSetIterGet(&iter);
      VecFloat* inputs = VecGetOp(sample, 1.0, mean, -1.0);
      for (int i = GDSGetNbInputs(that); i--;)
        VecSet(sample, i, VecGet(inputs, i));
      VecFree(&inputs);
    } while (GSetIterStep(&iter));
  }
  // Free memory
  VecFree(&mean);
}

// Normalize the GDataSet 'that', ie normalize each of its vectors
void GDSNormalize(GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Normalize all the data of the data set
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* sample = GSetIterGet(&iter);
      VecNormalise(sample);
    } while (GSetIterStep(&iter));
  }
}

// Normalize the inputs of GDataSet 'that', ie normalize each of its
// input vectors
void GDSNormalizeInputs(GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Normalize all the data of the data set
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* sample = GSetIterGet(&iter);
      VecFloat* inputs = VecGetNewDim(sample, GDSGetNbInputs(that));
      VecNormalise(inputs);
      for (int i = GDSGetNbInputs(that); i--;)
        VecSet(sample, i, VecGet(inputs, i));
      VecFree(&inputs);
    } while (GSetIterStep(&iter));
  }
}

// Get the mean of the GDataSet 'that'
VecFloat* GDSGetMean(const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the dimension of the samples
  const VecShort* dim = GDSSampleDim(that);
  // Create a vector to calculate the mean
  VecFloat* mean = VecFloatCreate(VecGet(dim, 0));
  // Calculate the mean
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = 
      GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* v = GSetIterGet(&iter);
      VecOp(mean, 1.0, v, 1.0);
    } while(GSetIterStep(&iter));
    VecScale(mean, 1.0 / (float)GDSGetSize(that));
  }
  // Return the result
  return mean;
}

// Get the max of the GDataSet 'that'
VecFloat* GDSGetMax(const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the dimension of the samples
  const VecShort* dim = GDSSampleDim(that);
  // Create a vector to calculate the max
  int d = VecGet(dim, 0);
  VecFloat* max = VecFloatCreate(d);
  // Calculate the mx
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = 
      GSetIterForwardCreateStatic(GDSSamples(that));
    VecFloat* v = GSetIterGet(&iter);
    VecCopy(max, v);
    do {
      v = GSetIterGet(&iter);
      for (int i = d; i--;)
        if (VecGet(max, i) < VecGet(v, i))
          VecSet(max, i, VecGet(v, i));
    } while(GSetIterStep(&iter));
  }
  // Return the result
  return max;
}

// Get a clone of the GDataSet 'that'
// All the data in the GDataSet are cloned except for the splitting
// categories which are reset to one category made of the original data
GDataSetVecFloat GDSClone(const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the result dataset
  GDataSetVecFloat dataset;
  // Create a pointer to the GDataSet for convenience
  GDataSet* tho = &(dataset._dataSet);
  // Clone or initialize the properties
  tho->_name = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (1 + strlen(that->_dataSet._name)));
  strcpy(tho->_name, that->_dataSet._name);
  tho->_desc = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (1 + strlen(that->_dataSet._desc)));
  strcpy(tho->_desc, that->_dataSet._desc);
  tho->_type = that->_dataSet._type;
  tho->_nbSample = that->_dataSet._nbSample;
  tho->_nbInputs = that->_dataSet._nbInputs;
  tho->_nbOutputs = that->_dataSet._nbOutputs;
  tho->_sampleDim = VecClone(that->_dataSet._sampleDim);
  tho->_samples = GSetCreateStatic();
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* v = GSetIterGet(&iter);
      GSetAppend(&(tho->_samples), VecClone(v));
    } while (GSetIterStep(&iter));
  }
  tho->_split = NULL;
  tho->_categories = NULL;
  tho->_iterators = NULL;
  tho->_split = VecLongCreate(1);
  VecSet(tho->_split, 0, tho->_nbSample);
  tho->_categories = PBErrMalloc(GDataSetErr, sizeof(GSet));
  tho->_categories[0] = GSetCreateStatic();
  if (GDSGetSize(that) > 0) {
    GSetIterForward iter = 
      GSetIterForwardCreateStatic(&(tho->_samples));
    do {
      void* sample = GSetIterGet(&iter);
      GSetAppend(tho->_categories, sample);
    } while (GSetIterStep(&iter));
  }
  tho->_iterators = 
    PBErrMalloc(GDataSetErr, sizeof(GSetIterForward));
  tho->_iterators[0] = 
    GSetIterForwardCreateStatic(tho->_categories);
  // Return the result dataset
  return dataset;
}

// Get the covariance matrix of the GDataSetVecFloat 'that'
MatFloat* GDSGetCovarianceMatrix(const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the dimension of the samples
  const VecShort* dim = GDSSampleDim(that);
  // Allocate memory for the covariance matrix;
  VecShort2D dimMat = VecShortCreateStatic2D();
  VecSet(&dimMat, 0, VecGet(dim, 0));
  VecSet(&dimMat, 1, VecGet(dim, 0));
  MatFloat* res = MatFloatCreate(&dimMat);
  // Loop on the matrix to set the covariances
  VecShort2D i = VecShortCreateStatic2D();
  do {
    // The matrix is symmetric, avoid calculating twice the same value
    if (VecGet(&i, 0) > VecGet(&i, 1)) {
      VecShort2D j = VecShortCreateStatic2D();
      VecSet(&j, 0, VecGet(&i, 1));
      VecSet(&j, 1, VecGet(&i, 0));
      MatSet(res, &i, MatGet(res, &j));
    } else {
      float covar = GDSGetCovariance(that, &i); 
      MatSet(res, &i, covar);
    }
  } while(VecStep(&i, &dimMat));
  // Return the covariance matrix
  return res;
}

// Get the covariance matrix of inputs of the GDataSetVecFloat 'that'
MatFloat* GDSGetInpCovarianceMatrix(const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the dimension of the inputksamples
  int dim = GDSGetNbInputs(that);
  // Allocate memory for the covariance matrix;
  VecShort2D dimMat = VecShortCreateStatic2D();
  VecSetAll(&dimMat, dim);
  MatFloat* res = MatFloatCreate(&dimMat);
  // Loop on the matrix to set the covariances
  VecShort2D i = VecShortCreateStatic2D();
  do {
    // The matrix is symmetric, avoid calculating twice the same value
    if (VecGet(&i, 0) > VecGet(&i, 1)) {
      VecShort2D j = VecShortCreateStatic2D();
      VecSet(&j, 0, VecGet(&i, 1));
      VecSet(&j, 1, VecGet(&i, 0));
      MatSet(res, &i, MatGet(res, &j));
    } else {
      float covar = GDSGetCovariance(that, &i); 
      MatSet(res, &i, covar);
    }
  } while(VecStep(&i, &dimMat));
  // Return the covariance matrix
  return res;
}

// Get the covariance of the variables at 'indices' in the
// GDataSetVecFloat 'that'
float GDSGetCovariance(const GDataSetVecFloat* const that,
  const VecShort2D* const indices) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (indices == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'indices' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a variable to memorize the result
  float res = 0.0;
  if (GDSGetSize(that) > 0) {
    // Get the means of the dataset
    VecFloat* means = GDSGetMean(that);
    // Calculate the covariance
    GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
    do {
      VecFloat* sample = GSetIterGet(&iter);
      res += (VecGet(sample, VecGet(indices, 0)) - 
        VecGet(means, VecGet(indices, 0))) *
        (VecGet(sample, VecGet(indices, 1)) - 
        VecGet(means, VecGet(indices, 1)));
    } while (GSetIterStep(&iter));
    res /= (float)GDSGetSize(that);
    // Free memory
    VecFree(&means);
  }
  // Return the covariance
  return res;
}

// Create a GDataSetVecFloat by importing the CSV file 'csvPath' and 
// decoding it with the 'importer'
// Return an empty GDatasetVecFloat if the importation failed
GDataSetVecFloat GDataSetCreateStaticFromCSV(
                    const char* const csvPath,
  const GDSVecFloatCSVImporter* const importer) {
#if BUILDMODE == 0
  if (csvPath == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'csvPath' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (importer == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'importer' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the result GDataSetVecFloat
  GDataSetVecFloat dataset;
  GDataSet* that = (GDataSet*)&dataset;
  *that = GDataSetCreateStatic(GDataSetType_VecFloat);

  // Initialise the properties
  that->_name = strdup(csvPath);
  that->_desc = strdup(csvPath);
  that->_sampleDim = VecShortCreate(1);
  VecSet(that->_sampleDim, 0, importer->_sizeSample); 
  that->_samples = GSetCreateStatic();

  // Open the csv file
  FILE* csvFile = fopen(csvPath, "r");

  // If we could open the file
  if (csvFile != NULL) {

    // Skip the header
    unsigned int nbSkip = 0;
    while (nbSkip < importer->_sizeHeader &&
      !feof(csvFile)) {
      char buffer;
      buffer = fgetc(csvFile);
      if (buffer == '\n')
        ++nbSkip;
    }
    
    // Load the samples line by line
    while (!feof(csvFile)) {
      
      // Allocate memory for a new sample
      VecFloat* sample = VecFloatCreate(importer->_sizeSample);
      
      // Decode each column
      for (unsigned int iCol = 0; 
        iCol < importer->_nbCol && !feof(csvFile); ++iCol) {

        // Read the value
        char buffer[1024];
        unsigned int iChar = 0;
        do {
          buffer[iChar] = fgetc(csvFile);
        } while (buffer[iChar] != importer->_sep &&
          buffer[iChar] != '\n' &&
          ++iChar && iChar < sizeof(buffer) && !feof(csvFile));
        buffer[iChar] = '\0';

        // Decode the value and set it into the sample
        if (importer->_converter != NULL) {
          importer->_converter(iCol, buffer, sample);
        } else {
          VecSet(sample, iCol, atof(buffer));
        }
      }
      // If we haven't reached the end of the file
      if (!feof(csvFile)) {
        
        // Add the sample to the dataset
        GSetAppend((GSet*)GDSSamples(that), sample);
      
      // Else, we are at the end of the file
      } else {

        // Free the memory
        VecFree(&sample);
      }
    }
    
    // Update the number of samples
    that->_nbSample = GSetNbElem(GDSSamples(that));

    // Close the csv file
    fclose(csvFile);
  }

  // Initialise the categories
  GDSResetCategories(that);
  
  // Return the dataset
  return dataset;
}

// Create a CSV importer for a GDataSetVecFloat
GDSVecFloatCSVImporter GDSVecFloatCSVImporterCreateStatic(
  const unsigned int sizeHeader,
          const char sep,
  const unsigned int nbCol,
  const unsigned int sizeSample,
                void (*converter)(
                  int col, 
                  char* val, 
                  VecFloat* sample)) {
  GDSVecFloatCSVImporter importer = {
    ._sizeHeader=sizeHeader,
    ._sep=sep,
    ._nbCol=nbCol,
    ._sizeSample=sizeSample,
    ._converter=converter
  };
  return importer;
}

// Function which return the JSON encoding of 'that' 
JSONNode* GDataSetVecFloatEncodeAsJSON(
  const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create the JSON structure
  JSONNode* json = JSONCreate();

  // Declare a buffer to convert value into string
  char val[100];

  // Encode the properties
  JSONAddProp(json, "dataSet", that->_dataSet._name);
  sprintf(val, "%d", that->_dataSet._type);
  JSONAddProp(json, "dataSetType", val);
  JSONAddProp(json, "desc", that->_dataSet._desc);
  sprintf(val, "%ld", GDSGetSize(that));
  JSONAddProp(json, "nbSample", val);
  sprintf(val, "%d", GDSGetNbInputs(that));
  JSONAddProp(json, "nbInputs", val);
  sprintf(val, "%d", GDSGetNbOutputs(that));
  JSONAddProp(json, "nbOutputs", val);
  JSONAddProp(json, "dim", VecEncodeAsJSON(that->_dataSet._sampleDim));
  JSONArrayStruct samples = JSONArrayStructCreateStatic();
  GSetIterForward iter = GSetIterForwardCreateStatic(GDSSamples(that));
  do {
    VecFloat* sample = GSetIterGet(&iter);
    JSONArrayStructAdd(&samples, 
      VecEncodeAsJSON(sample));
  } while (GSetIterStep(&iter));
  JSONAddProp(json, "samples", &samples);
  JSONArrayStructFlush(&samples);

  // Return the created JSON 
  return json;
}

// Function which return the JSON encoding of the category 'iCat'
// of'that' 
JSONNode* GDataSetVecFloatEncodeCategoryAsJSON(
  const GDataSetVecFloat* const that,
             const unsigned int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create the JSON structure
  JSONNode* json = JSONCreate();

  // Declare a buffer to convert value into string
  char val[100];

  // Encode the properties
  JSONAddProp(json, "dataSet", that->_dataSet._name);
  sprintf(val, "%d", that->_dataSet._type);
  JSONAddProp(json, "dataSetType", val);
  JSONAddProp(json, "desc", that->_dataSet._desc);
  sprintf(val, "%ld", GDSGetSizeCat(that, iCat));
  JSONAddProp(json, "nbSample", val);
  JSONAddProp(json, "dim", VecEncodeAsJSON(that->_dataSet._sampleDim));
  sprintf(val, "%d", GDSGetNbInputs(that));
  JSONAddProp(json, "nbInputs", val);
  sprintf(val, "%d", GDSGetNbOutputs(that));
  JSONAddProp(json, "nbOutputs", val);
  JSONArrayStruct samples = JSONArrayStructCreateStatic();
  GDSReset(that, iCat);
  do {
    VecFloat* sample = GDSGetSample(that, iCat);
    JSONArrayStructAdd(&samples, 
      VecEncodeAsJSON(sample));
    VecFree(&sample);
  } while (GDSStepSample(that, iCat));
  JSONAddProp(json, "samples", &samples);
  JSONArrayStructFlush(&samples);

  // Return the created JSON 
  return json;
}

// Save the GDataSetVecFloat to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool GDSVecFloatSave(
  const GDataSetVecFloat* const that, 
                    FILE* const stream, 
                     const bool compact) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'that' is null");
    PBErrCatch(GDataSetErr);
  }
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'stream' is null");
    PBErrCatch(GDataSetErr);
  }
#endif
  // Get the JSON encoding
  JSONNode* json = GDataSetVecFloatEncodeAsJSON(that);
  // Save the JSON
  if (!JSONSave(json, stream, compact)) {
    return false;
  }
  // Free memory
  JSONFree(&json);
  // Return success code
  return true;
}

// Save the category 'iCat' of the GDataSetVecFloat to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool GDSVecFloatSaveCategory(
  const GDataSetVecFloat* const that, 
                    FILE* const stream, 
                     const bool compact,
                      const int iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'that' is null");
    PBErrCatch(GDataSetErr);
  }
  if (stream == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'stream' is null");
    PBErrCatch(GDataSetErr);
  }
#endif
  // Get the JSON encoding
  JSONNode* json = GDataSetVecFloatEncodeCategoryAsJSON(
    that,
    iCat);
  // Save the JSON
  if (!JSONSave(json, stream, compact)) {
    return false;
  }
  // Free memory
  JSONFree(&json);
  // Return success code
  return true;
}

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
  const float threshold) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'that' is null");
    PBErrCatch(GDataSetErr);
  }
  if (nn == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'nn' is null");
    PBErrCatch(GDataSetErr);
  }
  if (iInputs == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'iInputs' is null");
    PBErrCatch(GDataSetErr);
  }
  if (iOutputs == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(GDataSetErr->_msg, "'iOutputs' is null");
    PBErrCatch(GDataSetErr);
  }
  if (VecGetDim(iInputs) != NNGetNbInput(nn)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(GDataSetErr->_msg, 
      "Dim of 'iInputs' is invalid (%ld==%d)",
      VecGetDim(iInputs),
      NNGetNbInput(nn));
    PBErrCatch(GDataSetErr);
  }
  if (VecGetDim(iOutputs) != NNGetNbOutput(nn)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(GDataSetErr->_msg, 
      "Dim of 'iOutputs' is invalid (%ld==%d)",
      VecGetDim(iOutputs),
      NNGetNbOutput(nn));
    PBErrCatch(GDataSetErr);
  }
#endif

  // Declare a variable to memorize the result
  float value = 0.0;
  
  // Declare a variable to memorize the nb of predicted samples
  long nbPredSample = 0;
  
  // Declare a variable to manage the threshold
  bool flag = true;

  // Declare variables for the input and output of the NeuraNet and
  // for the correct output of the NeuraNet
  VecFloat* inputNN = VecFloatCreate(NNGetNbInput(nn));
  VecFloat* outputNN = VecFloatCreate(NNGetNbOutput(nn));
  VecFloat* outputSample = VecFloatCreate(NNGetNbOutput(nn));

  // Reset the iterator on samples
  GDSReset(that, iCat);
  
  // Loop on the samples of the requested category
  do {
    // Get a clone of the sample
    VecFloat* sample = GDSGetSample(that, iCat);

    // Create the input of the NeuraNet from the sample
    for (int iInput = NNGetNbInput(nn); iInput--;) {
      VecSet(inputNN, iInput, 
        VecGet(sample, VecGet(iInputs, iInput)));
    }

    // Run the prediction
    NNEval(nn, inputNN, outputNN);

    // Create the correct output of the NeuraNet from the sample
    for (int iOutput = NNGetNbOutput(nn); iOutput--;) {
      VecSet(outputSample, iOutput, 
        VecGet(sample, VecGet(iOutputs, iOutput)));
    }

    // Calculate the value on this sample
    VecFloat* diff = VecGetOp(outputNN, 1.0, outputSample, -1.0);
    float sampleValue = VecNorm(diff);
    VecFree(&diff);
    
    // Update the total value
    value += sampleValue;
    
    // Check against the threshold
    float bestPossible = -1.0 * value / (float)GDSGetSizeCat(that, iCat);
    if (bestPossible <= threshold) {
      flag = false;
    }

    // Free memory
    VecFree(&sample);
    
    // Increment the nb of predicted samples
    ++nbPredSample;

  } while (flag && GDSStepSample(that, iCat));

  // Update the total value
  value /= (float)nbPredSample;

  // Free memory
  VecFree(&inputNN);
  VecFree(&outputNN);
  VecFree(&outputSample);
  
  // Return the value of the NeuraNet on the samples
  return value * -1.0;
}

// Create a new GDataSetVecFloat
GDataSetVecFloat GDataSetVecFloatCreateStatic(void) {
  // Declare the result GDataSetVecFloat
  GDataSetVecFloat that;
  
  // Initialise properties
  that._dataSet = GDataSetCreateStatic(GDataSetType_VecFloat);
  
  // Return the dataset
  return that;
}

// Get the proximity matrix of the samples of category 'iCat' in the
// GDataSetVecFloat 'that'
// M[i,j] = euclidean distance between the i-th sample and the j-th sample
MatFloat* GDSVecFloatGetProxMat(
  const GDataSetVecFloat* that,
             unsigned int iCat) {

#if BUILDMODE == 0

  if (that == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'that' is null");
    PBErrCatch(GDataSetErr);

  }

#endif

  // Allocate memory for the result matrix
  long nbSamples =
    GDSGetSizeCat(
      that,
      iCat);
  VecShort2D v = VecShortCreateStatic2D();
  VecSet(
    &v,
    0,
    nbSamples);
  VecSet(
    &v,
    1,
    nbSamples);
  MatFloat* proxMat = MatFloatCreate(&v);

  // Declare another vector used to manipulate the matrix
  VecShort2D w = VecShortCreateStatic2D();

  // Declare two variables to memorize the indices of samples
  long iSample = 0;
  long jSample = 0;

  // Loop on the samples of the category
  GDSReset(
    that,
    iCat);
  bool flagStepI = true;
  bool flagStepJ = true;
  do {

    // Get the sample
    VecFloat* sampleI = GSetIterGet(((GDataSet*)that)->_iterators + iCat);

    // Update the index to manipulate the matrix
    VecSet(
      &v,
      0,
      iSample);
    VecSet(
      &w,
      1,
      iSample);

    // Create a temporary iterator to loop on the following sample
    GSetIterForward iterJ = ((GDataSet*)that)->_iterators[iCat];
    jSample = iSample;

    // Loop on the following samples
    do {

      // Get the following sample
      VecFloat* sampleJ = GSetIterGet(&iterJ);

      // Get the distance between the sample and the following sample
      float dist = 0.0;
      if (iSample != jSample) {

        dist =
          VecDist(
            sampleI,
            sampleJ);

      }

      // Update the index to manipulate the matrix
      VecSet(
        &v,
        1,
        jSample);
      VecSet(
        &w,
        0,
        jSample);

      // Update the proximity matrix
      MatSet(
        proxMat,
        &v,
        dist);
      MatSet(
        proxMat,
        &w,
        dist);

      // Step to the next following sample
      flagStepJ = GSetIterStep(&iterJ);
      ++jSample;

    } while (flagStepJ);

    // Step to the next sample
    flagStepI =
      GDSStepSample(
        that,
        iCat);
    ++iSample;

  } while (flagStepI);

  // Return the proximity matrix
  return proxMat;

}

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
                      int iCat) {

#if BUILDMODE == 0

  if (that == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'that' is null");
    PBErrCatch(GDataSetErr);

  }

  if (target == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'target' is null");
    PBErrCatch(GDataSetErr);

  }

#endif

  // Declare the pointer to the result sample
  VecFloat* nearestNeighbour = NULL;

  // Declare a variable to store the best distance
  float bestDist = -1.0;

  // Create the set of examined samples
  GSet done = GSetCreateStatic();

  // Create the set of not yet examined samples 
  GSet todo = GSetCreateStatic();
  GDSReset(
    that,
    iCat);
  bool flagStep = true;
  do {

    VecFloat* sample = GSetIterGet(((GDataSet*)that)->_iterators + iCat);
    GSetAppend(
      &todo,
      sample);
    flagStep =
      GDSStepSample(
        that,
        iCat);

  } while(flagStep);

  // Declare the pointer to the candidate sample and initialise it
  // to the first sample not yet examined
  VecFloat* candidate = GSetPop(&todo);

  // While there is a candidate to examined
  while(candidate != NULL) {

    // Get the distance from the candidate to the target
    float distCandidate =
      VecDist(
        candidate,
        target);

    // Add the candidate to the set of examined samples sorted on
    // their distance to the target
    GSetAddSort(
      &done,
      candidate,
      distCandidate);

    // If there is no current best or the distance is less than the
    // current best
    if (
      bestDist < 0.0 ||
      distCandidate < bestDist) {

      // Update the current best
      bestDist = distCandidate;
      nearestNeighbour = candidate;

    }

    // Create a temporary set to update the lower bound
    GSet update = GSetCreateStatic();

    // For each sample not yet examined
    while(GSetNbElem(&todo) > 0) {

      // Get the sample
      VecFloat* sample = GSetPop(&todo);

      // Get the lower bound of the sample
      float lowerBound = 0.0;
      GSetIterForward iter = GSetIterForwardCreateStatic(&done);
      do {

        VecFloat* prevCandidate = GSetIterGet(&iter);
        const GSetElem* elem = GSetIterGetElem(&iter);
        float l =
          GSetElemGetSortVal(elem) -
          VecDist(
            sample,
            prevCandidate);
        if (l > lowerBound) {

          lowerBound = l;

        }

      } while(
        GSetIterStep(&iter) &&
        lowerBound < bestDist);

      // Add the sample to the updated set if its lower bound is below
      // the current best distance
      if (lowerBound < bestDist) {

        GSetAddSort(
          &update,
          sample,
          lowerBound);

      }

    }

    // Swap the todo and update sets
    todo = update;

    // Get the next candidate as the one with lowest bound in the list of
    // not yet examined samples
    candidate = GSetPop(&todo);

  }

  // Free memory
  GSetFlush(&done);

  // Return the nearest neighbour
  return nearestNeighbour;

}

// Get the nearest (in term of euclidean distance) sample of category
// 'iCat' to 'target' in the GDataSetVecFloat 'that' using brute force
VecFloat* GDSVecFloatNearestNeighbourBrute(
  const GDataSetVecFloat* that,
          const VecFloat* target,
                      int iCat) {

#if BUILDMODE == 0

  if (that == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'that' is null");
    PBErrCatch(GDataSetErr);

  }

  if (target == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'target' is null");
    PBErrCatch(GDataSetErr);

  }

#endif

  // Declare the pointer to the result
  VecFloat* nearest = NULL;

  // Declare a variable to memorize the best distance
  float bestDist = 0.0;

  // Loop on samples
  do {

    // Get the sample
    VecFloat* sample =
      GDSGetSample(
        that,
        iCat);

    // Get the distance to the target
    float dist =
      VecDist(
        target,
        sample);

    // If there is no nearest yet or the distance is better
    if (
      nearest == NULL ||
      dist < bestDist) {

        // Free memory used by the copy of the current best sample
        VecFree(&nearest);

        // Update the nearest sample
        bestDist = dist;
        nearest = sample;

    // Else, it is farther than the current best
    } else {

      // Free memory used by the copy of the sample
      VecFree(&sample);

    }

  } while(GDSStepSample(that, iCat));

  // Return the result
  return nearest;

}

// Check if the 'sample' in the category 'iCat' of the
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
             unsigned int nbOpponent) {

#if BUILDMODE == 0

  if (that == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'that' is null");
    PBErrCatch(GDataSetErr);

  }

  if (iCat >= GDSGetNbCat(that)) {

    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(
      GDataSetErr->_msg,
      "'iCat' is invalid (%ld<=%ld)",
      iCat,
      GDSGetNbCat(that));
    PBErrCatch(GDataSetErr);

  }

  if (sample == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'sample' is null");
    PBErrCatch(GDataSetErr);

  }

  if (GDSGetNbInputs(that) <= 0) {

    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(
      GDataSetErr->_msg,
      "No inputs in the sample");
    PBErrCatch(GDataSetErr);

  }

  if (GDSGetNbOutputs(that) <= 0) {

    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(
      GDataSetErr->_msg,
      "No outputs in the sample");
    PBErrCatch(GDataSetErr);

  }

#endif

  // If there is no sample in the category
  if (GDSGetSizeCat(that, iCat) == 0) {
    return false;
  }

  // Variable to memorise the nearest distance
  double nearestDist = -1.0;

  // Get the inputs and outputs of the checked sample
  VecFloat* inputs = VecFloatCreate(GDSGetNbInputs(that));
  for (short i = GDSGetNbInputs(that); i--;) {
    VecSet(inputs, i, VecGet(sample, i));
  }
  VecFloat* outputs = VecFloatCreate(GDSGetNbOutputs(that));
  for (short i = GDSGetNbOutputs(that); i--;) {
    VecSet(outputs, i, VecGet(sample, i + GDSGetNbInputs(that)));
  }

  // Create an iterator to loop on the samples of the category
  GSetIterForward iter = ((GDataSet*)that)->_iterators[iCat];
  GSetIterReset(&iter);

  // Loop on the samples
  do {
    // Get the sample
    const VecFloat* sampleIter = GSetIterGet(&iter);
    // Get the sample outputs
    VecFloat* outputsIter = VecFloatCreate(GDSGetNbOutputs(that));
    for (short i = GDSGetNbOutputs(that); i--;) {
      VecSet(outputsIter, i, VecGet(sampleIter, i + GDSGetNbInputs(that)));
    }
    // Get the distance to the checked sample
    float dist =
      VecDist(
        outputsIter,
        outputs);
    // If it's the same output
    if (ISEQUALF(dist, 0.0)) {
      // Get the sample inputs
      VecFloat* inputsIter = VecFloatCreate(GDSGetNbInputs(that));
      for (short i = GDSGetNbInputs(that); i--;) {
        VecSet(inputsIter, i, VecGet(sampleIter, i));
      }
      // Get the distance to the checked sample
      dist =
        VecDist(
          inputsIter,
          inputs);
      // If it's nearer that the actual nearest
      if (dist > PBMATH_EPSILON && (nearestDist < 0.0 || nearestDist > dist)) {
        // Update the nearest dist
        nearestDist = dist;
      }
      // Free memory
      VecFree(&inputsIter);
    }
    // Free memory
    VecFree(&outputsIter);
  } while (GSetIterStep(&iter));

  // Declare a variable to count the opponents
  long nb = 0;
  // Reset the iterator
  GSetIterReset(&iter);

  // Loop on the samples
  do {
    // Get the sample
    const VecFloat* sampleIter = GSetIterGet(&iter);
    // Get the sample outputs
    VecFloat* outputsIter = VecFloatCreate(GDSGetNbOutputs(that));
    for (short i = GDSGetNbOutputs(that); i--;) {
      VecSet(outputsIter, i, VecGet(sampleIter, i + GDSGetNbInputs(that)));
    }
    // Get the distance to the checked sample
    float dist =
      VecDist(
        outputsIter,
        outputs);
    // If it's not the same output
    if (!ISEQUALF(dist, 0.0)) {
      // Get the sample inputs
      VecFloat* inputsIter = VecFloatCreate(GDSGetNbInputs(that));
      for (short i = GDSGetNbInputs(that); i--;) {
        VecSet(inputsIter, i, VecGet(sampleIter, i));
      }
      // Get the distance to the checked sample
      dist =
        VecDist(
          inputsIter,
          inputs);
      // If it's nearerer than the nearest non opponent
      if (nearestDist > dist) {
        // Increment the number of opponents
        ++nb;
      }
      // Free memory
      VecFree(&inputsIter);
    }
    // Free memory
    VecFree(&outputsIter);
  } while (GSetIterStep(&iter));

  // Free memory
  VecFree(&inputs);
  VecFree(&outputs);

  // Return the result
  if (nb > nbOpponent) {
    return true;
  } else {
    return false;
  }

}

// Remove the outliers from the category 'iCat', check
// GDSVecFloatIsOutlierSampleCat to see how outliers are identified.
// Return a GSetVecFloat containing the removed outliers. The _split
// property is updated with the remaining number of sample in the
// category
GSetVecFloat* GDSVecFloatRemoveOutlierCat(
  const GDataSetVecFloat* that,
                     long iCat,
             unsigned int nbOpponent) {

#if BUILDMODE == 0

  if (that == NULL) {

    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(
      GDataSetErr->_msg,
      "'that' is null");
    PBErrCatch(GDataSetErr);

  }

  if (iCat >= GDSGetNbCat(that)) {

    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(
      GDataSetErr->_msg,
      "'iCat' is invalid (%ld<=%ld)",
      iCat,
      GDSGetNbCat(that));
    PBErrCatch(GDataSetErr);

  }

#endif

  // Create the set of outliers
  GSetVecFloat* outliers = GSetVecFloatCreate();

  // If there is no sample in the category
  if (GDSGetSizeCat(that, iCat) == 0) {
    return outliers;
  }

  // Loop on the samples
  GDSReset(that, iCat);
  bool step = true;
  do {
    step = true;
    VecFloat* sample = GSetIterGet(((GDataSet*)that)->_iterators + iCat);
    if (GDSIsOutlierSampleCat(that, sample, iCat, nbOpponent)) {
      GSetAppend(
        outliers,
        sample);
      step = GSetIterRemoveElem(((GDataSet*)that)->_iterators + iCat);
    } else {
      step = false;
    }
  } while (step || GDSStepSample(that, iCat));
  VecSet(
    ((GDataSet*)that)->_split,
    iCat,
    GSetNbElem(((GDataSet*)that)->_categories + iCat));

  // Return the outliers
  return outliers;

}
