// ============ GDATASET_C ================

// ================= Include =================

#include "gdataset.h"
#if BUILDMODE == 0
#include "gdataset-inline.c"
#endif

// ================ Functions implementation ====================

// Create a new GDataSet defined by the file at 'cfgFilePath'
// The random generator must have been initialized before calling 
// this function
GDataSet GDataSetCreateStatic(const char* const cfgFilePath) {
#if BUILDMODE == 0
  if (cfgFilePath == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'cfgFilePath' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new GDataSet
  GDataSet that;
  // Copy the file path
  that._cfgFilePath = PBErrMalloc(GDataSetErr, strlen(cfgFilePath) + 1);
  strcpy(that._cfgFilePath, cfgFilePath);
  // Open the description file
  FILE* cfgFile = fopen(cfgFilePath, "r");
  // If the description file doesn't exist
  if (cfgFile == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(GDataSetErr->_msg, "Can't open the configuration file %s",
      cfgFilePath);
    PBErrCatch(GDataSetErr);
  }
  // Load the encoded data
  that._json = JSONCreate();
  // Load the whole encoded data
  if (JSONLoad(that._json, cfgFile) == false) {
    printf("%s\n", GDataSetErr->_msg);
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, "Can't load the configuration file");
    PBErrCatch(GDataSetErr);
  }
  // Decode dataSet
  JSONNode* prop = JSONProperty(that._json, "dataSet");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dataSet missing)");
    PBErrCatch(GDataSetErr);
  }
  JSONNode* val = JSONValue(prop, 0);
  that._name = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (strlen(JSONLabel(val)) + 1));
  strcpy(that._name, JSONLabel(val));
  // Decode desc
  prop = JSONProperty(that._json, "desc");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (desc missing)");
    PBErrCatch(GDataSetErr);
  }
  val = JSONValue(prop, 0);
  that._desc = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (strlen(JSONLabel(val)) + 1));
  strcpy(that._desc, JSONLabel(val));
  // Decode dataSetType
  prop = JSONProperty(that._json, "dataSetType");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dataSetType missing)");
    PBErrCatch(GDataSetErr);
  }
  val = JSONValue(prop, 0);
  that._type = atoi(JSONLabel(val));
  // Decode dim
  prop = JSONProperty(that._json, "dim");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (dim missing)");
    PBErrCatch(GDataSetErr);
  }
  that._sampleDim = NULL;
  VecDecodeAsJSON(&(that._sampleDim), prop);
  // Decode nbSample
  prop = JSONProperty(that._json, "nbSample");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbSample missing)");
    PBErrCatch(GDataSetErr);
  }
  val = JSONValue(prop, 0);
  that._nbSample = atoi(JSONLabel(val));
  // Init the splits and iterators
  that._split = NULL;
  that._categories = NULL;
  that._iterators = NULL;
  
  // Close the description file
  fclose(cfgFile);
  
  // Return the new GDataSet
  return that;
}

// Free the memory used by a GDataSet
void GDataSetFreeStatic(GDataSet* const that) {
  if (that == NULL)
    return;
  // Free memory
  JSONFree(&(that->_json));
  free(that->_name);
  free(that->_desc);
  free(that->_cfgFilePath);
  for (int iCat = GDSGetNbCat(that); iCat--;) {
    GSetFlush(that->_categories + iCat);
  }
  free(that->_categories);
  free(that->_iterators);
  VecFree(&(that->_split));
  VecFree(&(that->_sampleDim));
}

// Create a new GDataSetVecFloat defined by the file at 'cfgFilePath'
// The random generator must have been initialized before calling 
// this function
GDataSetVecFloat GDataSetVecFloatCreateStatic(
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
  // Create the generic GDataSet
  that._dataSet = GDataSetCreateStatic(cfgFilePath);
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
  // Load the samples
  JSONNode* prop = JSONProperty(that._dataSet._json, "samples");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples missing)");
    PBErrCatch(GDataSetErr);
  }
  if (JSONGetNbValue(prop) != that._dataSet._nbSample) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples's number != nbSample)");
    PBErrCatch(GDataSetErr);
  }
  that._dataSet._samples = GSetCreateStatic();
  for (int iSample = 0; iSample < that._dataSet._nbSample; ++iSample) {
    JSONNode* val = JSONValue(prop, iSample);
    VecFloat* v = NULL;
    VecDecodeAsJSON(&v, val);
    GSetAppend(&(that._dataSet._samples), v);
  }
  // Init the split
  VecShort* split = VecShortCreate(1);
  VecSet(split, 0, that._dataSet._nbSample);
  GDSSplit(&that, split);
  VecFree(&split);
  // Return the new GDataSetVecFloat
  return that;
}

