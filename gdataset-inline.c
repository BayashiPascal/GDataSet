// ============ GDATASET_static inline.C ================

// ================ Functions implementation ====================

// Get the total number of samples in the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetSize(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbSample;
}

// Get the total number of samples in the GDataSet 'that' for the 
// category 'iCat'. Return 0 if the category doesn't exists
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetSizeCat(const GDataSet* const that, const long iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (that->_split == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that->_split' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%ld<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return (that->_split ? VecGet(that->_split, iCat) : 0);
}

// Unsplit the GDataSet 'that', i.e. after calling GDataSetUnsplit 'that' 
// has only one category containing all the samples
#if BUILDMODE != 0
static inline
#endif 
void _GDSUnsplit(GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Unsplitting is equivalent to splitting in one category with all the 
  // samples
  VecShort* split = VecShortCreate(1);
  VecSet(split, 0, GDSGetSize(that));
  GDSSplit(that, split);
  VecFree(&split);
}

// Shuffle the samples of the GDataSet 'that'.
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffle(GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Shuffle the samples
  GSetShuffle(&(that->_samples));
}

// Shuffle the samples of the category 'iCat' of the GDataSet 'that'.
// Reset the iterator of the category
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffleCat(GDataSet* const that, const long iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (that->_categories == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that->_categories' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%ld<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Shuffle the GSet of the category
  if (that->_categories)
    GSetShuffle(that->_categories + iCat);
  // Reset the iterator
  GDSReset(that, iCat);
}

// Shuffle the samples of all the categories of the GDataSet 'that'.
// Reset the iterator of the categories
#if BUILDMODE != 0
static inline
#endif 
void _GDSShuffleAll(GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Loop on categories
  for (int iCat = GDSGetNbCat(that); iCat--;)
    // Shuffle the category
    GDSShuffleCat(that, iCat);
}

// Get the name of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const char* _GDSName(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_name;
}

// Get the description of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const char* _GDSDesc(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_desc;
}

// Get the type of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
GDataSetType _GDSGetType(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_type;
}

// Get the number of categories of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
long _GDSGetNbCat(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return (that->_split ? VecGetDim(that->_split) : 0);
}

// If there is a next sample move to the next sample of the category 
// 'iCat' and return true, else return false
#if BUILDMODE != 0
static inline
#endif 
bool _GDSStepSample(const GDataSet* const that, const long iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (that->_iterators == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that->_iterators' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%ld<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return (that->_iterators ? 
    GSetIterStep(that->_iterators + iCat) : false);
}

// Reset the iterator on category 'iCat' of the GDataSet 'that', i.e. 
// the next call to GDataSetGetNextSample will give the first sample of 
// the category 'iCat'
#if BUILDMODE != 0
static inline
#endif 
void _GDSReset(GDataSet* const that, const long iCat) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (that->_iterators == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that->_iterators' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (iCat < 0 || iCat >= GDSGetNbCat(that)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'iCat' is invalid (0<=%ld<%ld)",
      iCat, GDSGetNbCat(that));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  if (that->_iterators)
    GSetIterReset(that->_iterators + iCat);
}

// Reset the iterator on all categories of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
void _GDSResetAll(GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Loop on categories
  for (int iCat = GDSGetNbCat(that); iCat--;)
    // Reset the category
    GDSReset(that, iCat);
}

// Get the dimensions of the samples of GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const VecShort* _GDSSampleDim(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_sampleDim;
}

// Get the number of masks in the GDataSetGenBrushPair 'that'
#if BUILDMODE != 0
static inline
#endif 
int GDSGetNbMaskGenBrushPair(const GDataSetGenBrushPair* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbMask;
}

// Get the samples of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
const GSet* _GDSSamples(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return &(that->_samples);
}
#if BUILDMODE != 0
static inline
#endif 
const GSetVecFloat* _GDSVecFloatSamples(
  const GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return (GSetVecFloat*)&(that->_dataSet._samples);
}
#if BUILDMODE != 0
static inline
#endif 
const GSet* _GDSGenBrushPairSamples(
  const GDataSetGenBrushPair* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return &(that->_dataSet._samples);
}

// Remove all the samples of the GDataSetVecFloat 'that'
#if BUILDMODE != 0
static inline
#endif
void _GDSVecFloatRemoveAllSample(GDataSetVecFloat* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  while (GSetNbElem(GDSSamples(that)) > 0) {
    VecFloat* sample = GSetPop((GSet*)GDSSamples(that));
    VecFree(&sample);
  }
  that->_dataSet._nbSample = 0;
  GDSUnsplit(that);
}

// Remove all the samples of the GDataSetGenBrushPair 'that'
#ifdef GENBRUSH_H
#if BUILDMODE != 0
static inline
#endif 
void _GDSGenBrushPairRemoveAllSample(GDataSetGenBrushPair* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  while (GSetNbElem(GDSSamples(that)) > 0) {
    GDSGenBrushPair* sample = GSetPop((GSet*)GDSSamples(that));
    GDSGenBrushPairFree(&sample);
  }
  that->_dataSet._nbSample = 0;
  GDSUnsplit(that);
}
#endif 

// Append 'sample' in the GDataSetVecFloat 'that'
#if BUILDMODE != 0
static inline
#endif 
void _GDSVecFloatAddSample(
  GDataSetVecFloat* const that,
  VecFloat* sample) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (sample == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'sample' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (VecGetDim(sample) != VecGet(GDSSampleDim(that), 0)) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg,
      "'sample' has invalid dimensions (%ld!=%d)",
      VecGetDim(sample), VecGet(GDSSampleDim(that), 0));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  GSetAppend((GSet*)GDSSamples(that), sample);
  that->_dataSet._nbSample += 1;
}

// Append 'sample' in the GDataSetGenBrush 'that'
#ifdef GENBRUSH_H
#if BUILDMODE != 0
static inline
#endif 
void _GDSGenBrushPairAddSample(
  GDataSetGenBrushPair* const that,
  GDSGenBrushPair* sample) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (sample == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'sample' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (VecIsEqual(GBDim(sample), GDSSampleDim(&dataset)) == FALSE) {
    GDataSetErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg,
      "'sample' has invalid dimensions ((%ld,%ld)!=(%ld,%ld))",
      VecGet(GBDim(sample), 0),
      VecGet(GBDim(sample), 1),
      VecGet(GDSSampleDim(&dataset), 0),
      VecGet(GDSSampleDim(&dataset), 1));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  GSetAppend((GSet*)GDSSamples(that), sample);
  that->_dataSet._nbSample += 1;
}
#endif 

// Get the number of input values in one sample of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
short _GDSGetNbInputs(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbInputs;
}

// Get the number of output values in one sample of the GDataSet 'that'
#if BUILDMODE != 0
static inline
#endif 
short _GDSGetNbOutputs(const GDataSet* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbOutputs;
}

// Set the number of input values in one sample of the GDataSet 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif 
void _GDSSetNbInputs(GDataSet* const that, const short nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_nbInputs = nb;
}

// Set the number of output values in one sample of the GDataSet 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif 
void _GDSSetNbOutputs(GDataSet* const that, const short nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    GDataSetErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_nbOutputs = nb;
}

