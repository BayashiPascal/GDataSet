#include <stdlib.h>
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
  VecFloat* max = GDSGetMax(&gdataset);
  VecFloat2D checkMax = VecFloatCreateStatic2D();
  VecSet(&checkMax, 0, 4.0);
  VecSet(&checkMax, 1, 5.0);
  if (!VecIsEqual(max, &checkMax)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSGetMax failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&max);
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
  VecLong* split = VecLongCreate(2);
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
  if (GDSGetSizeCat(&gdataset, 0) != GSetNbElem(gdataset._dataSet._categories) ||
    GDSGetSizeCat(&gdataset, 1) != GSetNbElem(gdataset._dataSet._categories + 1)) {
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
  GDSShuffleCat(&gdataset, 0);
  GDataSet* g = (GDataSet*)(&gdataset);
  if (GSetGet(g->_categories, 0) != GSetGet(&(g->_samples), 1)/* ||
    GSetGet(g->_categories, 1) != GSetGet(&(g->_samples), 0) ||
    GSetGet(g->_categories, 2) != GSetGet(&(g->_samples), 1)*/) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSShuffleCat failed");
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
    VecFloat* inputs = GDSGetSampleInputs(&gdataset, 0);
    VecFloat* outputs = GDSGetSampleOutputs(&gdataset, 0);
    if (ISEQUALF(VecGet(inputs, 0), check[iSample * 2]) == false) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetSampleInputs failed");
      PBErrCatch(GDataSetErr);
    }
    if (ISEQUALF(VecGet(outputs, 0), check[iSample * 2 + 1]) == false) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetSampleOutputs failed");
      PBErrCatch(GDataSetErr);
    }
    VecFree(&inputs);
    VecFree(&outputs);
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

void VecFloatCSVImporter(
  int col, 
  char* val,
  VecFloat* sample) {
  if (col == 0) {
    if (*val == 'A') {
      VecSet(sample, col, 10.0);
    } else if (*val == 'B') {
      VecSet(sample, col, 20.0);
    }
  } else {
    VecSet(sample, col, atof(val));
  }
}