// Free the memory used by a GDataSetVecFloat
void GDataSetVecFloatFreeStatic(GDataSetVecFloat* const that) {
  if (that == NULL)
    return;
  // Free memory
  GDataSetFreeStatic((GDataSet*)that);
  while (GSetNbElem(&(((GDataSet*)that)->_samples)) > 0) {
    VecFloat* sample = GSetPop(&(((GDataSet*)that)->_samples));
    VecFree(&sample);
  }
}

// Create a new GDataSetGenBrushPair defined by the file at 'cfgFilePath'
// The random generator must have been initialized before calling 
// this function
GDataSetGenBrushPair GDataSetGenBrushPairCreateStatic(
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
  // Create the generic GDataSet
  that._dataSet = GDataSetCreateStatic(cfgFilePath);
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
  // Load the samples
  JSONNode* prop = JSONProperty(that._dataSet._json, "samples");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples missing)");
    PBErrCatch(GDataSetErr);
  }
  if (JSONGetNbValue(prop) != that._dataSet._nbSample) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (samples's number != nbSample)");
    PBErrCatch(GDataSetErr);
  }
  that._dataSet._samples = GSetCreateStatic();
  for (int iSample = 0; iSample < that._dataSet._nbSample; ++iSample) {
    JSONNode* val = JSONValue(prop, iSample);
    // Allocate memory for the pair image/mask
    GDSFilePathPair* pair = PBErrMalloc(GDataSetErr, 
      sizeof(GDSFilePathPair));
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
    subVal = JSONValue(subProp, 0);
    pair->_path[1] = PBErrMalloc(GDataSetErr, 
      sizeof(char) * (strlen(JSONLabel(subVal)) + 1));
    strcpy(pair->_path[1], JSONLabel(subVal));
    // Add the pair to the samples
    GSetAppend(&(that._dataSet._samples), pair);
  }
  // Init the split
  VecShort* split = VecShortCreate(1);
  VecSet(split, 0, that._dataSet._nbSample);
  GDSSplit(&that, split);
  VecFree(&split);
  // Return the new GDataSetVecFloat
  return that;
}

// Free the memory used by a GDataSetGenBrushPair
void GDataSetGenBrushPairFreeStatic(GDataSetGenBrushPair* const that) {
  if (that == NULL)
    return;
  // Free memory
  GDataSetFreeStatic((GDataSet*)that);
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
// Each category must have at least one sample. Samples are allocated // randomly to the categories.
// If 'that' was already splitted the previous splitting is discarded.
void _GDSSplit(GDataSet* const that, const VecShort* const cat) {
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
  // Shuffle the samples
  GSetShuffle(&(that->_samples));
  // Declare an iterator on the samples
  GSetIterForward iter = GSetIterForwardCreateStatic(&(that->_samples));
  // Loop on categories
  for (long iCat = nbCat; iCat--;) {
    // Get the nb of samples for this category
    long nbSample = VecGet(cat, iCat);
    // Loop on the sample
    for (long iSample = nbSample; iSample--; GSetIterStep(&iter)) {
      // Get the next sample
      void* sample = GSetIterGet(&iter);
      // Add the sample to the category
      GSetAppend(that->_categories + iCat, sample);
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
  char* root = GDSGetCfgFolderPath(that);
  char* path = PBFSJoinPath(root, pairFile->_path[0]);
  GenBrush* gb = GBCreateFromFile(path);
  // Rescale the sample if needed to always provide to the user
  // the dimensions defined in the configuration file of the data set
  if (!VecIsEqual(GBDim(gb), GDSSampleDim(that))) {
    pairSample->_img = GBScale(gb, 
      (const VecShort2D*)GDSSampleDim(that), GBScaleMethod_Default);
    GBFree(&gb);
  } else {
    pairSample->_img = gb;
  }
  free(path);
  path = PBFSJoinPath(root, pairFile->_path[1]);
  gb = GBCreateFromFile(path);
  if (!VecIsEqual(GBDim(gb), GDSSampleDim(that))) {
    pairSample->_mask = GBScale(gb, 
      (const VecShort2D*)GDSSampleDim(that), GBScaleMethod_Default);
    GBFree(&gb);
  } else {
    pairSample->_mask = gb;
  }
  free(path);
  free(root);
  return pairSample;
}

// Release the memory used by the FilePathPair 'that'
void GDSFilePathPairFree(GDSFilePathPair** const that) {
  if (that == NULL || *that == NULL)
    return;
  free((*that)->_path[0]);
  free((*that)->_path[1]);
  free(*that);
  *that = NULL;
}

// Release the memory used by the GenBrushPair 'that'
void GDSGenBrushPairFree(GDSGenBrushPair** const that) {
  if (that == NULL || *that == NULL)
    return;
  GBFree(&((*that)->_img));
  GBFree(&((*that)->_mask));
  free(*that);
  *that = NULL;
}

