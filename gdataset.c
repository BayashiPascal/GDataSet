// ============ GDATASET_C ================

// ================= Include =================

#include "gdataset.h"
#if BUILDMODE == 0
#include "gdataset-inline.c"
#endif

// ================ Functions implementation ====================

// Create a new GDataSet defined by the file at 'cfgFilePath'
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
  // Create the initial category
  that._dataSet._split = VecShortCreate(1);
  VecSet(that._dataSet._split, 0, that._dataSet._nbSample);
  that._dataSet._categories = PBErrMalloc(GDataSetErr, sizeof(GSet));
  that._dataSet._categories[0] = GSetCreateStatic();
  GSetIterForward iter = 
    GSetIterForwardCreateStatic(&(that._dataSet._samples));
  do {
    void* sample = GSetIterGet(&iter);
    GSetAppend(that._dataSet._categories, sample);
  } while (GSetIterStep(&iter));
  that._dataSet._iterators = 
    PBErrMalloc(GDataSetErr, sizeof(GSetIterForward));
  that._dataSet._iterators[0] = 
    GSetIterForwardCreateStatic(that._dataSet._categories);
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
  // Get the nb of mask
  JSONNode* prop = JSONProperty(that._dataSet._json, "nbMask");
  if (prop == NULL) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (nbMask missing)");
    PBErrCatch(GDataSetErr);
  }
  that._nbMask = atoi(JSONLabel(JSONValue(prop, 0)));
  if (that._nbMask >= GDS_NBMAXMASK) {
    GDataSetErr->_type = PBErrTypeInvalidData;
    sprintf(GDataSetErr->_msg, 
      "Invalid description file (invalid nbMask %d>=%d)", 
      that._nbMask, GDS_NBMAXMASK);
    PBErrCatch(GDataSetErr);
  }
  // Load the samples
  prop = JSONProperty(that._dataSet._json, "samples");
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
    for (int iMask = 0; iMask < that._nbMask; ++iMask) {
      subVal = JSONValue(subProp, iMask);
      pair->_path[1 + iMask] = PBErrMalloc(GDataSetErr, 
        sizeof(char) * (strlen(JSONLabel(subVal)) + 1));
      strcpy(pair->_path[1 + iMask], JSONLabel(subVal));
    }
    // Add the pair to the samples
    GSetAppend(&(that._dataSet._samples), pair);
  }
  // Create the initial category
  that._dataSet._split = VecShortCreate(1);
  VecSet(that._dataSet._split, 0, that._dataSet._nbSample);
  that._dataSet._categories = PBErrMalloc(GDataSetErr, sizeof(GSet));
  that._dataSet._categories[0] = GSetCreateStatic();
  GSetIterForward iter = 
    GSetIterForwardCreateStatic(&(that._dataSet._samples));
  do {
    void* sample = GSetIterGet(&iter);
    GSetAppend(that._dataSet._categories, sample);
  } while (GSetIterStep(&iter));
  that._dataSet._iterators = 
    PBErrMalloc(GDataSetErr, sizeof(GSetIterForward));
  that._dataSet._iterators[0] = 
    GSetIterForwardCreateStatic(that._dataSet._categories);
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
  char* root = GDSGetCfgFolderPath(that);
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
  tho->_json = NULL;
  tho->_cfgFilePath = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (1 + strlen(that->_dataSet._cfgFilePath)));
  strcpy(tho->_cfgFilePath, that->_dataSet._cfgFilePath);
  tho->_name = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (1 + strlen(that->_dataSet._name)));
  strcpy(tho->_name, that->_dataSet._name);
  tho->_desc = PBErrMalloc(GDataSetErr, 
    sizeof(char) * (1 + strlen(that->_dataSet._desc)));
  strcpy(tho->_desc, that->_dataSet._desc);
  tho->_type = that->_dataSet._type;
  tho->_nbSample = that->_dataSet._nbSample;
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
  tho->_split = VecShortCreate(1);
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

