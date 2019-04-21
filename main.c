#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "genbrush.h"
#include "gdataset.h"

void UnitTestGDataSetVecFloatCreateFreeClone() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  if (strcmp(gdataset._dataSet._cfgFilePath, cfgFilePath) != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDataSetCreateStatic failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSet* g = (GDataSet*)(&gdataset);
  if (GSetGet(g->_categories, 0) != GSetGet(&(g->_samples), 0) ||
    GSetGet(g->_categories, 1) != GSetGet(&(g->_samples), 1) ||
    GSetGet(g->_categories, 2) != GSetGet(&(g->_samples), 2)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDataSetCreateStatic failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSetVecFloat clone = GDSClone(&gdataset);
  if (strcmp(clone._dataSet._cfgFilePath, cfgFilePath) != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSClone failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSet* f = (GDataSet*)(&clone);
  if (GSetGet(f->_categories, 0) != GSetGet(&(f->_samples), 0) ||
    GSetGet(f->_categories, 1) != GSetGet(&(f->_samples), 1) ||
    GSetGet(f->_categories, 2) != GSetGet(&(f->_samples), 2)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSClone failed");
    PBErrCatch(GDataSetErr);
  }
  
  GDataSetVecFloatFreeStatic(&clone);
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatCreateFreeClone OK\n");
}

void UnitTestGDataSetVecFloatGet() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  if (strcmp(GDSCfgFilePath(&gdataset), cfgFilePath) != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSCfgFilePath failed");
    PBErrCatch(GDataSetErr);
  }
  char* str = GDSGetCfgFilePath(&gdataset);
  if (strcmp(str, cfgFilePath) != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetCfgFilePath failed");
    PBErrCatch(GDataSetErr);
  }
  free(str);
  str = GDSGetCfgFolderPath(&gdataset);
  if (strcmp(str, "") != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetCfgFolderPath failed");
    PBErrCatch(GDataSetErr);
  }
  free(str);
  if (strcmp(GDSDesc(&gdataset), "UnitTestGDataSetCreateFree") != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSDesc failed");
    PBErrCatch(GDataSetErr);
  }
  if (strcmp(GDSName(&gdataset), "testGDataSet") != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSName failed");
    PBErrCatch(GDataSetErr);
  }
  if (GDSGetType(&gdataset) != GDataSetType_VecFloat) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetType failed");
    PBErrCatch(GDataSetErr);
  }
  if (GDSGetNbCat(&gdataset) != 1) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetNbCat failed");
    PBErrCatch(GDataSetErr);
  }
  if (GDSGetSize(&gdataset) != 3) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetSize failed");
    PBErrCatch(GDataSetErr);
  }
  if (GDSGetSizeCat(&gdataset, 0) != 3) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetSizeCat failed");
    PBErrCatch(GDataSetErr);
  }
  if ((GSet*)GDSSamples(&gdataset) != &(gdataset._dataSet._samples)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSSamples failed");
    PBErrCatch(GDataSetErr);
  }
  VecShort* dim = VecShortCreate(1);
  VecSet(dim, 0, 2);
  if (VecIsEqual(GDSSampleDim(&gdataset), dim) != true) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSSampleDim failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&dim);
  VecFloat* mean = GDSGetMean(&gdataset);
  VecFloat2D checkMean = VecFloatCreateStatic2D();
  VecSet(&checkMean, 0, 2.0);
  VecSet(&checkMean, 1, 3.0);
  if (!VecIsEqual(mean, &checkMean)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetMean failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&mean);
  GDSMeanCenter(&gdataset);
  VecFloat2D checkMeanCenter[3];
  for (int i = 0; i < GDSGetSize(&gdataset); ++i) {
    checkMeanCenter[i] = VecFloatCreateStatic2D();
    VecSet(checkMeanCenter + i, 0, -2.0 + (float)i * 2.0);
    VecSet(checkMeanCenter + i, 1, -2.0 + (float)i * 2.0);
  }
  GSetIterForward iter = GSetIterForwardCreateStatic(
    GDSSamples(&gdataset));
  int i = 0;
  do {
    VecFloat* sample = GSetIterGet(&iter);
    if (!VecIsEqual(sample, checkMeanCenter + i)) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSMeanCenter failed");
      PBErrCatch(GDataSetErr);
    }
  } while (GSetIterStep(&iter) && ++i);
  
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatGet OK\n");
}

void UnitTestGDataSetVecFloatSplitUnsplit() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  VecShort* split = VecShortCreate(2);
  VecSet(split, 0, 1);
  VecSet(split, 1, 2);
  GDSSplit(&gdataset, split);
  if (GDSGetNbCat(&gdataset) != 2) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSSplit failed");
    PBErrCatch(GDataSetErr);
  }
  if (GDSGetSizeCat(&gdataset, 0) != 1 ||
    GDSGetSizeCat(&gdataset, 1) != 2) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSSplit failed");
    PBErrCatch(GDataSetErr);
  }
  GDSUnsplit(&gdataset);
  if (GDSGetNbCat(&gdataset) != 1) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSUnsplit failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&split);
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatSplitUnsplit OK\n");
}

