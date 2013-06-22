struct design {
  int arrayWidth; // the network input width
  int type;       // type of network inputs (1 - floating point, 2 - int etc.)
  int wordLength; // the length (number of bits) of network inputs
  int exponent;   // the exponent size for floating point types
};

// returns true iff the FPGA design repository contains a network with
// the required width and word length
bool haveFPGADesign(design);

// provide an estimate of the execution time for the array width and
// number of arrays based on prior measured data (e.g. such as the
// performance table for varying sizes of N) or execution feedback (self-tunning)
double estimatedFPGAExecutionTime(arrayWidth, n);

// estimate CPU execution time based on prior runs or performance table
double estimatedCPUExecutionTime(arrayWidth, n);

// estimate reconfiguration time based on prior runs or performance table
double estimatedReconfigurationTime();

// estimated time to pad the n arrays of array_width to width of the loaded network
double estimatedPaddingTime();

// returns true iff loading the FPGA design for the required width and
// number of calls (n) improves the performance over either the software
// only version or using the existing design by padding the data to
// the existing network size
bool increasesPerformance(arrayWidth, n, requiredDesign, loadedDesign) {
  if ( esimatedCPUExecutionTime(arrayWidth, n) >
       estimatedFPGAExecutionTime(arrayWidth, n) +
       estimatedReconfigurationTime()) {
    return true;
  }

  if (compatible(requiredDesign, loadedDesign) &&
      estimatedCPUExecutionTime(arrayWidth, n) >
      estimatedFPGAExecutionTime(arrayWidth, n)
      + estimatedPaddingTime(requiredDesign.arrayWidth, n, loadedDesign.arrayWidth)) {
    return true;
  }

  return false;
}

// strategy for running the requiredDesign for n calls optimally
// based on estimated reconfiguration, FPGA and CPU execution time
void run(requiredDesign, n) {
  loadedDesign  = getLoadedDesign();

  if ( haveFPGADesign(requiredDesign)  &&
       increasesPerformance(arrayWidth, n, requiredDesign, loadedDesign)) {

    if ( !compatible(requiredDesign, loadedDesign) ||
         estimatedPaddingTime(arrayWidth, n) +
         estimatedExecutionTime(arrayWidth, n, loadedDesign.wordLength) >
         estimatedReconfigurationTime() +
         estimatedExecutionTime(arrayWidth, n, requiredWordLength))
      loadFPGADesign(requiredDesign);

    runFPGADesign(array);
  } else {
    cpuSort(n, k, array);
  }
}