void UnitTestGDataSetVecFloatCreateFromCSVSave() {
  char* csvPath = "./unitTestVecFloatCSV.csv";
  GDSVecFloatCSVImporter importer = 
    GDSVecFloatCSVImporterCreateStatic(
      1,
      ',',
      3,
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
  if (GDSSave(&dataset, stream, false) == false) {
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

void UnitTestGDataSetVecFloatSaveCategory() {
  char* csvPath = "./unitTestVecFloatCSV.csv";
  GDSVecFloatCSVImporter importer = 
    GDSVecFloatCSVImporterCreateStatic(
      1,
      ',',
      3,
      3,
      &VecFloatCSVImporter);
  GDataSetVecFloat dataset = 
    GDataSetCreateStaticFromCSV(
      csvPath, 
      &importer);
  VecLong2D split = VecLongCreateStatic2D();
  VecSet(&split, 0, 1);
  VecSet(&split, 1, 2);
  GDSSplit(&dataset, &split);
  FILE* stream = fopen("./unitTestVecFloatSaveCategory.json", "w");
  GDSVecFloatSaveCategory(
    &dataset, 
    stream, 
    true, 
    1);
  fclose(stream);
  GDataSetVecFloat load = 
    GDataSetVecFloatCreateStaticFromFile(
      "./unitTestVecFloatSaveCategory.json");
  float check[] = {
    10.0,3.0,4.0,
    20.0,5.0,6.0};
  for (int iSample = 0; iSample < GDSGetSize(&load); ++iSample) {
    for (unsigned int iCol = 0; iCol < importer._nbCol; ++iCol) {
      if (ISEQUALF(check[iSample * importer._nbCol + iCol],
        VecGet(GSetGet(GDSSamples(&load), iSample), iCol)) == false) {
        GDataSetErr->_type = PBErrTypeUnitTestFailed;
        sprintf(GDataSetErr->_msg, "GDataSetVecFloatSaveCategory failed");
        PBErrCatch(GDataSetErr);
      }
    }
  }
  GDataSetVecFloatFreeStatic(&dataset);
  GDataSetVecFloatFreeStatic(&load);
  printf("UnitTestGDataSetVecFloatSaveCategory OK\n");
}

void UnitTestGDataSetVecFloatAddRemoveSample() {
  char* cfgFilePath = "testGDataSetVecFloatNormalize.json";
  GDataSetVecFloat dataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
  VecLong2D split = VecLongCreateStatic2D();
  VecSet(&split, 0, 1);
  VecSet(&split, 1, 2);
  GDSSplit(&dataset, &split);
  GDSRemoveAllSample(&dataset);
  if (GDSGetNbCat(&dataset) != 1 ||
    GDSGetSize(&dataset) != 0) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSVecFloatRemoveAllSample failed");
    PBErrCatch(GDataSetErr);
  }
  VecFloat* sample = VecFloatCreate(3);
  GDSAddSample(&dataset, sample);
  if (GDSGetNbCat(&dataset) != 1 ||
    GDSGetSize(&dataset) != 1) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSVecFloatAddSample failed");
    PBErrCatch(GDataSetErr);
  }
  GDataSetVecFloatFreeStatic(&dataset);
  printf("UnitTestGDataSetVecFloatAddRemoveSample OK\n");
}

void UnitTestGDataSetVecFloatProxMatNearestNeighbour() {
  char* cfgFilePath = "testGDataSetVecFloat.json";
  GDataSetVecFloat dataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
  MatFloat* proxMat = GDSGetProxMat(&dataset, 0);
  float check[3][3] = {{0.0,2.828427,5.656854},{2.828427,0.0,2.828427},{5.656854,2.828427,0.0}};
  VecShort2D v = VecShortCreateStatic2D();
  do {
    if (!ISEQUALF(check[VecGet(&v, 0)][VecGet(&v, 1)], 
      MatGet(proxMat, &v))) {
      GDataSetErr->_type = PBErrTypeUnitTestFailed;
      sprintf(GDataSetErr->_msg, "GDSGetProxMat failed");
      PBErrCatch(GDataSetErr);
    }
  } while(VecStep(&v, MatDim(proxMat)));
  VecFloat2D target = VecFloatCreateStatic2D();
  VecSet(&target, 0, 6.0);
  VecSet(&target, 1, 1.5);
  VecFloat* nearest =
    GDSNearestNeighbour(&dataset, (VecFloat*)&target, 0);
  VecFloat2D checkb = VecFloatCreateStatic2D();
  VecSet(&checkb, 0, 4.0);
  VecSet(&checkb, 1, 5.0);
  if (!VecIsEqual(nearest, &checkb)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSNearestNeighbour failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&nearest);
  MatFree(&proxMat);
  GDataSetVecFloatFreeStatic(&dataset);

  srandom(time(NULL));
  for (int size = 100; size < 10000; size *= 2) {
    dataset = GDataSetVecFloatCreateStatic();
    dataset._dataSet._sampleDim = VecShortCreate(1);
    VecSet(dataset._dataSet._sampleDim, 0, 3);
    for (int iSample = size; iSample--;) {
      VecFloat* sample = VecFloatCreate(3);
      VecSet(sample, 0, (0.5 - rnd()) * 100.0);
      VecSet(sample, 1, (0.5 - rnd()) * 100.0);
      VecSet(sample, 2, (0.5 - rnd()) * 100.0);
      GDSAddSample(&dataset, sample);
    }
    VecLong* split = VecLongCreate(1);
    VecSet(split, 0, size);
    GDSSplit(&dataset, split);
    VecFree(&split);
    float delayAESA = 0.0;
    float delayBruteForce = 0.0;
    for (int iCheck = 10; iCheck--;) {
      VecFloat3D target3d = VecFloatCreateStatic3D();
      VecSet(&target3d, 0, (0.5 - rnd()) * 100.0);
      VecSet(&target3d, 1, (0.5 - rnd()) * 100.0);
      VecSet(&target3d, 2, (0.5 - rnd()) * 100.0);

      GDSResetAll(&dataset);
      clock_t clockBefore = clock();
      VecFloat* checkNearest = GDSVecFloatNearestNeighbourBrute(
        &dataset, (VecFloat*)&target3d, 0);
      clock_t clockAfter = clock();
      delayBruteForce += ((double)(clockAfter - clockBefore)) / 
        CLOCKS_PER_SEC * 1000.0;

      clockBefore = clock();
      nearest = GDSVecFloatNearestNeighbourAESA(
        &dataset, (VecFloat*)&target3d, 0);
      clockAfter = clock();
      delayAESA += ((double)(clockAfter - clockBefore)) / 
        CLOCKS_PER_SEC * 1000.0;

      if (!VecIsEqual(nearest, checkNearest)) {
        GDataSetErr->_type = PBErrTypeUnitTestFailed;
        sprintf(GDataSetErr->_msg, "GDSNearestNeighbour failed");
        PBErrCatch(GDataSetErr);
      }
      VecFree(&checkNearest);
    }
    printf("size: %d, ratio bruteForce/AESA: %f\n", 
      size, delayBruteForce / delayAESA);
    GDataSetVecFloatFreeStatic(&dataset);
  }
  printf("UnitTestGDataSetVecFloatProxMatNearestNeighbour OK\n");
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
  UnitTestGDataSetVecFloatSaveCategory();
  UnitTestGDataSetVecFloatAddRemoveSample();
  UnitTestGDataSetVecFloatProxMatNearestNeighbour();
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

void UnitTestNNEvalCSVImporter(
        int col, 
      char* val,
  VecFloat* sample) {
  if (col == 0) {
    if (*val == 'M') {
      VecSet(sample, 0, 1.0);
      VecSet(sample, 1, 0.0);
      VecSet(sample, 2, 0.0);
    } else if (*val == 'F') {
      VecSet(sample, 0, 0.0);
      VecSet(sample, 1, 1.0);
      VecSet(sample, 2, 0.0);
    } else if (*val == 'I') {
      VecSet(sample, 0, 0.0);
      VecSet(sample, 1, 0.0);
      VecSet(sample, 2, 1.0);
    }
  } else {
    VecSet(sample, col + 2, atof(val));
  }
}

void UnitTestGDataSetNNEval() {
  char* csvPath = "./unitTestNNEval.csv";
  GDSVecFloatCSVImporter importer = 
    GDSVecFloatCSVImporterCreateStatic(
      0,
      ' ',
      9,
      11,
      &UnitTestNNEvalCSVImporter);
  GDataSetVecFloat dataset = 
    GDataSetCreateStaticFromCSV(csvPath, &importer);
  NeuraNet* nn = NULL;
  FILE* fp = fopen("./unitTestNNEval.json", "r");
  (void)NNLoad(&nn, fp);
  fclose(fp);
  VecShort* inputs = VecShortCreate(10);
  for (int i = 10; i--;)
    VecSet(inputs, i, i);
  VecShort* outputs = VecShortCreate(1);
  VecSet(outputs, 0, 10);
  int iCat = 0;
  float threshold = -1000.0;
  float val = GDSEvaluateNN(
    &dataset, 
    nn, 
    iCat, 
    inputs, 
    outputs, 
    threshold);
  if (!ISEQUALF(val, -1.672186)) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSEvaluateNN failed");
    PBErrCatch(GDataSetErr);
  }
  VecFree(&inputs);
  VecFree(&outputs);
  NeuraNetFree(&nn);
  GDataSetVecFloatFreeStatic(&dataset);
  printf("UnitTestNNEval OK\n");
}

void UnitTestGDataSetOutliers() {

  srandom(1);
  char* cfgFilePath = "testGDataSetVecFloatOutlier.json";
  GDataSetVecFloat dataset = 
    GDataSetVecFloatCreateStaticFromFile(cfgFilePath);
  bool isOutlier =
    GDSIsOutlierSampleCat(
      &dataset,
      GDSGetSample(&dataset, 0),
      0,
      0);
  if (!isOutlier) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSIsOutlierSampleCat failed (1)");
    PBErrCatch(GDataSetErr);
  }
  isOutlier =
    GDSIsOutlierSampleCat(
      &dataset,
      GDSGetSample(&dataset, 0),
      0,
      1);
  if (isOutlier) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSIsOutlierSampleCat failed (2)");
    PBErrCatch(GDataSetErr);
  }
  GDSStepSample(&dataset, 0);
  isOutlier =
    GDSIsOutlierSampleCat(
      &dataset,
      GDSGetSample(&dataset, 0),
      0,
      0);
  if (isOutlier) {
    GDataSetErr->_type = PBErrTypeUnitTestFailed;
    sprintf(GDataSetErr->_msg, "GDSIsOutlierSampleCat failed (3)");
    PBErrCatch(GDataSetErr);
  }

  GDataSetVecFloatFreeStatic(&dataset);

  printf("UnitTestGDataSetOutliers OK\n");
}

void UnitTestAll() {
  UnitTestGDataSetVecFloat();
  UnitTestGDataSetGenBrushPair();
  UnitTestGDataSetNNEval();
  UnitTestGDataSetOutliers();
  UnitTestSDSIA();
}

int main(void) {
  //UnitTestAll();
  UnitTestGDataSetOutliers();

  return 0;
}