void UnitTestGDataSetVecFloatShuffle() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  GDSShuffle(&gdataset, 0);
  GDataSet* g = (GDataSet*)(&gdataset);
  if (GSetGet(g->_categories, 0) != GSetGet(&(g->_samples), 1)/* ||
    GSetGet(g->_categories, 1) != GSetGet(&(g->_samples), 0) ||
    GSetGet(g->_categories, 2) != GSetGet(&(g->_samples), 1)*/) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSShuffle failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatShuffle OK\n");
}

void UnitTestGDataSetVecFloatStepSampleGetSample() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  int iSample = 0;
  float check[6] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
  do {
    VecFloat* sample = GDSGetSample(&gdataset, 0);
    if (ISEQUALF(VecGet(sample, 0), check[iSample * 2]) == false ||
      ISEQUALF(VecGet(sample, 1), check[iSample * 2 + 1]) == false) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetSample failed");
      PBErrCatch(GDataSetErr);
    }
    VecFree(&sample);
    ++iSample;
  } while (GDSStepSample(&gdataset, 0));
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatStepSampleGetSample OK\n");
}

void UnitTestGDataSetVecFloatCovariance() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloatCovariance.json";
  GDataSetVecFloat gdataset = GDataSetVecFloatCreateStatic(cfgFilePath);
  MatFloat* covariance = GDSGetCovarianceMatrix(&gdataset);
  float v[9] = {
    6.888888, 6.0, 5.111111, 
    6.0, 6.0, 6.0, 
    5.111111, 6.0, 6.888888};
  VecShort2D i = VecShortCreateStatic2D();
  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(&dim, 0, 3);
  VecSet(&dim, 1, 3);
  int j = 0;
  do {
    if (!ISEQUALF(MatGet(covariance, &i), v[j])) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetCovarianceMatrix failed");
      PBErrCatch(GDataSetErr);
    }
    ++j;
  } while (VecStep(&i, &dim));
  MatFree(&covariance);
  GDataSetVecFloatFreeStatic(&gdataset);
    printf("UnitTestGDataSetVecFloatCovariance OK\n");
}

void UnitTestGDataSetVecFloat() {
  UnitTestGDataSetVecFloatCreateFreeClone();
  UnitTestGDataSetVecFloatGet();
  UnitTestGDataSetVecFloatSplitUnsplit();
  UnitTestGDataSetVecFloatShuffle();
  UnitTestGDataSetVecFloatStepSampleGetSample();
  UnitTestGDataSetVecFloatCovariance();
}

void UnitTestGDataSetGenBrushPair() {
  srandom(1);
  char* cfgFilePath = "testGDataSetGenBrushPair.json";
  GDataSetGenBrushPair gdataset = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  if (GDSGetNbMask(&gdataset) != 2) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetSample<GenBrushPair> failed");
    PBErrCatch(GDataSetErr);
  }
  int iCat = 0;
  do {
    GDSGenBrushPair* sample = GDSGetSample(&gdataset, iCat);
    if (VecIsEqual(GBDim(sample->_img), 
      GDSSampleDim(&gdataset)) == false ||
      VecIsEqual(GBDim(sample->_mask[0]), 
      GDSSampleDim(&gdataset)) == false ||
      VecIsEqual(GBDim(sample->_mask[1]), 
      GDSSampleDim(&gdataset)) == false) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetSample<GenBrushPair> failed");
      PBErrCatch(GDataSetErr);
    }
    GDSGenBrushPairFree(&sample);
  } while (GDSStepSample(&gdataset, iCat));
  GDataSetGenBrushPairFreeStatic(&gdataset);
  printf("UnitTestGDataSetGenBrushPair OK\n");
}

void UnitTestSDSIA() {
  srandom(1);
  char* cfgFilePath = "../SDSIA/UnitTestOut/002/001/dataset.json";
  GDataSetGenBrushPair gdataset = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  int iCat = 0;
  do {
    GDSGenBrushPair* sample = GDSGetSample(&gdataset, iCat);
    if (VecIsEqual(GBDim(sample->_img), 
      GDSSampleDim(&gdataset)) == false ||
      VecIsEqual(GBDim(sample->_mask[0]), 
      GDSSampleDim(&gdataset)) == false ||
      VecIsEqual(GBDim(sample->_mask[1]), 
      GDSSampleDim(&gdataset)) == false) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetSample<GenBrushPair> failed");
      PBErrCatch(GDataSetErr);
    }
    GDSGenBrushPairFree(&sample);
  } while (GDSStepSample(&gdataset, iCat));
  GDataSetGenBrushPairFreeStatic(&gdataset);
  printf("UnitTestSDSIA OK\n");
}

void UnitTestAll() {
  UnitTestGDataSetVecFloat();
  UnitTestGDataSetGenBrushPair();
  UnitTestSDSIA();
}

int main(void) {
  UnitTestAll();
  return 0;
}

