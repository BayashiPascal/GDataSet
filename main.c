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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
  GDataSet* g = (GDataSet*)(&gdataset);
  if (GSetGet(g->_categories, 0) != GSetGet(&(g->_samples), 0) ||
    GSetGet(g->_categories, 1) != GSetGet(&(g->_samples), 1) ||
    GSetGet(g->_categories, 2) != GSetGet(&(g->_samples), 2)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDataSetCreateStatic failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSetVecFloat clone = GDSClone(&gdataset);
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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
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
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
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

void UnitTestGDataSetVecFloatNormalize() {
  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloatNormalize.json";
  GDataSetVecFloat gdataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
  GDSNormalize(&gdataset);
  GSetIterForward iter = 
    GSetIterForwardCreateStatic(GDSSamples(&gdataset));
  float check[9] = {
      0.267261,0.534522,0.801784,
      0.683763,0.569803,0.455842,
      0.502571,0.574367,0.646162
    };
  int i = 0;
  do {
    VecFloat* v = GSetIterGet(&iter);
    if (!ISEQUALF(VecGet(v, 0), check[i * 3]) ||
      !ISEQUALF(VecGet(v, 1), check[i * 3 + 1]) ||
      !ISEQUALF(VecGet(v, 2), check[i * 3 + 2])) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSNormalize failed");
      PBErrCatch(GDataSetErr);
    }
    ++i;
  } while(GSetIterStep(&iter));
  GDataSetVecFloatFreeStatic(&gdataset);
  printf("UnitTestGDataSetVecFloatNormalize OK\n");
}

float VecFloatCSVImporter(int col, char* val) {
  if (col == 0) {
    if (*val == 'A') {
      return 10.0;
    } else if (*val == 'B') {
      return 20.0;
    }
  } else {
    return atof(val);
  }
  return 0.0;
}

void UnitTestGDataSetVecFloatCreateFromCSVSave() {
  char* csvPath = "./unitTestVecFloatCSV.csv";
  GDSVecFloatCSVImporter importer = 
    GDSVecFloatCSVImporterCreateStatic(
      1,
      ',',
      3,
      &VecFloatCSVImporter);
  GDataSetVecFloat dataset = 
    GDataSetCreateStaticFromCSV(csvPath, &importer);
  if (GDSGetSize(&dataset) != 3 ||
    GDSGetNbCat(&dataset) != 1) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDataSetCreateStaticFromCSV failed");
    PBErrCatch(GDataSetErr);
  }
  float check[] = {
    10.0,1.0,2.0,
    10.0,3.0,4.0,
    20.0,5.0,6.0};
  for (int iSample = 0; iSample < GDSGetSize(&dataset); ++iSample) {
    for (unsigned int iCol = 0; iCol < importer._nbCol; ++iCol) {
      if (ISEQUALF(check[iSample * importer._nbCol + iCol],
        VecGet(GSetGet(GDSSamples(&dataset), iSample), iCol)) == false) {
        GDataSetErr->_type = PBErrTypeUnitTestFailed;
        sprintf(GDataSetErr->_msg, "GDataSetCreateStaticFromCSV failed");
        PBErrCatch(GDataSetErr);
      }
    }
  }
  FILE* stream = fopen("./unitTestVecFloatSave.json", "w");
  if (GDataSetVecFloatSave(&dataset, stream, false) == false) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDataSetVecFloatSave failed");
    PBErrCatch(GDataSetErr);
  }
  fclose(stream);
  GDataSetVecFloat load = 
    GDataSetVecFloatCreateStaticFromFile("./unitTestVecFloatSave.json");
  for (int iSample = 0; iSample < GDSGetSize(&dataset); ++iSample) {
    for (unsigned int iCol = 0; iCol < importer._nbCol; ++iCol) {
      if (ISEQUALF(check[iSample * importer._nbCol + iCol],
        VecGet(GSetGet(GDSSamples(&load), iSample), iCol)) == false) {
        GDataSetErr->_type = PBErrTypeUnitTestFailed;
        sprintf(GDataSetErr->_msg, "GDataSetVecFloatSave failed");
        PBErrCatch(GDataSetErr);
      }
    }
  }
  GDataSetVecFloatFreeStatic(&dataset);
  GDataSetVecFloatFreeStatic(&load);
  printf("UnitTestGDataSetVecFloatCreateFromCSVSave OK\n");
}

void UnitTestGDataSetVecFloat() {
  UnitTestGDataSetVecFloatCreateFreeClone();
  UnitTestGDataSetVecFloatGet();
  UnitTestGDataSetVecFloatSplitUnsplit();
  UnitTestGDataSetVecFloatShuffle();
  UnitTestGDataSetVecFloatStepSampleGetSample();
  UnitTestGDataSetVecFloatCovariance();
  UnitTestGDataSetVecFloatNormalize();
  UnitTestGDataSetVecFloatCreateFromCSVSave();
}

void UnitTestGDataSetGenBrushPair() {
  srandom(1);
  char* cfgFilePath = "testGDataSetGenBrushPair.json";
  GDataSetGenBrushPair gdataset = 
    GDataSetGenBrushPairCreateStaticFromFile(cfgFilePath);
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
    GDataSetGenBrushPairCreateStaticFromFile(cfgFilePath);
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

